#ifndef __Util__
#define __Util__

#include <vector>
#include <utility> 
#include <string>
#include "Hypergraph.h"
#include "MIP_Problem.h"
#include <boost/functional/hash.hpp>

using std::string;
using std::vector;

const int con_vec_starting_idx_from_file = 1;

// each individual has a con_vec indicating which constraints are relaxed and the largest subproblem size
struct individual_information_struct{
    vector<double> con_vec;
    unsigned int number_constraints_relaxed;
    unsigned int largest_sp;
};

struct SubproblemRawDataFilenames{
    // MIP Times
    string MIP_times_filename;
    // MIP Bounds
    string MIP_bounds_filename;
    // LP Bounds
    string LP_bounds_filename;
    // LP Times
    string LP_time_filename;
    // Block SUM obj vals
    string sum_obj_filename;
    // Block SUM abs(obj) vals
    string sum_absobj_filename;
    // block average RHS vals
    string average_rhs_filename;
    // block average abs(RHS) vals
    string average_absrhs_filename;
    // Block largest abs(RHSLHS) ratios
    string largestRHSLHS_filename;
    // Block RHS range
    string RHS_range_filename;
    // Block Const props
    string const_props_filename;
    // Block equality props filename
    string equality_props_filename;
    // Block densities
    string densities_filename;
    // Block Shapes
    string shapes_filename;
};

struct RelaxedConstraintsRawDataFilenames{
    // props filename inludes single prop values - relaxed constraint prop, equality prop, bin prop, int prop, cont prop
    string props_filename;
    // non zero props of all relaxed constraints
    string non_zero_props_filename; 
    // Largest abs(RHS/LHS) ratios
    string largest_RHSLHS_ratios_filename;
    // Sum obj Coeffs
    string sum_obj_coeffs_of_constraints_filename;
    // Sum abs(obj coeffs)
    string sum_abs_obj_coeffs_of_constraints_filename;
    // Block SUM RHS vals
    string RHS_values_filename;
};


struct CPLEX_Return_struct{
    double bound;
    double obj_val;
    vector<double> dual_vals;
};

struct Common_Statistical_Measures{
    
    // proportion of relaxed constraints which are equality
    double equality_constaints_relaxed_prop;
    // proportion of relaxed constraints which are inequality = 1-equality_constaints_relaxed_prop
    double inequality_constaints_relaxed_prop;
    // non_zero prop in constraints relaxed
    double average_nonzero_prop;
    double stddev_nonzero_prop;

    // ratio of abs(RHS/largest coeff in constraint)

    vector<pair<int,double>> RHS_largest_coeff_ratios;
    double average_largest_ratio;
    double stddev_largest_ratio;
    double max_largest_ratio;
    double min_largest_ratio;

    // sum of all abs(RHS/LHS) values
    vector<pair<int,double>> sum_RHS_LHS_ratios;
    double average_sum_ratio;
    double stddev_sum_ratio;
    double max_sum_ratio;
    double min_sum_ratio;

    // sum of all abs(obj) values
    vector<pair<int,double>> sum_obj_values;
    double average_sum_obj;
    double stddev_sum_obj;
    double max_sum_obj;
    double min_sum_obj;
};



struct MIP_Parsing_Test_struct{
    string instance_name;
    double MPS_bound;
    double MPS_obj_val;
    double Parsed_bound;
    double Parsed_obj_val;
    int MIP_num_var;
    int Parsed_num_var;
    int MIP_num_const;
    int Parsed_num_const;
    int MIP_num_bin;
    int Parsed_num_bin;
    int MIP_num_cont;
    int Parsed_num_cont;
    int MIP_num_int;
    int Parsed_num_int;
    int num_non_zeroes;
    int Parsed_non_zeroes;
};

struct MIP_problems_to_solve {
    int max_subprobem_size;
    int max_num_subproblems;
    const char* output_file;
    int solver_time;
};

struct NSGA_ii_characteristics {
    int number_generations;
    int population_size;
    bool greedy;
};

struct LaPSOOutputFilenames{
    string output_stats_filename; 
    string output_best_lb_filename;
    string output_average_lb_filename;
    string final_lb_filename;
    string subproblem_statistics_folder;
    string instance_statistics_folder;
    string relaxed_constraints_statistics_folder;
    // SubproblemRawDataFilenames raw_subproblem_data_filenames;
    // RelaxedConstraintsRawDataFilenames raw_rc_data_filenames;
    string instance_statistics_filename;

};


