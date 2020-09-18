#include "MIPProblemProbe.h"
#include <math.h>

void MIPProblemProbe::populateInstanceStatistics(instance_statistics& is, MIP_Problem& MP)
{
    is.bin_prop = double(MP.getNumBin()) / double(MP.getNumVariables());
    is.int_prop = double(MP.getNumInt()) / double(MP.getNumVariables());
    is.cont_prop = double(MP.getNumCont()) / double(MP.getNumVariables());
    is.average_constraint_density = MP.getAverageConstraintDensity();
    is.stddev_constraint_density = MP.getStddevConstraintDensity();
    is.equality_constraints_prop = MP.getEqualityConstraintProp();
    is.inequality_constraints_prop = MP.getInequalityConstraintProp();

    for (int con_idx = 0; con_idx < MP.getNumConstraints(); ++con_idx) {
        bool get_constraint_success_flag = false;
        Constraint con = MP.getConstraint(con_idx, get_constraint_success_flag);
        if (get_constraint_success_flag) {        
            double currrent_constraint_largest_ratio = con.getLargestRHSLHSRatio();
            if (currrent_constraint_largest_ratio > is.max_largest_ratio){
                is.max_largest_ratio = currrent_constraint_largest_ratio;
            }
            if (currrent_constraint_largest_ratio < is.min_largest_ratio){
                is.min_largest_ratio = currrent_constraint_largest_ratio;
            }

            double current_constraint_sum_RHS_ratio = con.getSumRHSLHSRatio();
        
            if (current_constraint_sum_RHS_ratio > is.max_sum_ratio){
                is.max_sum_ratio = current_constraint_sum_RHS_ratio;
            }
            if (current_constraint_sum_RHS_ratio < is.min_sum_ratio){
                is.min_sum_ratio = current_constraint_sum_RHS_ratio;
            }

            double current_constraint_sum_obj_coeff = MP.getConstraintSumObj(con_idx);
            
            if (current_constraint_sum_RHS_ratio > is.max_sum_ratio){
                is.max_sum_ratio = current_constraint_sum_RHS_ratio;
            }
            if (current_constraint_sum_RHS_ratio < is.min_sum_ratio){
                is.min_sum_ratio = current_constraint_sum_RHS_ratio;
            }
        }
    }
}

void MIPProblemProbe::populateNonruntimeDecompositionStatistics(const int& decomposition_idx, const individual_information_struct& decomp, Nonruntime_Relaxed_Constraint_Statistics& nrcs, MIP_Problem& MP)
{

    // based on the relaxed constraints vector
    // Largest subproblem size is normalised
    nrcs.constraints_relaxed_prop = (decomp.number_constraints_relaxed / double(MP.getNumConstraints())) / (MP.getNumVariables());
    nrcs.LSP_prop = double(decomp.largest_sp) / double(MP.getNumVariables());
    nrcs.inequality_constaints_relaxed_prop = 1.00 - nrcs.equality_constaints_relaxed_prop;

    // this group of statistics requires looping through each constraint in the con_vec to calculate the statistics.
    // To save time these statistical measures are calculated during the same loop.

    populateNonRuntimeAverages(decomp, MP, nrcs);

    // nrcs.equality_constaints_relaxed_prop = double(getNumberEqualityConstraintsRelaxed(decomp.con_vec, MP)) / double(decomp.number_constraints_relaxed);

    // put this as a parameter into the above function as well..

    // get average statistics before stddev
    //nrcs.average_nonzero_prop = getAverageNonZeroInRelaxedConstraints(decomp.con_vec, MP);
    //nrcs.average_largest_ratio = getAverageLargestRatioInRelaxedConstraints(decomp.con_vec, MP);

    // get stddev statistics
    // nrcs.stddev_nonzero_prop = getStddevNonZeroInRelaxedConstraints(decomp.con_vec, MP);

    //how to return constraint_type at idx.. if it exists...

    // check to see if

    // logic begind getting proportion of constraints which are equality..
    // loop through con vec and if == 1, get constraint type at idx...

    // int decomposition_idx;
    // vector<double>* con_vec_ptr;
    // double LSP_prop;
    // double num_constraints_relaxed_prop;
    // // proportion of relaxed constraints which are equality
    // double equality_constaints_relaxed_prop;
    // // proportion of relaxed constraints which are inequality = 1-equality_constaints_relaxed_prop
    // double inequality_constaints_relaxed_prop;
    // // non_zero prop in constraints relaxed
    // double average_nonzero_prop;
    // double stddev_nonzero_prop;
    // // ratio of RHS to largest coeff in constraint
    // double average_largest_ratio;
    // double stddev_largest_ratio;
}

