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
    string run_generic_MIP_solver_string = std::string(p.run_generic_MIP_solver);
    boost::algorithm::to_lower(run_generic_MIP_solver_string);
    generic_MIP_Solver_Flag = char_to_BOOL[run_generic_MIP_solver_string];
    
    string run_nsga_decomp_string = std::string(p.run_nsga_decomp);
    boost::algorithm::to_lower(run_nsga_decomp_string);
    run_nsga_decomp_flag = char_to_BOOL[run_nsga_decomp_string];
    
    string run_lapso_string = std::string(p.run_lapso);
    boost::algorithm::to_lower(run_lapso_string);
    run_lapso_flag = char_to_BOOL[run_lapso_string];

    string test_greedy_decomp_string = std::string(p.test_greedy_decomp);
    boost::algorithm::to_lower(test_greedy_decomp_string);
    test_greedy_decomp_flag = char_to_BOOL[test_greedy_decomp_string];

    string test_random_decomp_string = std::string(p.test_random_decomp);
    boost::algorithm::to_lower(test_random_decomp_string);
    test_random_decomp_flag = char_to_BOOL[test_random_decomp_string];
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



