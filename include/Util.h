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


bool fileExists (const std::string& name);

#endif