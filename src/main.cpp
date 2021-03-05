#include "ConDecomp_LaPSO_Connector.h"
#include "ConstraintFileProcessing.h"
#include "Decomp.hpp"
#include "DecompMIP.h"
#include "Decomposition_Statistics.h"
#include "GreedyDecompCreator.h"
#include "Hypergraph.h"
#include "LRHandler.h"
#include "MIPProblemProbe.h"
#include "MIP_Fileparser.h"
#include "MIP_Problem_CPLEX_Solver.h"
#include "MIP_to_Hypergraph.h"
#include "Param.h"
#include "ParamAdapter.h"
#include "Problem_Adapter.h"
#include "Random.h"
#include "SolveGenericMIP.h"
#include "Testing.h"
#include "Util.h"
#include "WriteResults.h"
#include "Writer.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/nsga2.hpp>
#include <pagmo/archipelago.hpp>
#include <pagmo/problem.hpp>
#include <pagmo/utils/multi_objective.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Decomposition_Statistics::Instance;
using Decomposition_Statistics::LROutputs;
using Decomposition_Statistics::RelaxedConstraints;
using Decomposition_Statistics::Subproblems;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace pagmo;

void solveLapso(int& argc, const char** argv, MIP_Problem& MP, MIPProblemProbe& MPP, Hypergraph& HG, mainParam::Param& para,
    ParamAdapter& PA, vector<int>& con_relax_vector,
    const LaPSOOutputFilenames& LOF, int decomposition_idx,
    const std::vector<initial_dual_value_pair>& original_intial_dual_value_pairs, bool set_initial_dual_values = false,
    bool capture_statistics = false)
{
    // calculate number of constraints relaxed and
    // create vector of constraint indices which are relaxed
    int LR_iter_limit = para.maxIter;
    double total_LR_time_lim = para.total_LR_time_lim;
    bool debug_printing = PA.get_debug_printing_flag();

    int num_con_relaxed = con_relax_vector.size();
    // writer object to write necessary information to files
    Writer w;
    // test partition if required
    bool test_hypergraph_partitioning = false;
    //generate instance statistics
    if (capture_statistics) {
        //generate relaxed constraint statistics
        std::shared_ptr<RelaxedConstraints> rcs_ptr = std::make_shared<RelaxedConstraints>(decomposition_idx);
        rcs_ptr->generate_statistics(MPP, con_relax_vector);
        // write out raw relaxed constraint statistics
        w.writeRawRelaxedConstraintStatistics(LOF, rcs_ptr);
        w.writeRelaxedConstraintSingleValues(LOF, rcs_ptr);
        // free up memory instead of storing all raw data
        rcs_ptr.reset();
    }
    // subproblem_statistics structure is used
    std::shared_ptr<Subproblems> ss_ptr = std::make_shared<Subproblems>(decomposition_idx);
    // generate the different subproblem structures from relaxing the constaints. Structures are node and edge idx's in each subproblem
    std::vector<Partition_Struct> ps = HG.getPartitionStruct(con_relax_vector, test_hypergraph_partitioning);
    ss_ptr->resize(ps.size());

    // the LaPSO method
    if (debug_printing) {
        MP.printObjectiveFn();
        MP.printConstraints();
        MP.printVariables();
    }
    // based on the partitioned structures, calculate variable/constraint information.
    if (capture_statistics) {
        for (int partition_idx = 0; partition_idx < ps.size(); ++partition_idx) {
            ss_ptr->generateBlockStatistics(ps[partition_idx], MPP);
        }
    }
    // store the connector requirements
    ConnectorRequirements CR = { &MP, &ps, &con_relax_vector, debug_printing, total_LR_time_lim, ss_ptr };
    // number of subproblems
    cout << "CR ss_ptr size is " << CR.ss_ptr->mip_obj_solutions.size() << endl;
    // initialise the LR Handler
    LRHandler LRH;
    // create the LaPSO connector
    LRH.initConnector(CR);
    // create the parameter requirements to set up LaPSO
    std::shared_ptr<ConDecomp_LaPSO_Connector> CLC_ptr = LRH.getCLCPointer();
    LaPSO::LaPSORequirements LR = generateLaPSORequirements(CLC_ptr, MP, original_intial_dual_value_pairs, num_con_relaxed);
    // initialise the LaPSO solver
    // Create the LaPSO Handler object which controls LaPSO process
    cout << "Setting up LaPSO Handler" << endl;
    LRH.initLaPSOSolver(argc, argv, LR);
    // solve Lagrangian Relaxation
    cout << "solving Lagrangian Relaxation" << endl;
    LRH.solve();
    if (PA.get_run_MIP_Duals_testing_flag()) {
        // get the initial dual indicie values to see where the discrepancy arrises from
        LaPSO::IncorrectInitialDualIndices IIDI = LRH.getIncorrectInitialDualIndicies();
        // print out dual index
        // print out dual index as original index
        // print out constraint of that original index
        cout << "for the dual variables with lower bound problems " << endl;
        for (auto& lower_bound_pair : IIDI.lower_bound_errors) {
            cout << "Dual index is " << lower_bound_pair.first << endl;
            int original_idx = CLC_ptr->getOriginalIdxFromDual(lower_bound_pair.first);
            cout << "Original index is " << original_idx << endl;
            MP.getConstraint(original_idx).printInfo();
        }
    }
    // get the bounds and solve times for the Lagrangian Relaxation process
    std::tuple<double, double> LaPSO_outputs = LRH.getLaPSOOutputs();
    std::shared_ptr<LROutputs> lro_ptr = std::make_shared<LROutputs>(decomposition_idx, get<0>(LaPSO_outputs), get<1>(LaPSO_outputs));
    if (capture_statistics) {
        w.writeRawSubproblemStatistics(LOF, ss_ptr);
        w.writeLROutputs(LOF, lro_ptr);
    }
}

