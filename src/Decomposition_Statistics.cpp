#include "Decomposition_Statistics.h"
#include "MIPProblemProbe.h"

using Decomposition_Statistics::Subproblems;
using Decomposition_Statistics::RelaxedConstraints;
using Decomposition_Statistics::Instance;


// because the order of subproblems is changed for solving purposes, size the neccesary vectors beforehand
// so they cant be accessed via the subproblems index
void Subproblems::resize(const int& number_subproblems){
    mip_times.resize(number_subproblems);
    lp_times.resize(number_subproblems);
    mip_obj_solutions.resize(number_subproblems);
    lp_obj_solutions.resize(number_subproblems);
    subproblem_optimality_success.resize(number_subproblems);
    subproblem_lp_found.resize(number_subproblems);
    subproblem_attempted.resize(number_subproblems, false);
}


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
    
    // structure used to contain subproblem variable statistics
    SubproblemVariableStatistics svs = {};
    MPP.getSubproblemVariableStatistics(svs,ps.node_idxs);
    // sum of obj coefficients of variables in each block
    sum_block_obj_values.push_back(svs.sum_block_obj_val);
    // sum of abs(obj) coefficients of variables in each block
    sum_abs_block_obj_values.push_back(svs.sum_abs_block_obj_val);
    // // range of obj vals in blocks
    block_obj_val_ranges.push_back(svs.obj_val_range);

    if (number_of_constraints_in_subproblem != 0){
        // structure used to contain subproblem constraint statistics
        SubproblemConstraintStatistics scs = {};
        // gather the neccesary statistics for constraints contained within the subproblem
        MPP.getSubproblemConstraintStatistics(scs,ps.edge_idxs);
        equality_props.push_back(scs.equality_prop);
        average_block_RHS_values.push_back(scs.average_RHS_val);
        average_block_absRHS_values.push_back(scs.average_abs_RHS_val);
        average_block_Largest_RHSLHS_ratio.push_back(scs.average_largest_RHSLHS_ratio);
        average_block_shape.push_back(static_cast<double>(number_of_variables_in_subproblem) / static_cast<double>(number_of_constraints_in_subproblem));
        block_RHS_range.push_back(scs.largest_RHS_range);
        // Block densities (no. non_zeroes / (no. cons * no. var)
        block_densities.push_back(
                static_cast<double>(scs.num_non_zeroes) / (static_cast<double>(number_of_variables_in_subproblem) * static_cast<double>(number_of_constraints_in_subproblem)));
        non_zeroes_count.push_back(scs.num_non_zeroes);
    }
}


// go through and replace averages with the raw data
void RelaxedConstraints::generate_statistics(MIPProblemProbe& MPP, const vector<int>& relaxed_constraint_indices){

    // proportion of relaxed constraints out of total constraints
    relaxed_constraint_prop = static_cast<double>(relaxed_constraint_indices.size()) / MPP.getNumMIPConst();
    RelaxedConstraintStatistics rcs = {};
    MPP.getRelaxedConstraintStatistics(rcs,relaxed_constraint_indices);
    // get block equality/inequality constraint props
    equality_prop = rcs.equality_const_prop;
    // var props in the relaxed constraints
    bin_props = get<0>(rcs.relaxed_const_var_props);
    int_props = get<1>(rcs.relaxed_const_var_props);
    cont_props = get<2>(rcs.relaxed_const_var_props);
    // non zero props of relaxed constraints
    non_zero_counts = rcs.constraint_non_zeroes_counts;
    // Largest abs(RHS/LHS) ratio
    largest_RHSLHS_ratios = rcs.largest_RHSLHS_ratios;
    // sum obj coefficients
    sum_obj_coeffs_of_constraints = rcs.sum_obj_coeffs_of_constraints;
    // sum abs(obj) coefficients
    sum_abs_obj_coeffs_of_constraints = rcs.sum_abs_obj_coeffs_of_constraints;
    // RHS values
    RHS_values = rcs.RHS_values;
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

void Instance::getRHSLHSExtremes(MIPProblemProbe& MPP){
    //min max pair
    std::pair<double,double> rhslhs_minmax = MPP.getRHSLHSExtremes();
    min_rhslhs = rhslhs_minmax.first;
    max_rhslhs = rhslhs_minmax.second;
}
 
void Instance::getSumObjExtremes(MIPProblemProbe& MPP){
    //min max pair
    std::pair<double,double> sum_obj_minmax = MPP.getSumObjExtremes();
    min_sum_obj = sum_obj_minmax.first;
    max_sum_obj = sum_obj_minmax.second;
}

void Instance::getSumAbsObjExtremes(MIPProblemProbe& MPP){
    //min max pair
    std::pair<double,double> sum_abs_obj_minmax = MPP.getSumAbsObjExtremes();
    min_sum_abs_obj = sum_abs_obj_minmax.first;
    max_sum_abs_obj = sum_abs_obj_minmax.second;
}

// generate instance statistics
 void Instance::populateInstanceStatistics(MIPProblemProbe& MPP){
    
    getObjExtremes(MPP);
    getRHSExtremes(MPP);
    getRHSLHSExtremes(MPP);
    getSumObjExtremes(MPP);
    getSumAbsObjExtremes(MPP);
    num_var = MPP.getNumMIPVar();
    num_const = MPP.getNumMIPConst();
    num_bin = MPP.getNumMIPBin();
    num_cont = MPP.getNumMIPCont();
    num_int = MPP.getNumMIPInt();
    num_non_zeroes = MPP.getNumMIPNonZero();
    num_equality_constr = MPP.getInstanceEquality();
    MPP.getInstanceConstraintRawValues(ricv);
    MPP.getInstanceObjTerms(obj_val_terms);

}