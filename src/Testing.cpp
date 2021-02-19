#include "Testing.h"
#include "ConDecomp_LaPSO_Connector.h"
#include "Decomposition_Statistics.h"
#include "Hypergraph.h"
#include "LRHandler.h"
#include "LaPSO.hpp"
#include "MIPProblemProbe.h"
#include "MIP_Problem.h"
#include "MIP_Problem_CPLEX_Solver.h"
#include "Param.h"
#include "ParamAdapter.h"
#include "SolveGenericMIP.h"
#include "Util.h"
#include "Writer.h"

// copy across command line inputs
Testing::Testing(int& argc, const char** argv)
    : argc(argc)
    , argv(argv)
{
}

void Testing::solveLapso(MIP_Problem& MP, MIPProblemProbe& MPP, Hypergraph& HG, vector<int>& con_relax_vector,
    const LaPSOOutputFilenames& LOF, int decomposition_idx,
    const std::vector<initial_dual_value_pair>& original_intial_dual_value_pairs, bool set_initial_dual_values = false,
    bool capture_statistics = false)
{
    // Parse in parameters
    mainParam::Param para;
    para.parse(argc, argv);

    // Manipulate/transform parameters from char to required values through Adapter
    ParamAdapter PA(para);
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
    // get the bounds and solve times for the Lagrangian Relaxation process
    std::tuple<double, double> LaPSO_outputs = LRH.getLaPSOOutputs();
    std::shared_ptr<LROutputs> lro_ptr = std::make_shared<LROutputs>(decomposition_idx, get<0>(LaPSO_outputs), get<1>(LaPSO_outputs));
    if (capture_statistics) {
        w.writeRawSubproblemStatistics(LOF, ss_ptr);
        w.writeLROutputs(LOF, lro_ptr);
    }
}

void Testing::RunMIPTesting(MIP_Problem& MP)
{
    // Parse in parameters
    mainParam::Param para;
    para.parse(argc, argv);

    // Manipulate/transform parameters from char to required values through Adapter
    ParamAdapter PA(para);
    MIP_Parsing_Test_struct MPTS;
    string MIP_Problem_File = PA.getProblemFilename();
    Writer w;
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

    if (PA.get_run_MIP_Parse_testing_flag() == true) {
        cout << "testing MIP Parser" << endl;
        // solve the Parsed MIP problem
        // use the same runtime as the MPS file
        MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
        bool solve_as_LP = false;
        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), solve_as_LP);
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
        exit(0);
    }
}

void Testing::testEmptyConVec(MIP_Problem& MP, Hypergraph& HG)
{
    // Parse in parameters
    mainParam::Param para;
    para.parse(argc, argv);

    // Manipulate/transform parameters from char to required values through Adapter
    ParamAdapter PA(para);
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
            cout << "empty relaxed vec size is " << relaxed_constraints_str.size() << endl;

            for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
                try{
                    relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
                }
                catch(const std::invalid_argument& ia){
                    if (relaxed_constraints_str[i] == ""){
                        break;
                    }
                    else{
                        cout << "invalid relaxed constraint number parsed in input con vec - " << relaxed_constraints_str[i] << endl;
                        exit(EXIT_FAILURE);
                    }
                }
                
            }
            cout << endl;
            if (relaxed_constraints_int.empty()){
                cout << "no constraints are relaxed " << endl;
            }
        
            bool capture_statistics = true;
            solveLapso(MP, MPP, HG, relaxed_constraints_int, LOF, decomposition_idx, dual_values_from_LP, set_initial_dual_values, capture_statistics);
            ++decomposition_idx;
        }
    } else {
        cout << "constraint input file unable to be found/opened" << endl;
    }
    exit(EXIT_SUCCESS);
}

// // test the parsed in hypergraph partitioning
// if (PA.get_run_Hypergraph_Partitioning_testing_flag()) {
//     MP.printConstraints();
//     cout << "running hypergraph partitioning test" << endl;

//     bool test_partitioning = true;
//     vector<vector<int>> test_decompositions = readInConVecFromFile(para.general_decomp_file);
//     // partition function will print error message if partitioing was unsuccessful
//     for (const auto& test_convec : test_decompositions) {
//         HG.partition(test_convec, test_partitioning);
//         HG.printPartitions();
//     }

//     exit(0);
// }