struct solveLapsoStruct{
    int argc;
    char** argv;
    vector<bool>* con_relax_vector_ptr;
    double best_ub_sol;
    
    double sp_prop; 
    double sp_solver_time_limit;
    string final_lb_filename;
};


struct instance_statistics{
    double bin_prop;
    double cont_prop;
    double int_prop;
    double average_constraint_density;
    double stddev_constraint_density;
    double equality_constraints_prop;
    double inequality_constraints_prop;
    
    // ratio of abs(RHS/largest coeff in constraint)
    double average_largest_ratio;
    double stddev_largest_ratio;
    double max_largest_ratio;
    double min_largest_ratio;

    // sum of all abs(RHS/LHS) values
    double average_sum_ratio;
    double stddev_sum_ratio;
    double max_sum_ratio;
    double min_sum_ratio;

    // sum of all abs(obj) values
    double average_sum_obj;
    double stddev_sum_obj;
    double max_sum_obj;
    double min_sum_obj;
    //Cont Var prop
    //Int Var prop
    //Constraint Types	
    //Number of non zerores
    // average density of constraints, stddev of constraints
};

struct NSGA_ii_relaxed_constraint_statistics{

    int decomposition_idx;
    
    double LSP_prop;
    double num_constraints_relaxed_prop;
    // proportion of relaxed constraints which are equality
    double equality_constaints_relaxed_prop;
    // proportion of relaxed constraints which are inequality = 1-equality_constaints_relaxed_prop
    double inequality_constaints_relaxed_prop;
    // non_zero prop in constraints relaxed
    double average_nonzero_prop;
    double stddev_nonzero_prop;
    // ratio of RHS to largest coeff in constraint
    double average_largest_ratio;
    double stddev_largest_ratio;
};

struct Nonruntime_Relaxed_Constraint_Statistics : public Common_Statistical_Measures{
    int decomposition_idx;
    
    double LSP_prop;
    double constraints_relaxed_prop;
   
    double bin_prop;
    double int_prop;
    double cont_prop;
};


struct Subproblem_Statistics{

    ~Subproblem_Statistics(){};

    int decomposition_idx;
    
    // runtime statistics:

    // for each subproblem, store the time required to solve the subproblem
    vector<double> mip_times;
    vector<double> mip_obj_solutions;
    double average_mip_time;
    double max_mip_time;
    double min_mip_time;
    double stddev_mip_time;

    double average_mip_obj_soln;
    double max_mip_obj_soln;
    double min_mip_obj_soln;
    double stddev_mip_obj_soln;
    
    vector<double> lp_times;
    double average_lp_time;
    double max_lp_time;
    double min_lp_time;
    double stddev_lp_time;

    vector<double> lp_obj_solutions;
    double average_lp_obj_soln;
    double max_lp_obj_soln;
    double min_lp_obj_soln;
    double stddev_lp_obj_soln;

    
    // variable statistics
    // proportion of problem variables in the subproblem
    vector<double> block_variable_props;
    double average_block_variable_prop;
    double stddev_block_variable_prop;
    double max_block_variable_prop;

    vector<double> bin_props;
    double average_bin_prop;
    double stddev_bin_prop;

    vector<double> int_props;
    double average_int_prop;
    double stddev_int_prop;

    vector<double> cont_props;
    double average_cont_prop;
    double stddev_cont_prop;

    // constraint statistics

    vector<double> total_constr_props; // proportion of total constraints in subproblem
    double average_total_constraint_prop;
    double stddev_total_constraint_prop;

    vector<double> equality_props;
    double average_equality_prop;
    double stddev_equality_prop;
    
    vector<double> inequality_props;
    double average_inequality_prop;
    double stddev_inequality_prop;
   
    // average of objective coefficients in each of the blocks - This will 

    // average abs(objective coefficients) in each block... = sum(objective coeffs of constraints) / num constraints
    vector<double> sum_block_obj_values;
    double average_of_sum_block_obj_values;
    double stddev_of_sum_block_obj_values;