// int MIPProblemProbe::getNumberEqualityConstraintsRelaxed(const vector<double>& con_vec, MIP_Problem& MP)
// {
//     int num_equality_constraints = 0;
//     for (int con_idx = 0; con_idx < con_vec.size(); ++con_idx) {
//         // account for potential rounding errors
//         if (int(con_vec[con_idx] + 0.1) == 1) {
//             bool success_flag = false;
//             bound_type data = MP.getConstraintType(con_idx, success_flag);
//             if (success_flag) {
//                 if (data == Equal) {
//                     ++num_equality_constraints;
//                 }
//             }
//         }
//     }
//     return num_equality_constraints;
// }

// double MIPProblemProbe::getAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP)
// {
//     if (calculuated_average_nonzeroes_flag == false) {
//         calculateAverageNonZeroInRelaxedConstraints(con_vec, MP);
//     }
//     // return the average prop of nonzeroes
//     return average_nonzero_in_relaxed_constraints;
// }

// double MIPProblemProbe::getStddevNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP)
// {
//     // stddev is sqrt(1/n sum(x_i - x_mean)^2)
//     double variance_inner_sum = 0;
//     if (calculuated_average_nonzeroes_flag == false) {
//         calculateAverageNonZeroInRelaxedConstraints(con_vec, MP);
//     }

//     for (int con_idx = 0; con_idx < con_vec.size(); ++con_idx) {
//         // account for potential rounding errors
//         if (int(con_vec[con_idx] + 0.1) == 1) {
//             bool success_flag = false;
//             int data = MP.getConstraintNumNonZeroes(con_idx, success_flag);
//             if (success_flag) {
//                 variance_inner_sum += pow(double((data / number_constraints_relaxed)) - average_nonzero_in_relaxed_constraints, 2);
//             }
//         }
//     }
//     // return the stddev prop of nonzeroes prop
//     return sqrt(variance_inner_sum / double(number_constraints_relaxed));
// }

// void MIPProblemProbe::calculateAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP)
// {
//     if (calculuated_average_nonzeroes_flag == false) {
//         int total_nonzero_count_in_relaxed_constraints = 0;
//         for (int con_idx = 0; con_idx < con_vec.size(); ++con_idx) {
//             // account for potential rounding errors
//             if (int(con_vec[con_idx] + 0.1) == 1) {
//                 bool success_flag = false;
//                 int data = MP.getConstraintNumNonZeroes(con_idx, success_flag);
//                 if (success_flag) {
//                     total_nonzero_count_in_relaxed_constraints += data;
//                 }
//             }
//         }
//         average_nonzero_in_relaxed_constraints = double(total_nonzero_count_in_relaxed_constraints) / double(MP.getNumVariables());
//         calculuated_average_nonzeroes_flag = true;
//     }
// }

// this group of statistics requires looping through each constraint in the con_vec to calculate the statistics.
// To save time these statistical measures are calculated during the same loop.
// these features are :
// average,min,max non_zeroes in relaxed constraints
// average,min,max of largest abs(RHS/LHS)
// average,min,max of sum of constraint coeffs abs(RHS/LHS)
// average,min,max, objective coefficient/constraint coefficient