// // test NSGA writing
// if (PA.get_run_NSGA_testing_flag()) {
//     cout << "running NSGA testing" << endl;
//     Problem_Adapter ProblemAdapter;
//     // Decompositions are written to a file as the population is evolved
//     bool print_objectives = true;
//     ProblemAdapter.createNSGADecomps(HG, para.nsga_gen, string(para.nsga_decomp_output_file), string(para.nsga_pareto_optimal_output_file), para.nsga_pop_size, print_objectives);
//     exit(0);
// }

// //test greedy decomposition creation
// if (PA.get_run_greedy_decomposition_testing_flag()) {
//     //using created Hypergraph to generate greedy decompositions
//     cout << "running greedy decomposition testing" << endl;
//     GreedyDecompCreator GDC;

//     std::ofstream outfile;
//     outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.1.csv");
//     if (outfile) {
//         // creating 1000 decompositions
//         for (int i = 0; i < para.greedy_decomp_size; ++i) {
//             //std::cout << "creating Decomp number: " << i << std::endl;
//             vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.1);
//             for (const auto& con_val : greedy_decomp) {
//                 outfile << con_val << ",";
//             }
//             outfile << endl;
//         }
//         outfile << MP.getnumNonZero() << endl;
//         outfile.close();
//     }

//     outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.2.csv");
//     if (outfile) {
//         // creating 1000 decompositions
//         for (int i = 0; i < para.greedy_decomp_size; ++i) {
//             //std::cout << "creating Decomp number: " << i << std::endl;
//             vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.2);
//             for (const auto& con_val : greedy_decomp) {
//                 outfile << con_val << ",";
//             }
//             outfile << endl;
//         }
//         outfile << MP.getnumNonZero() << endl;
//         outfile.close();
//     }

//     outfile.open("/home/jake/PhD/Decomposition/Output/testing/Greedy_Decomp_Tests/decomps_0.5.csv");
//     if (outfile) {
//         // creating 1000 decompositions
//         for (int i = 0; i < para.greedy_decomp_size; ++i) {
//             //std::cout << "creating Decomp number: " << i << std::endl;
//             vector<bool> greedy_decomp = GDC.createGreedyDecomp(MP, 0.5);
//             for (const auto& con_val : greedy_decomp) {
//                 outfile << con_val << ",";
//             }
//             outfile << endl;
//         }
//         outfile << MP.getnumNonZero() << endl;
//         outfile.close();
//     }

//     exit(0);
// }

// // get LP dual values from the MIP solution...
// if (PA.get_run_MIP_Duals_testing_flag() == true) {
//     MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
//     bool solve_as_LP = true;
//     CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag(), solve_as_LP);
//     cout << "LP bound is " << MIP_results.bound << endl;
//     for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
//         cout << "dual value for con " << con_idx << " = " << MIP_results.dual_vals[con_idx] << endl;
//     }
// }

// // Evaluate Decompositions
// if (PA.get_run_evaluate_decompositions_testing_flag() == true) {
//     // read in and solve each decomposition 1 at a time
//     vector<bool> con_vec;
//     string input_filename = "";
//     ifstream input_fs(input_filename);
//     solveLapsoStruct sls;
//     // sls.output_stats_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "NSGA_" + prop_string + "txt";
//     // sls.output_best_lb_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_best_" + prop_string + "csv";
//     // sls.output_average_lb_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "NSGA_average_" + prop_string + "csv";

//     // File structure of von vec decomp_idx, con vec
//     if (input_fs) {
//         vector<string> string_vec;
//         // getline(input, line);
//         //     while (!input.eof())
//         while (!input_fs.eof()) {
//             string line;
//             getline(input_fs, line);
//             boost::split(string_vec, line, boost::is_any_of(","), boost::token_compress_on);
//             con_vec.clear();
//             // if "1" is read in, bool val is true, else it is false 0
//             for (int i = con_vec_starting_idx_from_file; i < string_vec.size(); ++i) {
//                 con_vec.push_back(string_vec[i] == "1");
//             }
//             sls.con_relax_vector_ptr = &con_vec;

//             // solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");
//         }
//     }
// }

