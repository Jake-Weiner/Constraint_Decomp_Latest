#ifndef __DECOMPOSITIONSTATISTICS__
#define __DECOMPOSITIONSTATISTICS__


#include <vector>
#include <tuple>
#include "Hypergraph.h"

using std::vector;

class MIPProblemProbe;

namespace DecompositionStatistics {

struct SubproblemConstraintStatistics{
    double equality_prop;
    double average_RHS_val;
    double average_abs_RHS_val;
    double average_largest_RHSLHS_ratio;
    double largest_RHS_range;
    double num_non_zeroes;
};

struct SubproblemVariableStatistics{
    double obj_val_range;
    double sum_block_obj_val;
    double sum_abs_block_obj_val;
};

struct RelaxedConstraintStatistics{
    std::tuple<vector<double>, vector<double>, vector<double>> relaxed_const_var_props;
    double equality_const_prop;
    vector<int> constraint_non_zeroes_counts;
    vector<double> largest_RHSLHS_ratios;
    vector<double> sum_obj_coeffs_of_constraints;
    vector<double> sum_abs_obj_coeffs_of_constraints;
    vector<double> RHS_values;
};

class Subproblems {

    public:
        Subproblems(int decomposition_idx) : decomposition_idx(decomposition_idx){};
        ~Subproblems(){};
        void resize(const int& number_subproblems);
        void generateBlockStatistics(const Partition_Struct& ps, MIPProblemProbe& MPP);

        int decomposition_idx;       
        // if there exists a subproblem in which the root node couldn't be found, then set this flag to false
        vector<bool> subproblem_optimality_success;
        vector<bool> subproblem_lp_found;
        vector<bool> subproblem_attempted;
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

        // average abs(objective coefficients) in each block... = sum(objective coeffs of constraints) / num constraints
        vector<double> sum_block_obj_values;
        double average_of_sum_block_obj_values;
        double stddev_of_sum_block_obj_values;

        // abs(objective coefficients) in each block... = sum(objective coeffs of constraints) / num constraints
        vector<double> sum_abs_block_obj_values;
        double average_of_sum_abs_block_obj_values;
        double stddev_of_sum_abs_block_obj_values;

        // range of objective values in block
        vector<double> block_obj_val_ranges;

        // average RHS value of each block
        vector<double> average_block_RHS_values;
        double average_of_average_block_RHS_values;
        double stddev_of_average_block_RHS_values;


        // average abs(RHS) value of each block
        vector<double> average_block_absRHS_values;
        double average_of_average_block_absRHS_values;
        double stddev_of_average_block_absRHS_values;

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

        vector<double> non_zeroes_count;
};

class RelaxedConstraints {

    public:
        RelaxedConstraints(const int& decomposition_idx) : decomposition_idx(decomposition_idx){};
        ~RelaxedConstraints(){};

        void generateStatistics(MIPProblemProbe& MPP, const vector<int>& relaxed_constraint_indices);
        int decomposition_idx;
        
        // constraint statistics
        double equality_prop;
     
        // proportion of relaxed constraints compared to all constraints in the original problem
        double relaxed_constraint_prop;
        // variable statistics
        // proportion of variable types in each of the relaxed constraints 
        vector<double> bin_props;
        vector<double> int_props;
        vector<double> cont_props;
        
        // non zero counts of all relaxed constraints
        vector<int> non_zero_counts;
        // double average_non_zero_prop;
        // double stddev_non_zero_prop;
        
        vector<double> largest_RHSLHS_ratios;
        // double average_RHSLHS_ratio;
        // double stddev_RHSLHS_ratio;

        vector<double> sum_obj_coeffs_of_constraints;
        // double average_sum_obj_coeffs;
        // double stddev_sum_obj_coeffs;

        vector<double> sum_abs_obj_coeffs_of_constraints;
        // double average_abs_sum_obj_coeffs;
        // double stddev_abs_sum_obj_coeffs;

        vector<double> RHS_values;
        // double average_RHS;
        // double stddev_RHS;

};


struct RawInstanceConstrVals{
    vector<int> non_zeroes;
    vector<double> rhs_vals;
    vector<double> constr_sum_obj;
    vector<double> constr_sum_abs_obj;
    vector<double> constr_largest_rhslhs_vals;
    vector<double> constr_sum_rhslhs_vals;
    vector<double> constr_sum_lhs_vals;
};


class Instance{

    public: 

        Instance(MIPProblemProbe& MPP){
            populateInstanceStatistics(MPP);
        };
        ~Instance(){};

        void populateInstanceStatistics(MIPProblemProbe& MPP);

        // processed values for normalisation
        void getObjExtremes(MIPProblemProbe& MPP);
        void getRHSExtremes(MIPProblemProbe& MPP);
        void getRHSLHSExtremes(MIPProblemProbe& MPP);
        void getSumObjExtremes(MIPProblemProbe& MPP);
        void getSumAbsObjExtremes(MIPProblemProbe& MPP);
        
    
        void getObjTerms(MIPProblemProbe& MPP);
        // get Constraint Raw values
        void getConstRawValues(MIPProblemProbe& MPP);
        // void getRHSTerms(MIPProblemProbe& MPP);
        // void getConstNonZeroes(MIPProblemProbe& MPP);
        // void getConstrSumObj(MIPProblemProbe& MPP);
        // void getConstrSumAbsObj(MIPProblemProbe& MPP);
        // void getConstrRHSLHS(MIPProblemProbe& MPP);

        int num_var;
        int num_const;
        int num_bin;
        int num_cont;
        int num_int;
        int num_non_zeroes;
        int num_equality_constr;
        double max_obj;
        double min_obj;
        double max_rhs;
        double min_rhs;
        double best_known_bound;
        double min_sum_obj;
        double max_sum_obj;
        double min_sum_abs_obj;
        double max_sum_abs_obj;
        double min_rhslhs;
        double max_rhslhs;
        vector<double> obj_val_terms;

        RawInstanceConstrVals ricv;
};



class LROutputs{

    public:
        LROutputs( int decomposition_idx, double bound, double time) : 
        decomposition_idx(decomposition_idx), bound(bound), time(time)
        {};
        int decomposition_idx;
        double bound;
        double time;
};


}

#endif