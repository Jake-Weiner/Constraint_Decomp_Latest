#ifndef __Util__
#define __Util__

#include <vector>
#include <utility> 
#include <string>

using std::string;
using std::vector;

// each individual has a con_vec indicating which constraints are relaxed and the largest subproblem size
struct individual_information_struct{
    vector<double> con_vec;
    unsigned int number_constraints_relaxed;
    unsigned int largest_sp;
};

struct CPLEX_Return_struct{
    double bound;
    double obj_val;
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


struct NSGA_ii_instance_statistics{

    double bin_prop;
    double cont_prop;
    double int_prop;
    double average_constraint_density;
    double stddev_constraint_density;
    double equality_constraints_prop;
    double inequality_constraints_prop;

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

#endif