    // abs(objective coefficients) in each block... = sum(objective coeffs of constraints) / num constraints
    vector<double> sum_abs_block_obj_values;
    double average_of_sum_abs_block_obj_values;
    double stddev_of_sum_abs_block_obj_values;

    // average abs(RHS) value of each block
    vector<double> average_block_RHS_values;
    double average_of_average_block_RHS_values;
    double stddev_of_average_block_RHS_values;

    //average of block largest RHS/LHS ratios
    vector<double> average_block_Largest_RHSLHS_ratio;
    double average_of_average_block_Largest_RHSLHS_ratio;
    double stddev_of_average_block_Largest_RHSLHS_ratio;

    //average block shapes, where shape is no.of variables / number of constraints
    vector<double> average_block_shape;
    double average_of_average_block_shapes;
    double stddev_of_average_block_shapes;

    // Max RHS - Min RHS for each block
    vector<double> block_RHS_range;
    double average_block_RHS_range;
    double stddev_block_RHS_range;

    // Block densities = Non_zeroes / (num constraints * num variables)
    vector<double> block_densities;
    double average_block_density;
    double stddev_block_density;

};

struct Relaxed_Constraint_Statistics {

     ~Relaxed_Constraint_Statistics(){};

    int decomposition_idx;
    
    // constraint statistics
    vector<double> equality_props;
    double average_equality_prop;
    double stddev_equality_prop;
    
    vector<double> inequality_props;
    double average_inequality_prop;
    double stddev_inequality_prop;
    
    // proportion of relaxed constraints compared to all constraints in the original problem
    double relaxed_constraint_prop;
    // variable statistics
    // proportion of problem variables in the subproblem
    double bin_prop;
    double int_prop;
    double cont_prop;
    
    // non zero counts of all relaxed constraints
    vector<double> non_zero_props;
    double average_non_zero_prop;
    double stddev_non_zero_prop;
    
    vector<double> largest_RHSLHS_ratios;
    double average_RHSLHS_ratio;
    double stddev_RHSLHS_ratio;

    vector<double> sum_obj_coeffs_of_constraints;
    double average_sum_obj_coeffs;
    double stddev_sum_obj_coeffs;

    vector<double> sum_abs_obj_coeffs_of_constraints;
    double average_abs_sum_obj_coeffs;
    double stddev_abs_sum_obj_coeffs;

    vector<double> RHS_values;
    double average_RHS;
    double stddev_RHS;

    vector<double> sum_LHS_values;
    double average_sum_LHS;
    double stddev_sum_LHS;
   
};

// calculates the statistics for a given vector of data (min, max, average stddev)
// stddev is sqrt(1/n sum(x_i - x_mean)^2)
template <class T1>
std::tuple<T1,T1,double,double> getStatistics(const std::vector<T1>& input_vec){

    T1 sum = 0;
    T1 min = std::numeric_limits<T1>::max();
    T1 max = -std::numeric_limits<T1>::max();
    for (const auto& element : input_vec){
        if (element > max){
            max = element;
        }
        if (element < min){
            min = element;
        }
        sum += element;
    }
 
    double average = static_cast<double>(sum) / static_cast<double>(input_vec.size());
    double variance = 0.0;

    for (const auto& element : input_vec){
        variance += pow(static_cast<double>(element) - average, 2);
    }
    double stddev = sqrt(variance / input_vec.size());
    return std::make_tuple(min,max,average,stddev);
}

// pair_hash used for hashmaps with pair type as the key. Creates a hash of the pair key.
struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2> &pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
	}
};

// container_hash used for creating hash when vector<typename> is the key
template <typename Container> // we can make this generic for any container [1]
struct container_hash {
    std::size_t operator()(Container const& c) const {
        return boost::hash_range(c.begin(), c.end());
    }
};




    //con_relax vec
    //based off NSGA_results
    //LSP
    // constraints statistics
    //num constraints_relaxed
    //prop equality constraints/non equality constraints
    //average perc/non_zeroes stddev nonzeros (in constraints relaxed)
    //average, stddev largest ratio (RHS/LHS)
    //average, stddev sum/coefficients (RHS/LHS)

bool fileExists (const std::string& name);
vector<bool> readInConVecFromFile(const std::string& name);
vector<double> readInDualsFromFile(const std::string& filename);



#endif