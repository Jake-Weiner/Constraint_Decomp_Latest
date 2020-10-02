#ifndef __Util__
#define __Util__

#include <vector>
#include <utility> 
#include <string>
#include "Hypergraph.h"
#include "MIP_Problem.h"

using std::string;
using std::vector;

const int con_vec_starting_idx_from_file = 1;

// each individual has a con_vec indicating which constraints are relaxed and the largest subproblem size
struct individual_information_struct{
    vector<double> con_vec;
    unsigned int number_constraints_relaxed;
    unsigned int largest_sp;
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
    vector<double>* con_vec_ptr;
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
    vector<double>* con_vec_ptr;
    double LSP_prop;
    double constraints_relaxed_prop;
   
    double bin_prop;
    double int_prop;
    double cont_prop;
};


struct Subproblem_Statistics : public Common_Statistical_Measures{
    int decomposition_idx;
    vector<double>* con_vec_ptr;
    
    // runtime statistics:

    vector<double> mip_times;
    double average_mip_time;
    double max_mip_time;
    double min_mip_time;
    double stddev_mip_time;
    
    vector<double> lp_times;
    double average_lp_time;
    double max_lp_time;
    double min_lp_time; 
    
    // variable statistics
    vector<double> variable_props;
    double average_variable_prop;
    double stddev_variable_prop;
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
    vector<double> equality_prop;
    double average_equality_prop;
    double stddev_equality_prop;
    double average_inequality_prop;
    double stddev_inequality_prop;
    double average_total_constraint_prop;
    double stddev_total_constraint_prop;
    
    // double LSP_prop;
    // double constraints_relaxed_prop;
};


// pair_hash used for hashmaps with pair type as the key. Creates a hash of the pair key.
struct pair_hash
{
	template <class T1, class T2>
	std::size_t operator() (const std::pair<T1, T2> &pair) const
	{
		return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
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


#endif