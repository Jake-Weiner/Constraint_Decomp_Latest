#include "ConDecomp_LaPSO_Connector.h"
#include "Decomp.hpp"
#include "DecompMIP.h"
#include "Hypergraph.h"
#include "MIP_Fileparser.h"
#include "MIP_Problem_CPLEX_Solver.h"
#include "MIP_to_Hypergraph.h"
#include "Param.h"
#include "Random.h"
#include "WriteResults.h"
#include "SolveLaPSO.h"
#include "Writer.h"
#include "ParamAdapter.h"
#include "Problem_Adapter.h"
#include "Util.h"
#include "SolveGenericMIP.h"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/nsga2.hpp>
#include <pagmo/archipelago.hpp>
#include <pagmo/problem.hpp>
#include <pagmo/utils/multi_objective.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>


using std::vector;
using std::cout;
using std::string;
using std::endl;

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

bool solveLapso(int& argc, const char** argv, MIP_Problem& MP, Hypergraph& HG, const vector<bool>& con_relax_vector,
    const double& best_ub_sol, const string& output_stats_filename, const string& output_best_lb_filename,
    const string& output_average_lb_filename, const double& sp_prop, const double sp_solver_time_limit
    ,const string& final_lb_filename)
{
    int num_con_relaxed = 0;
    for (auto&& val : con_relax_vector) {
        if (val == true ) {
            num_con_relaxed++;
        }
    }

    bool success_flag;
    std::vector<Partition_Struct> ps;
    // try create partition struct
    success_flag = HG.getPartitionStruct(con_relax_vector, sp_prop,ps);
    if (success_flag == true){

        ConDecomp_LaPSO_Connector CLC(MP, ps, false, sp_solver_time_limit);
        CLC.maxsolves = 100;
        CLC.nsolves = 0;

        LaPSO::constraint_type_indicies cti = {MP.getConGreaterBounds(), MP.getConLesserBounds(), MP.getConEqualBounds()};
        SolveLaPSO SL(argc, argv, HG.getNumNodes(), HG.getNumEdges(), best_ub_sol, cti);
        SL.solve(CLC);

        int largest_sp = HG.getLargestPartition();

        Writer w;
        w.writeAverageLBPlot(largest_sp, num_con_relaxed,SL.getSolverAverageLBTracking()
        ,SL.getSolverTimingTracking(), output_average_lb_filename);
        w.writeBestLBPlot(largest_sp, num_con_relaxed,SL.getSolverBestLBTracking()
        ,SL.getSolverTimingTracking(), output_best_lb_filename);
    }
    return success_flag;
    
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
                    else{
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
    MIP_Parsing_Test_struct MPTS;
    // solve generic MIP
    if (PA.get_generic_MIP_Solver_Flag() == true || PA.get_run_MIP_Parse_testing_flag() == true) {
        cout << "solving generic MIP File" << endl;

        SolveGenericMIP SGM(MIP_Problem_File, para.set_generic_MIP_time);
        bool random_seed = PA.get_generic_MIP_randomSeed_flag();
        CPLEX_Return_struct MIP_results = SGM.solve(random_seed);
        // if the results are required for testing purposes with Parsed MIP
        // store the bound and obj value
        if(PA.get_run_MIP_Parse_testing_flag() == true){
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

    //tests MIP was read correctly based on expected number of constraints, variables, bin variables, continuous
    //variables non_zeroes and the test has not been flagged as an exception
    

    if(PA.get_run_MIP_Parse_testing_flag() == true){
        cout << "testing MIP Parser" << endl;
        // solve the Parsed MIP problem
        // use the same runtime as the MPS file
        MIP_Problem_CPLEX_Solver MPCS(MP, para.set_generic_MIP_time);
        
        CPLEX_Return_struct MIP_results = MPCS.solve(PA.get_parsed_MIP_randomSeed_flag());
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


    // run NSGA if desired
    if (PA.get_run_nsga_decomp_flag() == true) {
        // start, stop, interval
        vector<double> desired_subproblem_props = PA.get_nsga_props();
        string NSGA_decomp_plot_filename = PA.get_NSGA_decomp_plot_filename();

        // creates all decompositions found in first front. Also returns all decompositions which satisfy the desired proportion
        Problem_Adapter ProblemAdapter;

        // write out to a file the different decompositions found
        vector<individual_information_struct> nsga_con_relax_info_struct = ProblemAdapter.createNSGADecomps(HG, para.nsga_gen, desired_subproblem_props, para.nsga_pop_size);

        bool write_decompositions = false;
        if (write_decompositions){
            try
            {
                std::ofstream outfile;
                std::string decomposition_output_filename = "/home/jake/PhD/Decomposition/Output/testing/NSGA_Tests.test.csv";
                outfile.open(decomposition_output_filename);
                for (auto& ind : nsga_con_relax_info_struct){
                    outfile << ind.number_constraints_relaxed << "," << ind.largest_sp << endl;
                }
            
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
            
        }
    
    // writes out NSGA Decomps to show pareto fronts NSGA_decomp_plot_filename  std::ofstream outfile;
    // outfile.open(output_filename);
    // vector<pop_size_t> front1 = non_dominated_front_2d(f_vals);
    // for (auto& idx : front1) {
    //     outfile << f_vals[idx][1] << "," << f_vals[idx][0] << endl;
    // }

    //instance statistics
    //Binary Var prop
    //Cont Var prop
    //Int Var prop
    //Constraint Types	
    //Number of non zerores
    // average density of constraints, stddev of constraints

    //hold the con_relax vec
    //LSP
    // constraints statistics
    //num constraints_relaxed
    //prop equality constraints/non equality constraints
    //average perc/non_zeroes stddev nonzeros (in constraints relaxed)
    //average, stddev largest ratio (RHS/LHS)
    //average, stddev sum/coefficients (RHS/LHS)

    //solver statistics
    // LB

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
        for (auto& con_relax_info : nsga_con_relax_info_struct) {
            string con_vec_filename = string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name) + "/" + to_string(con_relax_info.largest_sp) + "_vec.csv";
            string decomp_info_filename = string(para.nsga_vector_root_folder) + "/" + string(para.input_instance_name) + "/" + to_string(con_relax_info.largest_sp) + "_info.txt";
           
            // w.writeConVecToFile(con_relax_info.first, con_vec_filename);
            // writeConVecToFile(con_relax_info.first, con_vec_filename);
            // writeDecompInfoToFile(HG, con_relax_info.first, decomp_info_filename);
        }
    }

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
                        bool success_flag = solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stod(prop_string), para.subproblem_solver_runtime_lim, "");
                        if (success_flag == false){
                            cout << "LaPSO not successful" << endl;
                        }
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
    if (PA.get_test_greedy_decomp_flag() == true){
        cout << "solving greedy decomp" << endl;
        string con_count_string(para.test_greedy_decomp);
        
        string stats_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/stat_results/" + "Greedy_" + con_count_string + ".txt";
        string best_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_best_" + con_count_string + ".csv";
        string average_lb_output_filename = string(para.output_root_folder) + "/" + string(para.input_instance_name) + "/lb_results/" + "Greedy_average_" + con_count_string + ".csv";
        //create decomp vec

        std::vector<HG_Edge> edges_copy;

        for (auto& edge : HG.getHGEdges()){
            edges_copy.push_back(HG_Edge{edge.getEdgeIdx(), edge.getNodeIdxs()});
        }
        vector<bool> con_vec;
        con_vec.resize(edges_copy.size(), false);

        //sort by descendig order on constraint size
        std::sort(edges_copy.begin(),edges_copy.end(), greater <HG_Edge>());
        int count = 0;
        for (int i =0; i<edges_copy.size(); i++){
            if (count < (stoi(con_count_string))){
                con_vec[edges_copy[i].getEdgeIdx()] = 1;
                count++;
            }
            else{
                break;
            }
        }
        cout << "solving LaPSO" << endl;
        solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim, "");
    }

    // run LR with random decompositions
    if (PA.get_test_random_decomp_flag() == true){
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
        while (count < (stoi(con_count_string))){
            con_vec[idx] = 1;
            count++;
            idx++;
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(con_vec.begin(), con_vec.end(), g);
    
        cout << "solving LaPSO" << endl;
        solveLapso(argc, argv, MP, HG, con_vec, para.set_ub, stats_output_filename, best_lb_output_filename, average_lb_output_filename, stoi(con_count_string), para.subproblem_solver_runtime_lim
        ,string(para.random_lb_output));
    }

    return 0;
}