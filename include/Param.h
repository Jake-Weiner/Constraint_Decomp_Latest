//Parameters used for experiments

#ifndef __PARAM__
#define __PARAM__

#include <string>
#include <map>
#include <limits>


namespace mainParam {

using namespace std;



struct Param {

    /// load commandline arguments to set parameter values.
    /// This function will take any argument pair of the form
    /// --<name> <value> (where name is one of the attributes like
    /// maxIter) and set the corresponding parameter to the value
    void parse(int argc, const char** argv);


    int nsga_gen = 1;
    int nsga_pop_size = 1;
    int greedy_decomp_size = 1000;
    int MIP_num_var = -1;
    int MIP_num_cons = -1;
    int MIP_num_bin = -1;
    int MIP_num_cont = -1;
    int MIP_num_int = -1;
    int MIP_num_non_zeroes = -1;

    int maxIter = 1;
    

    double set_ub = std::numeric_limits<double>::max();
    double subproblem_solver_runtime_lim = 300;
    double set_generic_MIP_time = 100;
  
    // generic MIP parameters
    const char* run_generic_MIP_solver = "false";
    const char* solve_generic_MIP_output_filename = "";
    const char* generic_mip_random_seed = "true";
   


    const char* parsed_mip_random_seed = "true";
    double set_parsed_MIP_time = 100;

    // testing flags
    const char* run_MIP_Parse_testing = "false";
    const char* run_Hypergraph_Partitioning_testing = "false";
    const char* run_NSGA_testing = "false";
    
    const char* run_greedy_decomposition_testing = "false";
    const char* run_evaluate_decompositions_testing_flag = "false";
    const char* run_MIP_Duals_testing = "false";
    const char* run_LR_testing = "false";
    const char* run_statistic_testing_flag = "false";

    // debug flags
    const char* debug_printing = "false";
    
    // statistics output files
    const char* subproblem_statistics_folder = "";
    const char* relaxed_constraint_statistics_folder = "";
    const char* instance_statistics_folder = "";
    // bound and time outputs
    const char* LR_outputs_folder = "";
    const char* LP_outputs_folder = "";



    // decomposition flags
    const char* run_greedy_decomp = "false";
    const char* run_NSGA_decomp = "false";
    
    // decomposition output files
    const char* nsga_decomp_output_file = "";
    const char* greedy_decomp_output_file = "";

    const char* con_vec_filename = "";
    

    const char* MIP_Parse_testing_output_filename = "";
    const char* input_root_folder;
    const char* subproblem_var_prop = "";
    const char* output_root_folder = "";
    const char* input_instance_name = "";
    const char* decomp_type = "";
    const char* nsga_vector_root_folder = "";
    const char* cplex_decomp_warmstart_file = "";
    const char* cplex_decomp_output_root_folder = "";
    
    const char* solve_parsed_MIP_output_filename = "";   
    const char* run_nsga_decomp = "";

    // input decompositions
    const char* decomps_to_remove_red_const_file = "";
    const char* decomps_to_remove_duplicates_file = "";
    const char* general_decomp_file = "";

    // output decompositions
    const char* redundant_const_removed_output_file = "";
    const char* duplicates_removed_output_file = "";
    
    // run flags
    const char* run_constraint_redundancy = "false";
    const char* run_remove_duplicate_constraints = "false";
    const char* run_lapso = "false";
    const char* run_gather_statistics = "false";

    const char* test_greedy_decomp = "";
    const char* test_random_decomp = "";
    
    const char* random_lb_output = "";
    const char* MIP_Parse_Test_Exception = "false";

    // LaPSO parameters
    double total_LR_time_lim = 300;
   
};
};
#endif