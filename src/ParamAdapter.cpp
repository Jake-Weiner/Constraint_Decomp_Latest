#include "ParamAdapter.h"
#include <boost/algorithm/string.hpp>


using std::string;
using std::cout;
using std::vector;


        

ParamAdapter::ParamAdapter(const mainParam::Param& p){

    createInputFilePaths(p);
    createTestFlags(p);
    if (run_nsga_decomp_flag == true){
        create_nsga_props(p);
    }
    create_NSGA_decomp_plot_filename(p);
}

void ParamAdapter::createTestFlags(const mainParam::Param& p){
    generic_MIP_Solver_Flag = getBoolVal(p.run_generic_MIP_solver);
    run_nsga_decomp_flag = getBoolVal(p.run_nsga_decomp);
    run_lapso_flag = getBoolVal(p.run_lapso);
    test_greedy_decomp_flag = getBoolVal(p.test_greedy_decomp);
    test_random_decomp_flag = getBoolVal(p.test_random_decomp);
    MIP_Parse_Test_Exception_flag = getBoolVal(p.MIP_Parse_Test_Exception);
   
    generic_MIP_randomSeed_flag = getBoolVal(p.generic_mip_random_seed);
    parsed_MIP_randomSeed_flag = getBoolVal(p.parsed_mip_random_seed);

    // testing flags
    run_MIP_Parse_testing_flag = getBoolVal(p.run_MIP_Parse_testing);
    run_Hypergraph_Partitioning_testing_flag = getBoolVal(p.run_Hypergraph_Partitioning_testing);
    run_NSGA_testing_flag = getBoolVal(p.run_NSGA_testing);
    run_constraint_redundancy_flag = getBoolVal(p.run_constraint_redundancy);
    run_greedy_decomposition_testing_flag = getBoolVal(p.run_greedy_decomposition_testing);
    run_evaluate_decompositions_testing_flag = getBoolVal(p.run_evaluate_decompositions_testing_flag);
    run_MIP_Duals_testing_flag = getBoolVal(p.run_MIP_Duals_testing);
    run_LR_testing_flag = getBoolVal(p.run_LR_testing);
    run_statistic_testing_flag = getBoolVal(p.run_statistic_testing_flag);

    // running flags
    run_remove_duplicate_constraints_flag = getBoolVal(p.run_remove_duplicate_constraints);

    // decomp flags
    run_greedy_decomp_flag = getBoolVal(p.run_greedy_decomp);
    run_NSGA_decomp_flag = getBoolVal(p.run_NSGA_decomp);
    
    // debug printing flags
    debug_printing_flag = getBoolVal(p.debug_printing);
}

// return a bool val from the given input char*
bool ParamAdapter::getBoolVal(const char* input){
    string temp_string = std::string(input);
    boost::algorithm::to_lower(temp_string);
    return char_to_BOOL[temp_string];
}


void ParamAdapter::create_NSGA_decomp_plot_filename(const mainParam::Param& p){
    NSGA_decomp_plot_filename = std::string(p.nsga_vector_root_folder) + "/" + std::string(p.input_instance_name) + "/population_plot.csv";
}

void ParamAdapter::createInputFilePaths(const mainParam::Param& p){
    Problem_File = string(p.input_root_folder) + "/" + string(p.input_instance_name);
}

/*// start , stop,  interval -- 1, 10, 10, 10- 1 + 10 - 1 / 0.1 - 1 - 10  0.1, 0.2,0.3, 0.4*/
void ParamAdapter::create_nsga_props(const mainParam::Param& p){
    int start_idx = 0;
    int end_idx = 1;
    int number_points_idx = 2;
    string subproblem_var_prop(p.subproblem_var_prop);
    std::vector<std::string> subproblem_var_prop_info;
    boost::split(subproblem_var_prop_info, subproblem_var_prop, boost::is_any_of(":"), boost::token_compress_on);
    // should only be 3 values in vector
    if (subproblem_var_prop_info.size() == 3){
        std::cout << "Creating Proportion Requirements Using Predetermined Intervals" << std::endl;
        double start_prop = stod(subproblem_var_prop_info[start_idx]);
        double end_prop = stod(subproblem_var_prop_info[end_idx]);
        double gap = (end_prop - start_prop) / (stod(subproblem_var_prop_info[number_points_idx])- 1);
        for (double i = start_prop; i<= end_prop ; i+= gap){
        nsga_props.push_back(i);
        }
    }
    else{
        std::cout << "No Predetermined Intervals Given, Testing All Decompositions" << std::endl;
    }
    
}



