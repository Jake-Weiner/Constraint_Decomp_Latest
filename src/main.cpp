#include "ConDecomp_LaPSO_Connector.h"
#include "Decomp.hpp"
#include "DecompMIP.h"
#include "GreedyDecompCreator.h"
#include "Hypergraph.h"
#include "LaPSOHandler.h"
#include "MIPProblemProbe.h"
#include "MIP_Fileparser.h"
#include "MIP_Problem_CPLEX_Solver.h"
#include "MIP_to_Hypergraph.h"
#include "Param.h"
#include "ParamAdapter.h"
#include "Problem_Adapter.h"
#include "Random.h"
#include "SolveGenericMIP.h"
#include "Util.h"
#include "WriteResults.h"
#include "Writer.h"
#include "ConstraintFileProcessing.h"
#include "Decomposition_Statistics.h"

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

using std::cout;
using std::endl;
using std::string;
using std::vector;
using Decomposition_Statistics::Subproblems;
using Decomposition_Statistics::RelaxedConstraints;
using Decomposition_Statistics::Instance;

using namespace pagmo;

void solveDecompMIP(Hypergraph& HG, const double subproblem_prop, const char* output_filename, bool warm_start, const char* warm_start_filename)
{
    // solve decomposition using MIP
    DecompMIP DM;
    DecompInfo DI;

    DI = DM.solveMIP(HG.getHGEdges(), HG.getHGNodes(), subproblem_prop * HG.getNumNodes(),
        50, true, output_filename, 1800, warm_start, warm_start_filename);
}

vector<double> getCplexConVector(string cplex_filename)
{
    vector<double> test_partition;
    ifstream input(cplex_filename);
    if (input.is_open()) {
        bool vector_reached = false;
        while (!input.eof()) {
            string line;
            getline(input, line);
            if (line.empty()) {
                continue;
            }
            trim(line);
            vector<string> line_split;
            boost::split(line_split, line, boost::is_any_of(" \t"), boost::token_compress_on);

            if (line.find("vector") != std::string::npos) {
                vector_reached = true;
                continue;
            }
            if (vector_reached == true) {
                boost::split(line_split, line, boost::is_any_of(","), boost::token_compress_on);
                for (auto& val : line_split) {
                    boost::erase_all(val, "[");
                    boost::erase_all(val, "]");
                    double constraint_val;
                    try {
                        constraint_val = stod(val);
                        test_partition.push_back(constraint_val);
                    } catch (...) {
                    }
                    cout << val << " ";
                }
                cout << endl;
            }
        }
    }
    return test_partition;
}

