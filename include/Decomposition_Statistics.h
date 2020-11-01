#ifndef __DECOMPOSITIONSTATISTICS__
#define __DECOMPOSITIONSTATISTICS__

#include <vector>
#include "MIPProblemProbe.h"

using std::vector;

namespace Decomposition_Statistics {

class Subproblems {

    Subproblems(int decomposition_idx) : decomposition_idx(decomposition_idx){};
    ~Subproblems(){};


    public:

        void generate_statistics(MIPProblemProbe& MPP, const vector<int>& relaxed_constraint_indices);



        int decomposition_idx;

       

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

class Relaxed_Constraints {

     ~Relaxed_Constraints(){};

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


}

#endif