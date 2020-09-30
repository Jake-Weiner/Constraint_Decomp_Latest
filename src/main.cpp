#include "ConDecomp_LaPSO_Connector.h"
#include "Decomp.hpp"
#include "DecompMIP.h"
#include "GreedyDecompCreator.h"
#include "Hypergraph.h"
#include "MIPProblemProbe.h"
#include "MIP_Fileparser.h"
#include "MIP_Problem_CPLEX_Solver.h"
#include "MIP_to_Hypergraph.h"
#include "Param.h"
#include "ParamAdapter.h"
#include "Problem_Adapter.h"
#include "Random.h"
#include "SolveGenericMIP.h"
#include "LaPSOHandler.h"
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

using std::cout;
using std::endl;
using std::string;
using std::vector;

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
    const double& best_ub_sol, const LaPSOOutputFilenames& LOF,
    const double sp_solver_time_limit, std::vector<initial_daul_value_pair>* intial_dual_value_pairs, bool set_initial_dual_values)
{

    // calculate number of constraints relaxed
    int num_con_relaxed = 0;
    for (auto&& val : con_relax_vector) {
        if (val == true) {
            num_con_relaxed++;
        }
    }

    // test partition if required
    bool test_hypergraph_partitioning = false;
    std::vector<Partition_Struct> ps = HG.getPartitionStruct(con_relax_vector, test_hypergraph_partitioning);
    // try create partition struct
    // partition the HG based on the relaxed constraints which then provides new partitions which forms the subproblems for
    // the LaPSO method
    bool debug_printing = true;
    ConDecomp_LaPSO_Connector CLC(MP, ps,con_relax_vector, debug_printing, sp_solver_time_limit);
    CLC.maxsolves = 100;
    CLC.nsolves = 0;
   
    // get the indices of the constraint types
    LaPSO::constraint_type_indicies cti = {MP.getConGreaterBounds(), MP.getConLesserBounds(), MP.getConEqualBounds()};

    // set up the initial requirements for LaPSO initialisation
    LaPSO::LaPSORequirements LR = {};
    LR.cti = &cti;
    LR.nVar = HG.getNumNodes();
    LR.nConstr = num_con_relaxed;
    LR.best_ub = best_ub_sol;
    LR.benchmark_ub_flag = true;
    LR.intial_dual_value_pairs = intial_dual_value_pairs;
    LR.set_initial_dual_values = set_initial_dual_values;
  

    // Create the LaPSO Handler object which controls LaPSO process
    LaPSOHandler LH(argc, argv, LR);
    // solve Lagrangian Relaxation
    LH.solve(CLC);

    // int largest_sp = HG.getLargestPartition();

    // Writer w;
    // w.writeAverageLBPlot(largest_sp, num_con_relaxed, LH.getSolverAverageLBTracking(), LH.getSolverTimingTracking(), output_average_lb_filename);
    // w.writeBestLBPlot(largest_sp, num_con_relaxed, LH.getSolverBestLBTracking(), LH.getSolverTimingTracking(), output_best_lb_filename);
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

int main(int argc,const char** argv)
{
    // Parse in parameters
    mainParam::Param para;
    para.parse(argc, argv);
    // Manipulate/transform parameters from char to required values through Adapter
    ParamAdapter PA(para);
    Writer w;
    string MIP_Problem_File = PA.getProblemFilename();
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
    if (PA.get_run_constraint_redundancy_testing_flag()) {
        cout << "running constraint redundancy testing" << endl;
        MP.printConstraints();
        vector<bool> test_convec = { true, true, true, false, false, true };
        std::cout << "original constraint relaxed vector is " << std::endl;
        for (const auto& con_val : test_convec) {
            std::cout << con_val;
        }
        std::cout << endl;
        std::cout << "new constraint relaxed vector is " << std::endl;
        vector<bool> new_test_convec = HG.removeRelaxedConstraintRedundancies(test_convec);
        for (const auto& con_val : new_test_convec) {
            std::cout << con_val;
        }
        std::cout << std::endl;
        HG.printPartitions();
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
            for (int i = 0; i < 10000; ++i) {
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
            for (int i = 0; i < 10000; ++i) {
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
            for (int i = 0; i < 10000; ++i) {
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
        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), true);
        cout << "LP bound is" << MIP_results.bound << endl;
        for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
            cout << "dual value for con " << con_idx << " = " << MIP_results.dual_vals[con_idx] << endl;
        }
        exit(0);
    }

    // run NSGA if desired
    if (PA.get_run_nsga_decomp_flag() == true) {
        // start, stop, interval
        vector<double> desired_subproblem_props = PA.get_nsga_props();
        string NSGA_decomp_plot_filename = PA.get_NSGA_decomp_plot_filename();

        // creates all decompositions found in first front. Also returns all decompositions which satisfy the desired proportion
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
                for(int i = con_vec_starting_idx_from_file; i<string_vec.size(); ++i){
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
        solveLapso(argc, argv, MP, HG, test_convec1, para.set_ub, LOF, 100, {}, false);

        vector<bool> test_convec2 = { false, true, true };
        std::cout << "original constraint relaxed vector is " << std::endl;
        for (const auto& con_val : test_convec2) {
            std::cout << con_val << " ";
        }

        std::vector<initial_daul_value_pair> idvp_2;
        idvp_2.push_back({0,-11});
        idvp_2.push_back({1,0});
        solveLapso(argc, argv, MP, HG, test_convec2, para.set_ub, LOF, 100, &idvp_2, true);
        std::cout << std::endl;
        exit(0);

    } 


    // flow pattern of the whole process

    // generate decomposition
    // run through constraint redundancy
    // get constraint decomposition statistics
    // run through a single iteration of LR to get subproblem solver statistics



            //     std::vector<std::string> result;
            //     std::string line;
            
            //     std::stringstream lineStream(line);
            //     std::string cell;

            //     while (std::getline(lineStream, cell, ',')) {
            //         result.push_back(cell);
            //     }
            //     // used for breaking words
            //     stringstream s(line);
            //     // read every column data of a row and
            //     // store it in a string variable, 'word'
            //     while (getline(s, word, ',')) {

            //         // add all the column data
            //         // of a row to a vector
            //         row.push_back(word);
            //     }
            // }
           

            // read in constraint_vector from file
            // vector<bool> con_vec = getNSGAConVec(filename);
            // cout << "con vec size is " << con_vec.size() << endl;
            // string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
            // string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
            // string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";
            // cout << "running Lapso: " << prop_string << endl;
            // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");

            //non-runtime decompositions statistics

            //         Largest Subproblem as a proportion of total variables
            // proportion of total constraints relaxed
            // proportion of constraints relaxed which are equality constraints
            // proportion of constraints relaxed which are inequality constraints
            // proportion of binary variables in relaxed constraints
            // proportion of integer variables in relaxed constraints
            // proportion of continuous variables in relaxed constraints
            // average, min, max, stddev nonzeros percentage in relaxed constraints
            // average, min, max, stddev of the largest ratio of each constraint (abs(RHS/LHS)) (e.g for constraint x1 + 2x2 = 3, Largest ratio is 3/1,  2x1 + x3 = 4,  Largest ratio is 4)
            //read in decompositions from file
            //for each decomposition, run LR once with LP duals forming the LR values
            // get the statistics of the constraints
        

        // writes out NSGA Decomps to show pareto fronts NSGA_decomp_plot_filename  std::ofstream outfile;
        // outfile.open(output_filename);
        // vector<pop_size_t> front1 = non_dominated_front_2d(f_vals);
        // for (auto& idx : front1) {
        //     outfile << f_vals[idx][1] << "," << f_vals[idx][0] << endl;
        // }

        //instance statistics (based of MIP Parser)
        //Binary Var prop
        //Cont Var prop
        //Int Var prop
        //Constraint Types
        //Number of non zerores
        // average density of constraints, stddev of constraints

        // MIPProblemProbe MPP;
        // NSGA_ii_instance_statistics nis;

        // // fill out the required instance statistics
        // MPP.populateInstanceStatistics(nis, MP);

        // // loop through decompositions, assigning a decomposition number

        // int decomp_idx = 0;
        // for (auto& decomp: nsga_con_relax_info_struct){
        //     NSGA_ii_relaxed_constraint_statistics nrcs;
        //     MPP.populateRelaxedConstraintsStatistics(decomp_idx,decomp, nrcs, MP);
        //     ++decomp_idx;
        // }

        // vector<double> con_vec;
        // double LSP_prop;
        // double num_constraints_relaxed_prop;
        // // proportion of relaxed constraints which are equality
        // double equality_constaints_relaxed_prop;
        // // proportion of relaxed constraints which are inequality = 1-equality_constaints_relaxed_prop
        // double inequality_constaints_relaxed_prop;
        // // non_zero prop in constraints relaxed
        // double average_nonzero_prop;
        // double stddev_nonzero_prop;
        // // ratio of RHS to largest coeff in constraint
        // double average_largest_ratio;
        // double stddev_largest_ratio;

        // instance_statistics_masterfile.csv
        // <instance_name>, Binary Var Prop, Cont Var Prop, Int Var Prop, Constraint types(?), Number Non-zeroes, Average density(nonzero/no. constraints), stddev nonzeroes

        //con_relax vec
        //based off NSGA_results
        //LSP
        // constraints statistics
        //num constraints_relaxed
        //prop equality constraints/non equality constraints
        //average perc/non_zeroes stddev nonzeros (in constraints relaxed)
        //average, stddev largest ratio (RHS/LHS)
        //average, stddev sum/coefficients (RHS/LHS)

        // output file is <instance_name>_constraint_statistics.csv
        // Decomp No.,[xxxx], LSP, No. Constraints Relaxed, prop equality constraints, prop inequality constraints, ave prop non zeroes, stddev non zeroes,
        // average largest ratio RHS/LHS, stddev largest ration (RHS/LHS), average sum (not sure if we need this feature)

        //output file is <instance_name>_solver_statistics
        //solver statistics
        // Decomp No., Max MIP Time, average MIP time, stdev mip time,
        // Max LP Time, average LP time, stdev LP time,

        //subproblem statistics
        // Subproblem Number
        //MIP Time
        //LP Time
        //Problem Reduction (%)
        // average Bin, cont, int.
        // stdev bin, cont, int
        //average no. constraint, stddev no. constraint
        //First iteration solve time

        // write out decompositions of desired proportions
        // for (auto& con_relax_info : nsga_con_relax_info_struct) {
        //     string con_vec_filename = string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name) + "/" + to_string(con_relax_info.largest_sp) + "_vec.csv";
        //     string decomp_info_filename = string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name) + "/" + to_string(con_relax_info.largest_sp) + "_info.txt";

        //     // w.writeConVecToFile(con_relax_info.first, con_vec_filename);
        //     // writeConVecToFile(con_relax_info.first, con_vec_filename);
        //     // writeDecompInfoToFile(HG, con_relax_info.first, decomp_info_filename);
        // }

        // run normal LR
        if (PA.get_run_lapso_flag() == true) {
            cout << "solving LaPSO with NSGA Decomposition" << endl;
            boost::filesystem::path p(string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name));
            try {
                if (exists(p)) {
                    if (is_directory(p)) {
                        cout << p << " is a directory containing:\n";

                        for (boost::filesystem::directory_entry& x : boost::filesystem::directory_iterator(p)) {
                            string filename = x.path().string();
                            if (filename.find("_vec") == string::npos) {
                                continue;
                            }
                            string prop_string = "";
                            int pos = filename.find_last_of("/");
                            for (int i = pos + 1; i < filename.size(); i++) {
                                if (isdigit(filename[i]) || filename[i] == '.') {
                                    prop_string += filename[i];
                                }
                            }
                            //read in file
                            vector<bool> con_vec = getNSGAConVec(filename);
                            cout << "con vec size is " << con_vec.size() << endl;
                            string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
                            string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
                            string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";
                            cout << "running Lapso: " << prop_string << endl;
                            // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");
                        }
                    } else
                        cout << p << " exists, but is not a regular file or directory\n";
                } else
                    cout << p << " does not exist\n";
            } catch (const boost::filesystem::filesystem_error& ex) {
                cout << ex.what() << '\n';
            }
        }

        // run LR with greedily produced decompositions
        if (PA.get_test_greedy_decomp_flag() == true) {
            cout << "solving greedy decomp" << endl;
            string con_count_string(para.test_greedy_decomp);

            string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "Greedy_" + con_count_string + ".txt";
            string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_best_" + con_count_string + ".csv";
            string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_average_" + con_count_string + ".csv";
            //create decomp vec

            std::vector<HG_Edge> edges_copy;

            for (auto& edge : HG.getHGEdges()) {
                edges_copy.push_back(HG_Edge{ edge.getEdgeIdx(), edge.getNodeIdxs() });
            }
            vector<bool> con_vec;
            con_vec.resize(edges_copy.size(), false);

            //sort by descending order on constraint size
            std::sort(edges_copy.begin(), edges_copy.end(), greater<HG_Edge>());
            int count = 0;
            for (int i = 0; i < edges_copy.size(); i++) {
                if (count < (stoi(con_count_string))) {
                    con_vec[edges_copy[i].getEdgeIdx()] = 1;
                    count++;
                } else {
                    break;
                }
            }
            cout << "solving LaPSO" << endl;
            // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim, "");
        }

        // run LR with random decompositions
        if (PA.get_test_random_decomp_flag() == true) {
            cout << "solving random decomp" << endl;
            string con_count_string(para.test_random_decomp);
            string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "Random_" + con_count_string + ".txt";
            string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Random_best_" + con_count_string + ".csv";
            string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Random_average_" + con_count_string + ".csv";
            //create decompveco

            vector<bool> con_vec;
            con_vec.resize(HG.getNumEdges(), false);
            //set seed to current time in seconds

            int count = 0;
            int idx = 0;
            while (count < (stoi(con_count_string))) {
                con_vec[idx] = 1;
                count++;
                idx++;
            }

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(con_vec.begin(), con_vec.end(), g);

            cout << "solving LaPSO" << endl;
            // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim, string(para.random_lb_output));
        }

        return 0;
    }