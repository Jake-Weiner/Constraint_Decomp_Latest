#include "Decomposition_Statistics.h"


using Decomposition_Statistics::Subproblems;
using Decomposition_Statistics::RelaxedConstraints;
using Decomposition_Statistics::Instance;


void Subproblems::generateBlockStatistics(const Partition_Struct& ps, MIPProblemProbe& MPP){
    int number_of_variables_in_subproblem = ps.getNumNodes();
    int number_of_constraints_in_subproblem = ps.getNumEdges();

    // subproblem sizes as a proportion of original problem
    block_variable_props.push_back(static_cast<double>(number_of_variables_in_subproblem) / static_cast<double>(MPP.getNumMIPVar()));

    // proportion of variables in each partition which are bin, int and cont
    tuple<int, int, int> variable_counts = MPP.getVariableCounts(ps.node_idxs);
    bin_props.push_back(static_cast<double>(get<0>(variable_counts)) / static_cast<double>(number_of_variables_in_subproblem));
    int_props.push_back(static_cast<double>(get<1>(variable_counts)) / static_cast<double>(number_of_variables_in_subproblem));
    cont_props.push_back(static_cast<double>(get<2>(variable_counts)) / static_cast<double>(number_of_variables_in_subproblem));

    //constraint statistics
    total_constr_props.push_back(static_cast<double>(number_of_constraints_in_subproblem) / static_cast<double>(MPP.getNumMIPConst()));

    // get block equality/inequality constraint props
    double equality_const_prop = MPP.getEqualityConstraintProp(ps.edge_idxs);
    equality_props.push_back(equality_const_prop);
    
    // sum of obj coefficients of variables in each block
    double sum_block_obj_val = MPP.getBlockSumObjs(ps.node_idxs, false);
    sum_block_obj_values.push_back(sum_block_obj_val);

    // sum of abs(obj) coefficients of variables in each block
    double sum_abs_block_obj_val = MPP.getBlockSumObjs(ps.node_idxs, true);
    sum_abs_block_obj_values.push_back(sum_abs_block_obj_val);

    // range of obj vals in blocks
    double obj_val_range = MPP.getBlockObjRange(ps.node_idxs);
    block_obj_val_ranges.push_back(obj_val_range);

    if (number_of_constraints_in_subproblem != 0){
        // averages of rhs coefficients in each block
        average_block_RHS_values.push_back(MPP.getAverageBlockRHS(ps.edge_idxs, false));
        average_block_absRHS_values.push_back(MPP.getAverageBlockRHS(ps.edge_idxs, true));
        // averages of abs(Largest RHS/LHS ratio) coefficients in each block
        average_block_Largest_RHSLHS_ratio.push_back(MPP.getAverageBlockLargestRHSLHSRatio(ps.edge_idxs));
        // averages of block shapes
        average_block_shape.push_back(static_cast<double>(number_of_variables_in_subproblem) / static_cast<double>(number_of_constraints_in_subproblem));
        // Block RHS ranges
        block_RHS_range.push_back(MPP.getBlockLargestRHSRange(ps.edge_idxs));
        // Block densities (no. non_zeroes / (no. cons * no. var)
        block_densities.push_back(
                static_cast<double>(MPP.getBlockNonZeroes(ps.edge_idxs)) / static_cast<double>(number_of_variables_in_subproblem * number_of_constraints_in_subproblem));
    }
}


// go through and replace averages wioth the raw data
void RelaxedConstraints::generate_statistics(MIPProblemProbe& MPP, const vector<int>& relaxed_constraint_indices){

    // proportion of relaxed constraints out of total constraints
    relaxed_constraint_prop = static_cast<double>(relaxed_constraint_indices.size()) / MPP.getNumMIPConst();

    // get block equality/inequality constraint props
    equality_prop = MPP.getEqualityConstraintProp(relaxed_constraint_indices);
   
    // var props in the relaxed constraints
    tuple<vector<double>, vector<double>, vector<double>> relaxed_const_var_props = MPP.getVariableProps(relaxed_constraint_indices);
    bin_props = get<0>(relaxed_const_var_props);
    int_props = get<1>(relaxed_const_var_props);
    cont_props = get<2>(relaxed_const_var_props);

    // non zero props of relaxed constraints
    non_zero_counts = MPP.getConstraintNonZeroCounts(relaxed_constraint_indices);
    // tuple<double, double, double, double> relaxed_const_nonzero_stats = getStatistics(non_zero_props);
    // average_non_zero_prop = get<2>(relaxed_const_nonzero_stats);
    // stddev_non_zero_prop = get<3>(relaxed_const_nonzero_stats);

    // Largest abs(RHS/LHS) ratio
    largest_RHSLHS_ratios = MPP.getLargestRHSLHSRatios(relaxed_constraint_indices);
    // tuple<double, double, double, double> relaxed_const_LargestRHSLHS_stats = getStatistics(largest_RHSLHS_ratios);
    // average_RHSLHS_ratio = get<2>(relaxed_const_LargestRHSLHS_stats);
    // stddev_RHSLHS_ratio = get<3>(relaxed_const_LargestRHSLHS_stats);

    // sum obj coefficients
    sum_obj_coeffs_of_constraints = MPP.getConstraintSumObjs(relaxed_constraint_indices);
    // tuple<double, double, double, double> relaxed_const_SumObj_stats = getStatistics(sum_obj_coeffs_of_constraints);
    // average_sum_obj_coeffs = get<2>(relaxed_const_SumObj_stats);

    // sum abs(obj) coefficients
    sum_abs_obj_coeffs_of_constraints = MPP.getConstraintSumAbsObjs(relaxed_constraint_indices);
    // tuple<double, double, double, double> relaxed_const_SumAbsObj_stats = getStatistics(sum_abs_obj_coeffs_of_constraints);
    // average_abs_sum_obj_coeffs = get<2>(relaxed_const_SumAbsObj_stats);
    // stddev_abs_sum_obj_coeffs = get<3>(relaxed_const_SumAbsObj_stats);

    // RHS values
    RHS_values = MPP.getConstraintRHSVals(relaxed_constraint_indices);
    // tuple<double, double, double, double> relaxed_const_RHS_stats = getStatistics(RHS_values);
    // average_RHS = get<2>(relaxed_const_RHS_stats);
    // stddev_RHS = get<3>(relaxed_const_RHS_stats);

}

void Instance::getObjExtremes(MIPProblemProbe& MPP){

    //min max pair

    std::pair<double,double> obj_minmax = MPP.getObjExtremes();
    min_obj = obj_minmax.first;
    max_obj = obj_minmax.second;
    
        

}

void Instance::getRHSExtremes(MIPProblemProbe& MPP){

    //min max pair
    std::pair<double,double> rhs_minmax = MPP.getRHSExtremes();
    min_rhs = rhs_minmax.first;
    max_rhs = rhs_minmax.second;
}


// generate instance statistics
 void Instance::populateInstanceStatistics(MIPProblemProbe& MPP){

    getObjExtremes(MPP);
    getRHSExtremes(MPP);

    num_var = MPP.getNumMIPVar();
    num_const = MPP.getNumMIPConst();
    num_bin = MPP.getNumMIPBin();
    num_cont = MPP.getNumMIPCont();
    num_int = MPP.getNumMIPInt();
    num_non_zeroes = MPP.getNumMIPNonZero();
    

 }