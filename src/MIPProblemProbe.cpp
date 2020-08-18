#include "MIPProblemProbe.h"
#include <math.h>

void MIPProblemProbe::populateInstanceStatistics(NSGA_ii_instance_statistics& nis, MIP_Problem& MP)
{
    nis.bin_prop = double(MP.getNumBin()) / double(MP.getNumVariables());
    nis.int_prop = double(MP.getNumInt()) / double(MP.getNumVariables());
    nis.cont_prop = double(MP.getNumCont()) / double(MP.getNumVariables());
    nis.average_constraint_density = MP.getAverageConstraintDensity();
    nis.stddev_constraint_density = MP.getStddevConstraintDensity();
    nis.equality_constraints_prop = MP.getEqualityConstraintProp();
    nis.inequality_constraints_prop = MP.getInequalityConstraintProp();
}

void MIPProblemProbe::populateRelaxedConstraintsStatistics(const int& decomposition_idx, const individual_information_struct& decomp, NSGA_ii_relaxed_constraint_statistics& nrcs, MIP_Problem& MP)
{
    number_constraints_relaxed = decomp.number_constraints_relaxed;
    // based on the relaxed constraints vector
    nrcs.LSP_prop = double(decomp.largest_sp) / double(MP.getNumVariables());
    nrcs.num_constraints_relaxed_prop = double(decomp.number_constraints_relaxed) / double(MP.getNumConstraints());
    

    // this group of statistics requires looping through each constraint in the con_vec to calculate the statistics.
    // To save time these statistical measures are calculated during the same loop.
    populateStatisticsRequiringIteration(decomp,  MP, nrcs.equality_constaints_relaxed_prop 
    , nrcs.average_nonzero_prop, nrcs.average_largest_ratio);

    // nrcs.equality_constaints_relaxed_prop = double(getNumberEqualityConstraintsRelaxed(decomp.con_vec, MP)) / double(decomp.number_constraints_relaxed);
    
    // put this as a parameter into the above function as well..
    nrcs.inequality_constaints_relaxed_prop = 1.00 - nrcs.equality_constaints_relaxed_prop;

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
//         int total_nonzero_count = 0;
//         for (int con_idx = 0; con_idx < con_vec.size(); ++con_idx) {
//             // account for potential rounding errors
//             if (int(con_vec[con_idx] + 0.1) == 1) {
//                 bool success_flag = false;
//                 int data = MP.getConstraintNumNonZeroes(con_idx, success_flag);
//                 if (success_flag) {
//                     total_nonzero_count += data;
//                 }
//             }
//         }
//         average_nonzero_in_relaxed_constraints = double(total_nonzero_count) / double(MP.getNumVariables());
//         calculuated_average_nonzeroes_flag = true;
//     }
// }

void MIPProblemProbe::populateStatisticsRequiringIteration(const individual_information_struct& decomp, MIP_Problem& MP, double& equality_constaints_relaxed_prop
    , double& average_nonzero_prop, double& average_largest_ratio_prop)
{

    int equality_constraint_count = 0;
    int total_nonzero_count = 0;
    double largest_ratio_sum = 0;

    for (int con_idx = 0; con_idx < decomp.con_vec.size(); ++con_idx) {
        // account for potential rounding errors
        if (int(decomp.con_vec[con_idx] + 0.1) == 1) {
            bool success_flag_non_zeroes = false;
            int non_zeroes = MP.getConstraintNumNonZeroes(con_idx, success_flag_non_zeroes);
            if (success_flag_non_zeroes) {
                total_nonzero_count += non_zeroes;
            }
            bool success_flag_bound_type = false;
            bound_type bound = MP.getConstraintType(con_idx, success_flag_bound_type);
            if (success_flag_bound_type) {
                if (bound == Equal) {
                    ++equality_constraint_count;
                }
            }
            bool success_flag_ratio = false;
            double largest_ratio = MP.getConstraintLargestRatio(con_idx, success_flag_ratio);
            if (success_flag_ratio){
                largest_ratio_sum += largest_ratio;
            }

        }
    }

    //double(getNumberEqualityConstraintsRelaxed(decomp.con_vec, MP)) / double(decomp.number_constraints_relaxed);
    equality_constaints_relaxed_prop = double(equality_constraint_count) / double(decomp.number_constraints_relaxed);
    average_nonzero_prop = double(total_nonzero_count) / double(MP.getNumVariables());


}