void writeConVecToFile(const vector<double>& con_vec, string filename)
{
    std::ofstream outfile;
    outfile.open(filename);
    for (auto& con_val : con_vec) {
        outfile << con_val << ",";
    }
    outfile.close();
}

int main(int argc, const char** argv)
{
    // Parse in parameters
    mainParam::Param para;
    para.parse(argc, argv);

    // Manipulate/transform parameters from char to required values through Adapter
    ParamAdapter PA(para);
    Writer w;
    string MIP_Problem_File = PA.getProblemFilename();

    // make sure mps file exists
    if (!fileExists(MIP_Problem_File)) {
        cout << "Input MPS file cannot be found" << endl;
        exit(EXIT_FAILURE);
    }

    // Parse in MIP file into self made structure
    MIP_Fileparser MIP_FP;
    cout << "Parsing MPS File" << endl;
    MIP_FP.parse(file_type::MPS, MIP_Problem_File);
    MIP_Problem MP = MIP_FP.getMIPProblem();

    bool printMIPProblem = false;
    if (printMIPProblem == true) {
        MP.printObjectiveFn();
        MP.printVariables();
        MP.printConstraints();
    }

    //convert parsed MIP to Hypergraph
    MIP_to_Hypergraph MTH;
    cout << "Converting MIP read to Hypergraph " << endl;
    MTH.convertToHypergraph(MP);
    Hypergraph HG(MTH.getHGEdges(), MTH.getHGNodes());

    Testing test_obj(argc, argv);

    if (PA.get_generic_MIP_Solver_Flag() == true || PA.get_run_MIP_Parse_testing_flag() == true) {
        test_obj.RunMIPTesting(MP);
    }

    if (PA.get_run_no_relaxed_constr_testing_flag() == true) {
        cout << "running no relaxed constraints test" << endl;
        test_obj.testEmptyConVec(MP, HG);
        exit(0);
    }

    //test constraint redundancy for decompositions
    if (PA.get_run_constraint_redundancy_flag()) {
        ConstraintFileProcessing CFP;
        CFP.removeRedundantConstraints(para.decomps_to_remove_red_const_file, para.redundant_const_removed_output_file, HG);
    }

    // remove duplicate constraints
    if (PA.get_run_remove_duplicate_constraints_flag()) {
        ConstraintFileProcessing CFP;
        CFP.removeDuplicateConstraints(para.decomps_to_remove_duplicates_file,
            para.duplicates_removed_output_file);
    }

    // run NSGA if desired
    if (PA.get_run_nsga_decomp_flag() == true) {
        Problem_Adapter ProblemAdapter;
        // write out to a file the different decompositions found
        bool print_objectives = false;
        cout << "pareto opt sol filepath is " << string(para.nsga_pareto_optimal_output_file) << endl;
        ProblemAdapter.createNSGADecomps(HG, para.nsga_gen, string(para.nsga_decomp_output_file), string(para.nsga_pareto_optimal_output_file), para.nsga_pop_size, print_objectives);
    }

    if (PA.get_run_gather_statistics_flag() == true) {
        // using the same instance as in the LR testing should give the same dual value as the first relaxtion tested.
        // get in the dual values for all constraints
        vector<initial_dual_value_pair> dual_values_from_LP;
        // convert parsed MIP into cplex model to solve and obtain dual values
        MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
        bool solve_as_LP = true;
        // solve MIP as LP
        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), solve_as_LP);
        for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
            dual_values_from_LP.push_back({ con_idx, MIP_results.dual_vals[con_idx] });
            // cout << "dual value for con " << con_idx << " = " << MIP_results.dual_vals[con_idx] << endl;
        }

        // write out LP results for parsed file
        Writer w;
        if (string(para.LP_outputs_folder) != "") {
            string LP_Output_file = string(para.LP_outputs_folder) + "/LP_outputs.csv";
            w.writeLPOutputs(LP_Output_file, MIP_results.obj_val, MIP_results.solve_time);
        }
        // initial dual values are going to be used
        bool set_initial_dual_values = true;
        // assign a starting decomposition index
        int decomposition_idx = 0;
        // read in con_vec from file
        std::ifstream input_fs(string(para.con_vec_filename));

        // assign output filenames
        LaPSOOutputFilenames LOF = {};
        LOF.subproblem_statistics_folder = string(para.subproblem_statistics_folder);
        LOF.relaxed_constraints_statistics_folder = string(para.relaxed_constraint_statistics_folder);
        LOF.instance_statistics_folder = string(para.instance_statistics_folder);
        LOF.LR_outputs_folder = string(para.LR_outputs_folder);

        // MIPProblem probe object used to get statistics from MIP problem
        MIPProblemProbe MPP(&MP);
        if (string(para.instance_statistics_folder) != "") {
            // capture and write out instance statistics
            std::shared_ptr<Instance> ins_ptr = std::make_shared<Instance>(MPP);
            w.writeInstanceStatistics(LOF, ins_ptr);
        }
        // input file successfully opened
        if (input_fs) {
            string line_read;
            while (getline(input_fs, line_read)) {
                vector<string> relaxed_constraints_str;
                vector<int> relaxed_constraints_int;
                // split the line based on ,
                boost::split(relaxed_constraints_str, line_read, boost::is_any_of(","), boost::token_compress_on);

                // first line contains the number of nodes
                // last element will be empty because of ending final comma
                for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
                    try {
                        relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
                    } catch (const std::invalid_argument& ia) {
                        if (relaxed_constraints_str[i] == "") {
                            break;
                        } else {
                            cout << "invalid relaxed constraint number parsed in input con vec - " << relaxed_constraints_str[i] << endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                }
                bool capture_statistics = true;
                solveLapso(argc, argv, MP, MPP, HG, para, PA, relaxed_constraints_int, LOF, decomposition_idx, dual_values_from_LP, set_initial_dual_values, capture_statistics);
                ++decomposition_idx;
            }
        } else {
            cout << "constraint input file unable to be found/opened" << endl;
        }
        exit(EXIT_SUCCESS);
    }

    // Run greedy decomp creation
    if (PA.get_run_greedy_decomp_flag()) {
        cout << "running greedy decomposition testing" << endl;
        GreedyDecompCreator GDC;
        vector<double> rc_props = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9};
        std::ofstream outfile;
        outfile.open(para.greedy_decomp_output_file, std::ofstream::app);
        int desired_decomp_num_per_rcprop = para.greedy_decomp_size / rc_props.size();
        if (outfile) {
            // write out greedy decomposition as relaxed constraints to specified file
            for (const auto& rc_prop : rc_props) {
                for (int decomp_num = 0; decomp_num < desired_decomp_num_per_rcprop; ++decomp_num) {
                    vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, rc_prop);
                    for (int i = 0; i < greedy_decomp.size(); ++i) {
                        if (greedy_decomp[i] == true) {
                            outfile << i << ",";
                        }
                    }
                    outfile << endl;
                }
            }
            outfile.close();
        }
    }

    return 0;
}