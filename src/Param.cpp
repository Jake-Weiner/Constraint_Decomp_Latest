#include "Param.h"
#include "anyoption.h"

namespace mainParam{

void Param::parse(int argc, const char** argv)
{
    AnyOption parser(100);
    parser.setOption("input_root_folder");
    parser.setOption("subproblem_var_prop");
    parser.setOption("output_root_folder");
    parser.setOption("input_instance_name"); 
    parser.setOption("decomp_type"); 
    parser.setOption("nsga_vector_root_folder");
    parser.setOption("cplex_decomp_warmstart_file");
    parser.setOption("cplex_decomp_output_root_folder");
    parser.setOption("solve_generic_MIP_output_filename");
    parser.setOption("generic_mip_random_seed");
    parser.setOption("parsed_mip_random_seed");
    parser.setOption("solve_parsed_MIP_output_filename");
    parser.setOption("run_nsga_decomp"); 
    parser.setOption("set_ub"); 
    parser.setOption("set_generic_MIP_time");
    parser.setOption("set_parsed_MIP_time");  
    parser.setOption("run_generic_MIP_solver");

    // Decomp parameters
    parser.setOption("nsga_gen"); 
    parser.setOption("nsga_pop_size"); 
    parser.setOption("greedy_decomp_size"); 

    // MPS File Statistics
    parser.setOption("MIP_num_var"); 
    parser.setOption("MIP_num_cons");
    parser.setOption("MIP_num_bin"); 
    parser.setOption("MIP_num_cont"); 
    parser.setOption("MIP_num_int");
    parser.setOption("MIP_num_non_zeroes"); 
    parser.setOption("MIP_Parse_Test_Exception");

    // testing flags/parameters
    parser.setOption("run_Hypergraph_Partitioning_testing");  
    parser.setOption("run_NSGA_testing");  
    parser.setOption("run_MIP_Parse_testing");
    
    parser.setOption("run_greedy_decomposition_testing");
    parser.setOption("run_MIP_Duals_testing");
    parser.setOption("run_LR_testing");
    parser.setOption("run_statistic_testing_flag");
    parser.setOption("run_LSP_testing_flag");
    
    // input files
    parser.setOption("decomps_to_remove_red_const_file");
    parser.setOption("decomps_to_remove_duplicates_file");
    parser.setOption("general_decomp_file");
 

    // output files
    // statistics folders
    parser.setOption("subproblem_statistics_folder");
    parser.setOption("relaxed_constraint_statistics_folder");
    parser.setOption("instance_statistics_folder");
    parser.setOption("LR_outputs_folder");
    parser.setOption("LP_outputs_folder");
    
    // decomposition redundancy removal
    parser.setOption("redundant_const_removed_output_file");
    parser.setOption("duplicates_removed_output_file");
    

    parser.setOption("run_evaluate_decompositions_testing_flag");
    parser.setOption("MIP_Parse_testing_output_filename");
    parser.setOption("test_greedy_decomp"); 
    parser.setOption("test_random_decomp");

    //LR Parameters
    parser.setOption("maxIter");
    parser.setOption("total_LR_time_lim");

    // decomp output files
    parser.setOption("nsga_decomp_output_file");
    parser.setOption("nsga_pareto_optimal_output_file");
    parser.setOption("greedy_decomp_output_file");
    
    // debug flags
    parser.setOption("debug_printing");

    // running flags/parameters
    parser.setOption("run_parsed_MIP_solver");
    parser.setOption("run_lapso");
    parser.setOption("run_greedy_decomp");
    parser.setOption("run_constraint_redundancy");
    parser.setOption("run_remove_duplicate_constraints");
    parser.setOption("run_gather_statistics");

    parser.setOption("random_lb_output");
    parser.setOption("con_vec_filename");

    if (argc == -1) // abuse of function
        parser.processFile(*argv);
    else
        parser.processCommandArgs(argc, argv);
    if (parser.getValue("input_root_folder"))
        input_root_folder = parser.getValue("input_root_folder");
    if (parser.getValue("subproblem_var_prop"))
        subproblem_var_prop = parser.getValue("subproblem_var_prop");
    if (parser.getValue("output_root_folder"))
        output_root_folder = parser.getValue("output_root_folder");
    if (parser.getValue("nsga_gen"))
        nsga_gen = atoi(parser.getValue("nsga_gen"));
    if (parser.getValue("nsga_pop_size"))
        nsga_pop_size = atoi(parser.getValue("nsga_pop_size"));
    if (parser.getValue("input_instance_name"))
        input_instance_name = parser.getValue("input_instance_name");
    if (parser.getValue("decomp_type"))
        decomp_type = parser.getValue("decomp_type");
    if (parser.getValue("nsga_vector_root_folder"))
        nsga_vector_root_folder = parser.getValue("nsga_vector_root_folder");
    if (parser.getValue("cplex_decomp_warmstart_file"))
        cplex_decomp_warmstart_file = parser.getValue("cplex_decomp_warmstart_file");
    if (parser.getValue("cplex_decomp_output_root_folder"))
        cplex_decomp_output_root_folder = parser.getValue("cplex_decomp_output_root_folder");
    if (parser.getValue("solve_generic_MIP_output_filename"))
        solve_generic_MIP_output_filename = parser.getValue("solve_generic_MIP_output_filename");
    if (parser.getValue("run_nsga_decomp"))
        run_nsga_decomp = parser.getValue("run_nsga_decomp");
    if (parser.getValue("set_ub"))
        set_ub = atof(parser.getValue("set_ub"));
    if (parser.getValue("set_generic_MIP_time"))
        set_generic_MIP_time = atof(parser.getValue("set_generic_MIP_time"));
    if (parser.getValue("set_parsed_MIP_time"))
        set_parsed_MIP_time = atof(parser.getValue("set_parsed_MIP_time"));
    if (parser.getValue("generic_mip_random_seed"))
        generic_mip_random_seed = parser.getValue("generic_mip_random_seed");
    if (parser.getValue("parsed_mip_random_seed"))
        parsed_mip_random_seed = parser.getValue("parsed_mip_random_seed");
    if (parser.getValue("subproblem_solver_runtime_lim"))
        subproblem_solver_runtime_lim = atof(parser.getValue("subproblem_solver_runtime_lim"));
    if (parser.getValue("run_generic_MIP_solver"))
        run_generic_MIP_solver = parser.getValue("run_generic_MIP_solver");
    if (parser.getValue("run_MIP_Parse_testing"))
        run_MIP_Parse_testing = parser.getValue("run_MIP_Parse_testing");
    if (parser.getValue("MIP_Parse_testing_output_filename"))
        MIP_Parse_testing_output_filename = parser.getValue("MIP_Parse_testing_output_filename");
    if (parser.getValue("run_lapso"))
        run_lapso = parser.getValue("run_lapso");
    if (parser.getValue("test_greedy_decomp"))
        test_greedy_decomp = parser.getValue("test_greedy_decomp");
    if (parser.getValue("test_random_decomp"))
        test_random_decomp = parser.getValue("test_random_decomp");
    if (parser.getValue("random_lb_output"))
        random_lb_output = parser.getValue("random_lb_output");
    
    if (parser.getValue("con_vec_filename"))
        con_vec_filename = parser.getValue("con_vec_filename");

    // running flags
    
    if (parser.getValue("run_greedy_decomp"))
        run_greedy_decomp = parser.getValue("run_greedy_decomp");
    
    if (parser.getValue("run_remove_duplicate_constraints"))
        run_remove_duplicate_constraints = parser.getValue("run_remove_duplicate_constraints");


    // decomp parameters
    if (parser.getValue("greedy_decomp_size"))
        greedy_decomp_size = atoi(parser.getValue("greedy_decomp_size"));
    

    // decomp output files
    if (parser.getValue("nsga_decomp_output_file"))
        nsga_decomp_output_file = parser.getValue("nsga_decomp_output_file");  
    if (parser.getValue("nsga_pareto_optimal_output_file"))
        nsga_pareto_optimal_output_file = parser.getValue("nsga_pareto_optimal_output_file");  
    if (parser.getValue("greedy_decomp_output_file"))
        greedy_decomp_output_file = parser.getValue("greedy_decomp_output_file");
    if (parser.getValue("redundant_const_removed_output_file"))
        redundant_const_removed_output_file = parser.getValue("redundant_const_removed_output_file");
    if (parser.getValue("duplicates_removed_output_file"))
        duplicates_removed_output_file = parser.getValue("duplicates_removed_output_file");
    
    
    // decomp input files
    if (parser.getValue("decomps_to_remove_red_const_file"))
        decomps_to_remove_red_const_file = parser.getValue("decomps_to_remove_red_const_file");
    if (parser.getValue("decomps_to_remove_duplicates_file"))
        decomps_to_remove_duplicates_file = parser.getValue("decomps_to_remove_duplicates_file");
    if (parser.getValue("general_decomp_file"))
        general_decomp_file = parser.getValue("general_decomp_file");
    
    // debug flags
    if (parser.getValue("debug_printing"))
        debug_printing = parser.getValue("debug_printing");
        
    // running flags
    if (parser.getValue("run_gather_statistics"))
        run_gather_statistics = parser.getValue("run_gather_statistics");

    
    // statistics output files
    if (parser.getValue("subproblem_statistics_folder"))
        subproblem_statistics_folder = parser.getValue("subproblem_statistics_folder");
    if (parser.getValue("instance_statistics_folder"))
        instance_statistics_folder = parser.getValue("instance_statistics_folder");
    if (parser.getValue("relaxed_constraint_statistics_folder"))
        relaxed_constraint_statistics_folder = parser.getValue("relaxed_constraint_statistics_folder");
    if (parser.getValue("LR_outputs_folder"))
        LR_outputs_folder = parser.getValue("LR_outputs_folder");
    if (parser.getValue("LP_outputs_folder"))
        LP_outputs_folder = parser.getValue("LP_outputs_folder");

    // unit testing flags
    if (parser.getValue("run_Hypergraph_Partitioning_testing"))
        run_Hypergraph_Partitioning_testing = parser.getValue("run_Hypergraph_Partitioning_testing");
    if (parser.getValue("run_NSGA_testing"))
        run_NSGA_testing = parser.getValue("run_NSGA_testing");
    if (parser.getValue("run_constraint_redundancy"))
        run_constraint_redundancy = parser.getValue("run_constraint_redundancy");
    if (parser.getValue("run_greedy_decomposition_testing"))
        run_greedy_decomposition_testing = parser.getValue("run_greedy_decomposition_testing");
    if (parser.getValue("run_MIP_Duals_testing"))
        run_MIP_Duals_testing = parser.getValue("run_MIP_Duals_testing");
    if (parser.getValue("run_LR_testing"))
        run_LR_testing = parser.getValue("run_LR_testing");
    if (parser.getValue("run_statistic_testing_flag"))
        run_statistic_testing_flag = parser.getValue("run_statistic_testing_flag");
    if (parser.getValue("run_LSP_testing_flag"))
        run_LSP_testing_flag = parser.getValue("run_LSP_testing_flag");

    // input MIP parameters
    if (parser.getValue("MIP_num_var"))
        MIP_num_var = atoi(parser.getValue("MIP_num_var"));
    if (parser.getValue("MIP_num_cons"))
        MIP_num_cons = atoi(parser.getValue("MIP_num_cons"));
    if (parser.getValue("MIP_num_bin"))
        MIP_num_bin = atoi(parser.getValue("MIP_num_bin"));
    if (parser.getValue("MIP_num_cont"))
        MIP_num_cont = atoi(parser.getValue("MIP_num_cont"));
    if (parser.getValue("MIP_num_int"))
        MIP_num_int = atoi(parser.getValue("MIP_num_int"));
    if (parser.getValue("MIP_num_non_zeroes"))
        MIP_num_non_zeroes = atoi(parser.getValue("MIP_num_non_zeroes"));
    if (parser.getValue("MIP_Parse_Test_Exception"))
        MIP_Parse_Test_Exception = (parser.getValue("MIP_Parse_Test_Exception"));
    
    // LR parameters
    
    if (parser.getValue("maxIter")){
        maxIter = atoi(parser.getValue("maxIter"));
    }
    if (parser.getValue("total_LR_time_lim")){
        total_LR_time_lim = atof(parser.getValue("total_LR_time_lim"));
    }
    

}
};