void MIPProblemProbe::populateNonRuntimeAverages(const individual_information_struct& decomp, MIP_Problem& MP, Nonruntime_Relaxed_Constraint_Statistics& nrcs)
{
    int equality_constraint_count = 0;
    int total_nonzero_count_in_relaxed_constraints = 0;
    int total_bin_count_in_relaxed_constraints = 0;
    int total_int_count_in_relaxed_constraints = 0;
    int total_cont_count_in_relaxed_constraints = 0;
    int num_bin = 0;
    int num_int = 0;
    int num_cont = 0;
    double largest_ratio_sum = 0;
    double sum_RHS_ratio_sum = 0;
    double sum_obj_coeff_sum = 0;

    //get totals for non zeroes, constraint types and largest ratio
    for (int con_idx = 0; con_idx < decomp.con_vec.size(); ++con_idx) {
        // account for potential rounding errors
        if (int(decomp.con_vec[con_idx] + 0.1) == 1) {
            bool get_constraint_success_flag = false;
            Constraint con = MP.getConstraint(con_idx, get_constraint_success_flag);
            if (get_constraint_success_flag) {        
                int non_zeroes =  con.getNumVar();
                total_nonzero_count_in_relaxed_constraints += non_zeroes;
                
                // constraint type
                bound_type bound = con.getBoundType();
                if (bound == Equal) {
                    ++equality_constraint_count;
                }

                // proportion of variable types in the relaxed constraints
                for (const auto& variable_idx : con.getVarIndxs()){
                    Variable var = MP.getVariable(variable_idx);
                    if (var.getVarType() == Bin){
                        ++num_bin;
                    }
                    else if(var.getVarType() == Int){
                        ++num_int;
                    }
                    else if(var.getVarType() == Cont){
                        ++num_cont;
                    }
                }

                double currrent_constraint_largest_ratio = con.getLargestRHSLHSRatio();
                largest_ratio_sum += currrent_constraint_largest_ratio;
                if (currrent_constraint_largest_ratio > nrcs.max_largest_ratio){
                    nrcs.max_largest_ratio = currrent_constraint_largest_ratio;
                }
                if (currrent_constraint_largest_ratio < nrcs.min_largest_ratio){
                    nrcs.min_largest_ratio = currrent_constraint_largest_ratio;
                }
                
                double current_constraint_sum_RHS_ratio = con.getSumRHSLHSRatio();
                sum_RHS_ratio_sum += current_constraint_sum_RHS_ratio;
            
                if (current_constraint_sum_RHS_ratio > nrcs.max_sum_ratio){
                    nrcs.max_sum_ratio = current_constraint_sum_RHS_ratio;
                }
                if (current_constraint_sum_RHS_ratio < nrcs.min_sum_ratio){
                    nrcs.min_sum_ratio = current_constraint_sum_RHS_ratio;
                }
            
                double current_constraint_sum_obj_coeff = MP.getConstraintSumObj(con_idx);
                sum_obj_coeff_sum += current_constraint_sum_obj_coeff;

                if (current_constraint_sum_obj_coeff > nrcs.max_sum_obj){
                    nrcs.max_sum_obj = current_constraint_sum_obj_coeff;
                }
                if (current_constraint_sum_obj_coeff < nrcs.min_sum_obj){
                    nrcs.min_sum_obj = current_constraint_sum_obj_coeff;
                }
            }
        }
    }

    nrcs.average_nonzero_prop = (total_nonzero_count_in_relaxed_constraints / decomp.number_constraints_relaxed) / (MP.getNumVariables());
    
    nrcs.equality_constaints_relaxed_prop = double(equality_constraint_count) / double(decomp.number_constraints_relaxed);
    nrcs.inequality_constaints_relaxed_prop = 1.0 - nrcs.equality_constaints_relaxed_prop;
    
    nrcs.average_largest_ratio = largest_ratio_sum / decomp.number_constraints_relaxed;
    nrcs.average_sum_ratio = sum_RHS_ratio_sum / decomp.number_constraints_relaxed;
    nrcs.average_sum_obj = sum_obj_coeff_sum / decomp.number_constraints_relaxed;

    nrcs.bin_prop = num_bin / total_nonzero_count_in_relaxed_constraints;
    nrcs.int_prop = num_int / total_nonzero_count_in_relaxed_constraints;
    nrcs.cont_prop = num_cont / total_nonzero_count_in_relaxed_constraints;

}