void solveLapso(int& argc, const char** argv, MIP_Problem& MP, Hypergraph& HG, const vector<bool>& con_relax_vector,
    const double& best_ub_sol, const LaPSOOutputFilenames& LOF, int decomposition_idx,
    const double sp_solver_time_limit, const int LR_iter_limit, const std::vector<initial_dual_value_pair>& original_intial_dual_value_pairs, bool set_initial_dual_values = false,
    bool debug_printing = false)
{

    // calculate number of constraints relaxed and
    // create vector of constraint indices which are relaxed
    int num_con_relaxed = 0;
    vector<int> relaxed_constraint_indices;
    for (int con_idx = 0; con_idx < con_relax_vector.size(); ++con_idx) {
        if (con_relax_vector[con_idx]) {
            ++num_con_relaxed;
            relaxed_constraint_indices.push_back(con_idx);
        }
    }

    // writer object to write necessary information to files
    Writer w;
    // test partition if required
    bool test_hypergraph_partitioning = false;

    // MIPProblem probe object used to get statistics from MIP problem
    MIPProblemProbe MPP(&MP);

    //generate instance statistics
    std::shared_ptr<Instance> ins_ptr = std::make_shared<Instance>(MPP);

    w.writeInstanceStatistics(LOF, ins_ptr);

    //generate relaxed constraint statistics
    std::shared_ptr<RelaxedConstraints> rcs_ptr = std::make_shared<RelaxedConstraints>(decomposition_idx);

    rcs_ptr->generate_statistics(MPP, relaxed_constraint_indices);

    // write out raw relaxed constraint statistis
    w.writeRawRelaxedConstraintStatistics(LOF, rcs_ptr);

    // free up memory instead of storing all raw data
    rcs_ptr.reset();

    // subproblem_statistics structure is used
    std::shared_ptr<Subproblems> ss_ptr = std::make_shared<Subproblems>(decomposition_idx);
    // assign decomposition index
  
    // generate the different subproblem structures from relaxing the constaints. Structures are node and edge idx's in each subproblem
    std::vector<Partition_Struct> ps = HG.getPartitionStruct(con_relax_vector, test_hypergraph_partitioning);

    // the LaPSO method
    debug_printing = false;
    ConDecomp_LaPSO_Connector CLC(MP, ps, con_relax_vector, debug_printing, sp_solver_time_limit, ss_ptr);

    // based on the partitioned structures, calculate variable/constraint information.

    CLC.maxsolves = LR_iter_limit;
    CLC.nsolves = 0;

    for (int partition_idx = 0; partition_idx < ps.size(); ++partition_idx) {
        ss_ptr->generateBlockStatistics(ps[partition_idx], MPP);
    }

    // // calculate required statistical measures from data collected
    // // min,max,average,stddev

    // // subproblem sizes
    // tuple<double, double, double, double> subproblem_sizes_statistics = getStatistics(ss_ptr->block_variable_props);
    // ss_ptr->max_block_variable_prop = get<1>(subproblem_sizes_statistics);
    // ss_ptr->average_block_variable_prop = get<2>(subproblem_sizes_statistics);
    // ss_ptr->stddev_block_variable_prop = get<3>(subproblem_sizes_statistics);

    // get the indices of the different constraint types
    LaPSO::constraint_type_indicies original_constraint_indices = { MP.getConGreaterBounds(), MP.getConLesserBounds(), MP.getConEqualBounds() };

    // set up the initial requirements for LaPSO initialisation
    LaPSO::LaPSORequirements LR = {};
    LR.cti = CLC.convertOriginalConstraintTypeIndicies(original_constraint_indices);
    LR.nVar = HG.getNumNodes();
    LR.nConstr = num_con_relaxed;
    LR.best_ub = best_ub_sol;
    LR.benchmark_ub_flag = true;
    LR.intial_dual_value_pairs = CLC.convertOriginalConstraintInitialDualIndicies(original_intial_dual_value_pairs);
    LR.set_initial_dual_values = set_initial_dual_values;

    // Create the LaPSO Handler object which controls LaPSO process
    cout << "Setting up LaPSO Handler" << endl;
    LaPSOHandler LH(argc, argv, LR);

    // solve Lagrangian Relaxation
    cout << "solving Lagrangian Relaxation" << endl;
    LH.solve(CLC);
    
    // output subproblem statistics
    w.writeRawSubproblemStatistics(LOF,ss_ptr);

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

// void writeDecompInfoToFile(Hypergraph& HG, const vector<double>& con_vec, string filename)
// {
//     int num_con_relaxed = 0;
//     for (auto& val : con_vec) {
//         if (val == 1) {
//             num_con_relaxed++;
//         }
//     }
//     HG.partition(con_vec);
//     Writer writer();

//     std::ofstream outfile;
//     outfile.open(filename);

//     outfile << "Size of original Problem: " << HG.getNumEdges() << endl;
//     outfile << "Size of largest subproblem: " << HG.getLargestPartition() << endl;
//     outfile << "Number of Constraints in original problem: " << HG.getNumEdges() << endl;
//     outfile << "Number of Constraints relaxed: " << num_con_relaxed << endl;
//     outfile.close();
// }

vector<bool> getNSGAConVec(const string& input_file)
{
    vector<bool> con_vec;
    ifstream input(input_file);
    if (input.is_open()) {
        bool vector_reached = false;
        while (!input.eof()) {
            string line;
            getline(input, line);
            if (line.empty()) {
                continue;
            }
            trim(line);
            vector<string> line_split;
            boost::split(line_split, line, boost::is_any_of(","), boost::token_compress_on);
            for (auto& val : line_split) {
                double constraint_val;
                try {
                    constraint_val = stod(val);
                    if (floor(constraint_val + 0.1) == 1.0)
                        con_vec.push_back(true);
                    else {
                        con_vec.push_back(false);
                    }
                } catch (...) {
                }
            }
        }
    }
    return con_vec;
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

    MIP_Parsing_Test_struct MPTS;
    // solve generic MIP
    if (PA.get_generic_MIP_Solver_Flag() == true || PA.get_run_MIP_Parse_testing_flag() == true) {
        cout << "solving generic MIP File" << endl;
        SolveGenericMIP SGM(MIP_Problem_File, para.set_generic_MIP_time);
        bool random_seed = PA.get_generic_MIP_randomSeed_flag();
        CPLEX_Return_struct MIP_results = SGM.solve(random_seed, false);
        // if the results are required for testing purposes with Parsed MIP
        // store the bound and obj value
        if (PA.get_run_MIP_Parse_testing_flag() == true) {
            MPTS.instance_name = MIP_Problem_File;
            MPTS.MPS_bound = MIP_results.bound;
            MPTS.MPS_obj_val = MIP_results.obj_val;
        }
        // w.writeCPLEXResults(para.solve_generic_MIP_output_filename, MIP_Problem_File, MIP_results);
    }

    // Parse in MIP file into self made structure
    MIP_Fileparser MIP_FP;
    cout << "Parsing MPS File" << endl;
    MIP_FP.parse(file_type::MPS, MIP_Problem_File);
    MIP_Problem MP = MIP_FP.getMIPProblem();

    // get dual values from solving the MIP problem
    //
    //tests MIP was read correctly based on expected number of constraints, variables, bin variables, continuous
    //variables non_zeroes and the test has not been flagged as an exception

    if (PA.get_run_MIP_Parse_testing_flag() == true) {
        cout << "testing MIP Parser" << endl;
        // solve the Parsed MIP problem
        // use the same runtime as the MPS file
        MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);

        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), false);
        // populate the Mip Parsing Structure with Parsed MIP information
        MPTS.Parsed_bound = MIP_results.bound;
        MPTS.Parsed_obj_val = MIP_results.obj_val;
        MPTS.Parsed_num_bin = MP.getNumBin();
        MPTS.Parsed_num_cont = MP.getNumCont();
        MPTS.Parsed_num_int = MP.getNumInt();
        MPTS.Parsed_non_zeroes = MP.getnumNonZero();
        MPTS.Parsed_num_var = MP.getNumVariables();
        MPTS.Parsed_num_const = MP.getNumConstraints();

        // populate the Mip Parsing Structure with MPS information from MIPLIB
        MPTS.MIP_num_var = para.MIP_num_var;
        MPTS.MIP_num_const = para.MIP_num_cons;
        MPTS.MIP_num_bin = para.MIP_num_bin;
        MPTS.MIP_num_int = para.MIP_num_int;
        MPTS.MIP_num_cont = para.MIP_num_cont;
        MPTS.num_non_zeroes = para.MIP_num_non_zeroes;

        cout << "Parsed MIP No. of Variables = " << MPTS.Parsed_num_var << endl;
        cout << "Parsed MIP No. of Constraints = " << MPTS.Parsed_num_const << endl;
        // write the results out for actual MIP (MPS file) and parsed MIP
        w.writeMIPParsingResults(string(para.MIP_Parse_testing_output_filename), MPTS);
        // MP.testMIPProblem(para.MIP_num_cons, para.MIP_num_var, para.MIP_num_bin,
        // para.MIP_num_cont, para.MIP_num_int, para.MIP_num_non_zeroes)){
        // cout << "MIP File Parsed Incorrectly" << endl;
        exit(0);
    }

    bool printMIPProblem = false;
    if (printMIPProblem == true) {
        MP.printObjectiveFn();
        MP.printVariables();
        MP.printConstraints();
    }

    // solve the input MIP problem to see if it produces the same results
    // if (PA.get_generic_MIP_Solver_Flag() == true){
    //     MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
    //     bool random_seed = false;
    //     CPLEX_Return_struct MIP_results = MPCS.solve(random_seed);
    //     // need to output the results from cplex solver to compare my parsing and the actual solver...
    //     w.writeCPLEXResults(para.solve_parsed_MIP_output_filename, MIP_Problem_File, MIP_results);
    //     // output results to a file
    //     // i)f (
    // }

    //convert parsed MIP to Hypergraph
    MIP_to_Hypergraph MTH;
    cout << "Converting MIP read to Hypergraph " << endl;
    MTH.convertToHypergraph(MP);
    Hypergraph HG(MTH.getHGEdges(), MTH.getHGNodes());

    // test the parsed in hypergraph partitioning
    if (PA.get_run_Hypergraph_Partitioning_testing_flag()) {
        MP.printConstraints();
        cout << "running hypergraph partitioning test" << endl;
        vector<bool> test_convec = { true, true, true, false, false };
        bool test_partitioning = true;
        // partition function will print error message if partitioing was unsuccessful
        HG.partition(test_convec, test_partitioning);
        HG.printPartitions();
        exit(0);
    }

    // test NSGA writing
    if (PA.get_run_NSGA_testing_flag()) {
        cout << "running NSGA testing" << endl;
        Problem_Adapter ProblemAdapter;
        // Decompositions are written to a file as the population is evolved
        ProblemAdapter.createNSGADecomps(HG, para.nsga_gen, string(para.nsga_decomp_output_file), para.nsga_pop_size);
        exit(0);
    }

    //test constraint redundancy for decompositions
    if (PA.get_run_constraint_redundancy_flag()) {

        ConstraintFileProcessing CFP;
        CFP.removeRedundantConstraints(para.decomps_to_remove_red_const_file
        ,para.redundant_const_removed_output_file, HG);
      
    }

    // remove duplicate constraints
    if (PA.get_run_remove_duplicate_constraints_flag()){
        ConstraintFileProcessing CFP;
        CFP.removeDuplicateConstraints(para.decomps_to_remove_duplicates_file,
        para.duplicates_removed_output_file);
    }

    //test greedy decomposition creation
    if (PA.get_run_greedy_decomposition_testing_flag()) {
        //using created Hypergraph to generate greedy decompositions
        cout << "running greedy decomposition testing" << endl;
        GreedyDecompCreator GDC;

        std::ofstream outfile;
        outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.1.csv");
        if (outfile) {
            // creating 1000 decompositions
            for (int i = 0; i < para.greedy_decomp_size; ++i) {
                //std::cout << "creating Decomp number: " << i << std::endl;
                vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.1);
                for (const auto& con_val : greedy_decomp) {
                    outfile << con_val << ",";
                }
                outfile << endl;
            }
            outfile << MP.getnumNonZero() << endl;
            outfile.close();
        }

        outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.2.csv");
        if (outfile) {
            // creating 1000 decompositions
            for (int i = 0; i < para.greedy_decomp_size; ++i) {
                //std::cout << "creating Decomp number: " << i << std::endl;
                vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.2);
                for (const auto& con_val : greedy_decomp) {
                    outfile << con_val << ",";
                }
                outfile << endl;
            }
            outfile << MP.getnumNonZero() << endl;
            outfile.close();
        }

        outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.5.csv");
        if (outfile) {
            // creating 1000 decompositions
            for (int i = 0; i < para.greedy_decomp_size; ++i) {
                //std::cout << "creating Decomp number: " << i << std::endl;
                vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.5);
                for (const auto& con_val : greedy_decomp) {
                    outfile << con_val << ",";
                }
                outfile << endl;
            }
            outfile << MP.getnumNonZero() << endl;
            outfile.close();
        }

        exit(0);
    }

    // get LP dual values from the MIP solution...
    if (PA.get_run_MIP_Duals_testing_flag() == true) {
        MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
        bool solve_as_LP = true;
        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), solve_as_LP);
        cout << "LP bound is" << MIP_results.bound << endl;
        for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
            cout << "dual value for con " << con_idx << " = " << MIP_results.dual_vals[con_idx] << endl;
        }
        exit(0);
    }

    // run NSGA if desired
    if (PA.get_run_nsga_decomp_flag() == true) {

        Problem_Adapter ProblemAdapter;
        // write out to a file the different decompositions found
        ProblemAdapter.createNSGADecomps(HG, para.nsga_gen, string(para.nsga_decomp_output_file), para.nsga_pop_size);
    }

    // Evaluate Decompositions
    if (PA.get_run_evaluate_decompositions_testing_flag() == true) {
        // read in and solve each decomposition 1 at a time
        vector<bool> con_vec;
        string input_filename = "";
        ifstream input_fs(input_filename);
        solveLapsoStruct sls;
        // sls.output_stats_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
        // sls.output_best_lb_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
        // sls.output_average_lb_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";

        // File structure of von vec decomp_idx, con vec
        if (input_fs) {
            vector<string> string_vec;
            // getline(input, line);
            //     while (!input.eof())
            while (!input_fs.eof()) {
                string line;
                getline(input_fs, line);
                boost::split(string_vec, line, boost::is_any_of(","), boost::token_compress_on);
                con_vec.clear();
                // if "1" is read in, bool val is true, else it is false 0
                for (int i = con_vec_starting_idx_from_file; i < string_vec.size(); ++i) {
                    con_vec.push_back(string_vec[i] == "1");
                }
                sls.con_relax_vector_ptr = &con_vec;

                // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");
            }
        }
    }

    if (PA.get_run_LR_testing_flag() == true) {

        //test_instance 1, relaxing constraint 1
        cout << "running LR testing" << endl;
        MP.printConstraints();
        LaPSOOutputFilenames LOF = {};
        vector<bool> test_convec1 = { true, false, false };
        std::cout << "original constraint relaxed vector is " << std::endl;
        for (const auto& con_val : test_convec1) {
            std::cout << con_val << " ";
        }
        std::cout << std::endl;
        //void solveLapso(int& argc, const char** argv, MIP_Problem& MP, Hypergraph& HG, const vector<bool>& con_relax_vector,
        //const double& best_ub_sol, const LaPSOOutputFilenames& LOF, const double sp_solver_time_limit)
        std::vector<initial_dual_value_pair> idvp;
        int decomposition_idx = 0;
        solveLapso(argc, argv, MP, HG, test_convec1, para.set_ub, LOF, decomposition_idx, 100, 100, idvp, false);

        vector<bool> test_convec2 = { false, true, true };
        std::cout << "original constraint relaxed vector is " << std::endl;
        for (const auto& con_val : test_convec2) {
            std::cout << con_val << " ";
        }
        ++decomposition_idx;
        std::vector<initial_dual_value_pair> idvp_2;
        idvp_2.push_back({ 0, -11 });
        idvp_2.push_back({ 1, 0 });

        bool set_inital_dual_values = true;

        //  solveLapso(argc, argv, MP, HG, test_decomposition, para.set_ub, LOF, decomposition_idx,  subproblem_solver_time_lim,
        // LR_iter_limit, &dual_values_from_LP, set_initial_dual_values);
        solveLapso(argc, argv, MP, HG, test_convec2, para.set_ub, LOF, decomposition_idx, 100, 100, idvp_2, set_inital_dual_values);
        std::cout << std::endl;
        exit(0);
    }

    

    if (PA.get_run_statistic_testing_flag() == true) {

        // using the same instance as in the LR testing should give the same dual value as the first relaxtion tested.
        // get in the dual values for all constraints
        // vector<initial_daul_value_pair> dual_values_from_LP;
        // MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
        // bool solve_as_LP = true;

        // CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), true);
        // cout << "LP bound is" << MIP_results.bound << endl;
        // for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
        //     dual_values_from_LP.push_back({con_idx, MIP_results.dual_vals[con_idx]});
        //     cout << "dual value for con " << con_idx << " = " << MIP_results.dual_vals[con_idx] << endl;
        // }

  
        // assign a decomposition index
        int decomposition_idx = 0;
        // for test_mip 1...
        // get the constraint vector from external file.

        vector<bool> con_vec_readin = readInConVecFromFile(string(para.con_vec_filename));
        vector<initial_dual_value_pair> test_dual_values;
        for (int i = 0; i < con_vec_readin.size(); ++i) {
            test_dual_values.push_back({ i, 0 });
        }
        bool set_initial_dual_values = true;
        LaPSOOutputFilenames LOF = {};
        LOF.subproblem_statistics_folder = string(para.subproblem_statistics_folder);
        LOF.relaxed_const
        // LOF.instance_statistics_filename = string(para.)
        cout << "Subproblem Statistics Filename: " << string(para.subproblem_statistics_folder) << endl;
        int LR_iter_limit = para.maxIter;
        int subproblem_solver_time_lim = 300;
        solveLapso(argc, argv, MP, HG, con_vec_readin, para.set_ub, LOF, decomposition_idx, subproblem_solver_time_lim,
            LR_iter_limit, test_dual_values, set_initial_dual_values);

        exit(0);
        // for test_mip 2...
        //vector<bool> test_convec2 = {true, true, true, false, false, false, false, false};

        // solveLapso(argc, argv, MP, HG, test_convec2, para.set_ub, LOF, decomposition_idx,  subproblem_solver_time_lim,
        // LR_iter_limit
        // , &dual_values_from_LP, set_initial_dual_values);
    }
    //  run_greedy_decomp_flag = getBoolVal(p.run_greedy_decomp);
    //     run_NSGA_decomp_flag = getBoolVal(p.run_NSGA_decomp);
    if (PA.get_run_greedy_decomp_flag()) {
        cout << "running greedy decomposition testing" << endl;
        GreedyDecompCreator GDC;
        vector<double> rc_props = { 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
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

    // flow pattern of the whole process

    // generate decomposition
    // run through constraint redundancy
    // get constraint decomposition statistics
    // run through a single iteration of LR to get subproblem solver statistics
    // test decomposition statistics

    // read in constraint_vector from file
    // vector<bool> con_vec = getNSGAConVec(filename);
    // cout << "con vec size is " << con_vec.size() << endl;
    // string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
    // string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
    // string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";
    // cout << "running Lapso: " << prop_string << endl;
    // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");

    // run normal LR
    // if (PA.get_run_lapso_flag() == true) {
    //     cout << "solving LaPSO with NSGA Decomposition" << endl;
    //     boost::filesystem::path p(string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name));
    //     try {
    //         if (exists(p)) {
    //             if (is_directory(p)) {
    //                 cout << p << " is a directory containing:\n";

    //                 for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(p)) {
    //                     string filename = x.path().string();
    //                     if (filename.find("_vec") == string::npos) {
    //                         continue;
    //                     }
    //                     string prop_string = "";
    //                     int pos = filename.find_last_of("/");
    //                     for (int i = pos + 1; i < filename.size(); i++) {
    //                         if (isdigit(filename[i]) || filename[i] == '.') {
    //                             prop_string += filename[i];
    //                         }
    //                     }
    //                     //read in file
    //                     vector<bool> con_vec = getNSGAConVec(filename);
    //                     cout << "con vec size is " << con_vec.size() << endl;
    //                     string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
    //                     string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
    //                     string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";
    //                     cout << "running Lapso: " << prop_string << endl;
    //                     // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");
    //                 }
    //             } else
    //                 cout << p << " exists, but is not a regular file or directory\n";
    //         } else
    //             cout << p << " does not exist\n";
    //     } catch (const boost::filesystem::filesystem_error& ex) {
    //         cout << ex.what() << '\n';
    //     }
    // }

    // // run LR with greedily produced decompositions
    // if (PA.get_test_greedy_decomp_flag() == true) {
    //     cout << "solving greedy decomp" << endl;
    //     string con_count_string(para.test_greedy_decomp);

    //     string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "Greedy_" + con_count_string + ".txt";
    //     string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_best_" + con_count_string + ".csv";
    //     string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_average_" + con_count_string + ".csv";
    //     //create decomp vec

    //     std::vector<HG_Edge> edges_copy;

    //     for (auto& edge : HG.getHGEdges()) {
    //         edges_copy.push_back(HG_Edge{ edge.getEdgeIdx(), edge.getNodeIdxs() });
    //     }
    //     vector<bool> con_vec;
    //     con_vec.resize(edges_copy.size(), false);

    //     //sort by descending order on constraint size
    //     std::sort(edges_copy.begin(), edges_copy.end(), greater<HG_Edge>());
    //     int count = 0;
    //     for (int i = 0; i < edges_copy.size(); i++) {
    //         if (count < (stoi(con_count_string))) {
    //             con_vec[edges_copy[i].getEdgeIdx()] = 1;
    //             count++;
    //         } else {
    //             break;
    //         }
    //     }
    //     cout << "solving LaPSO" << endl;
    //     // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim, "");
    // }

    // // run LR with random decompositions
    // if (PA.get_test_random_decomp_flag() == true) {
    //     cout << "solving random decomp" << endl;
    //     string con_count_string(para.test_random_decomp);
    //     string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "Random_" + con_count_string + ".txt";
    //     string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Random_best_" + con_count_string + ".csv";
    //     string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Random_average_" + con_count_string + ".csv";
    //     //create decompveco

    //     vector<bool> con_vec;
    //     con_vec.resize(HG.getNumEdges(), false);
    //     //set seed to current time in seconds

    //     int count = 0;
    //     int idx = 0;
    //     while (count < (stoi(con_count_string))) {
    //         con_vec[idx] = 1;
    //         count++;
    //         idx++;
    //     }

    //     std::random_device rd;
    //     std::mt19937 g(rd());
    //     std::shuffle(con_vec.begin(), con_vec.end(), g);

    //     cout << "solving LaPSO" << endl;
    //     // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim, string(para.random_lb_output));
    // }

    return 0;
}