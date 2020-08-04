#include "Param.h"
#include "anyoption.h"

namespace mainParam{

void Param::parse(int argc, const char** argv)
{
    AnyOption parser(100);
    parser.setOption("input_root_folder");
    parser.setOption("subproblem_var_prop");
    parser.setOption("output_root_folder");
    parser.setOption("nsga_gen"); 
    parser.setOption("nsga_pop_size"); 
    parser.setOption("input_instance_name"); 
    parser.setOption("decomp_type"); 
    parser.setOption("nsga_vector_root_folder");
    parser.setOption("cplex_decomp_warmstart_file");
    parser.setOption("cplex_decomp_output_root_folder");
    parser.setOption("solve_generic_MIP_output_root_folder");
    parser.setOption("run_nsga_decomp"); 
    parser.setOption("set_ub"); 
    parser.setOption("set_generic_MIP_time"); 
    parser.setOption("run_generic_MIP_solver");
    parser.setOption("run_lapso"); 
    parser.setOption("test_greedy_decomp"); 
    parser.setOption("test_random_decomp");
    parser.setOption("random_lb_output");
    parser.setOption("MIP_num_var"); 
    parser.setOption("MIP_num_cons");
    parser.setOption("MIP_num_bin"); 
    parser.setOption("MIP_num_cont"); 
    parser.setOption("MIP_num_int");
    parser.setOption("MIP_num_non_zeroes"); 
    parser.setOption("MIP_Parse_Test_Exception"); 
    
    

    
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
    if (parser.getValue("solve_generic_MIP_output_root_folder"))
        solve_generic_MIP_output_root_folder = parser.getValue("solve_generic_MIP_output_root_folder");
    if (parser.getValue("run_nsga_decomp"))
        run_nsga_decomp = parser.getValue("run_nsga_decomp");
    if (parser.getValue("set_ub"))
        set_ub = atof(parser.getValue("set_ub"));
    if (parser.getValue("set_generic_MIP_time"))
        set_generic_MIP_time = atof(parser.getValue("set_generic_MIP_time"));
    if (parser.getValue("run_generic_MIP_solver"))
        run_generic_MIP_solver = parser.getValue("run_generic_MIP_solver");
    if (parser.getValue("run_lapso"))
        run_lapso = parser.getValue("run_lapso");
    if (parser.getValue("test_greedy_decomp"))
        test_greedy_decomp = parser.getValue("test_greedy_decomp");
    if (parser.getValue("test_random_decomp"))
        test_random_decomp = parser.getValue("test_random_decomp");
    if (parser.getValue("random_lb_output"))
        random_lb_output = parser.getValue("random_lb_output");
    
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

        
}
};