// if (PA.get_run_LR_testing_flag() == true) {
//     //test_instance 1, relaxing constraint 1
//     cout << "running LR testing" << endl;
//     MP.printConstraints();
//     LaPSOOutputFilenames LOF = {};
//     int decomposition_idx = 0;
//     vector<vector<int>> test_decompositions = readInConVecFromFile(para.general_decomp_file);
//     std::vector<initial_dual_value_pair> idvp_2;
//     idvp_2.push_back({ 0, -1 });
//     idvp_2.push_back({ 1, -11 });
//     idvp_2.push_back({ 2, 0 });
//     bool set_inital_dual_values = true;
//     bool debug_printing = PA.get_debug_printing_flag();
//     // MIPProblem probe object used to get statistics from MIP problem
//     MIPProblemProbe MPP(&MP);
//     // solveLapso(argc, argv, MP, MPP, HG, test_decompositions[0], para.set_ub, LOF, decomposition_idx, 100, 100, idvp_2, set_inital_dual_values, debug_printing);
//     exit(0);
// }

// if (PA.get_run_statistic_testing_flag() == true) {
//     // assign a decomposition index
//     int decomposition_idx = 0;
//     // for test_mip 1...
//     // get the constraint vector from external file.
//     std::ifstream input_fs(string(para.con_vec_filename));
//     LaPSOOutputFilenames LOF = {};
//     LOF.subproblem_statistics_folder = string(para.subproblem_statistics_folder);
//     LOF.relaxed_constraints_statistics_folder = string(para.relaxed_constraint_statistics_folder);
//     LOF.instance_statistics_folder = string(para.instance_statistics_folder);
//     LOF.LR_outputs_folder = string(para.LR_outputs_folder);

//     // MIPProblem probe object used to get statistics from MIP problem
//     MIPProblemProbe MPP(&MP);

//     if (string(para.instance_statistics_folder) != "") {
//         std::shared_ptr<Instance> ins_ptr = std::make_shared<Instance>(MPP);
//         w.writeInstanceStatistics(LOF, ins_ptr);
//     }

//     // input file successfully opened
//     if (input_fs) {
//         string line_read;
//         while (getline(input_fs, line_read)) {
//             vector<string> relaxed_constraints_str;
//             vector<int> relaxed_constraints_int;
//             // split the line based on ,
//             boost::split(relaxed_constraints_str, line_read, boost::is_any_of(","), boost::token_compress_on);
//             // first line contains the number of nodes
//             // last element will be empty because of ending final comma
//             for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
//                 relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
//             }
//             // set all dual values to 0
//             vector<initial_dual_value_pair> test_dual_values;
//             for (int i = 0; i < relaxed_constraints_int.size(); ++i) {
//                 test_dual_values.push_back({ i, 0 });
//             }
//             bool set_initial_dual_values = true;
//             // LOF.instance_statistics_filename = string(para.)
//             cout << "Subproblem Statistics Filename: " << string(para.subproblem_statistics_folder) << endl;
//             int LR_iter_limit = para.maxIter;
//             double total_LR_time_lim = para.total_LR_time_lim;

//             bool capture_statistics = true;
//             bool debug_printing = PA.get_debug_printing_flag();
//             solveLapso(argc, argv, MP, MPP, HG, para, PA, relaxed_constraints_int, LOF, decomposition_idx, test_dual_values, set_initial_dual_values, capture_statistics);
//         }
//     }

//     else {
//         cout << "redundant constraint input file unable to be found/opened" << endl;
//     }
//     exit(EXIT_SUCCESS);
// }

// if (PA.get_run_LSP_testing_flag()) {
//     // print out the largest subproblem of the read in decompositions
//     std::ifstream input_fs(string(para.con_vec_filename));
//     int total_nodes = MP.getNumVariables();
//     if (input_fs) {
//         string line_read;
//         while (getline(input_fs, line_read)) {
//             vector<string> relaxed_constraints_str;
//             vector<int> relaxed_constraints_int;
//             // split the line based on ,
//             boost::split(relaxed_constraints_str, line_read, boost::is_any_of(","), boost::token_compress_on);
//             // first line contains the number of nodes
//             // last element will be empty because of ending final comma
//             for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
//                 relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
//             }
//             vector<Partition_Struct> ps = HG.getPartitionStruct(relaxed_constraints_int, false);
//             double largest_sp = 0;

//             for (const auto& partition : ps) {
//                 int partition_num_nodes = partition.getNumNodes();
//                 if ((static_cast<double>(partition_num_nodes) / static_cast<double>(total_nodes)) > largest_sp) {
//                     largest_sp = static_cast<double>(partition_num_nodes) / static_cast<double>(total_nodes);
//                 }
//             }
//             cout << "Largest SP is " << largest_sp << endl;
//         }
//     } else {
//         cout << "constraint input file unable to be found/opened" << endl;
//     }
//     exit(EXIT_SUCCESS);
// }