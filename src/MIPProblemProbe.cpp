#include "MIPProblemProbe.h"
#include "Decomposition_Statistics.h"
#include <math.h>
#include <unordered_map>

MIPProblemProbe::MIPProblemProbe(MIP_Problem* MP_ptr)
    : MP_ptr(MP_ptr)
{
}

std::tuple<int, int, int> MIPProblemProbe::getVariableCounts(const std::vector<int>& variable_indexes)
{
    int bin_count = 0;
    int int_count = 0;
    int cont_count = 0;
    for (const auto& variable_idx : variable_indexes) {
        Variable v = MP_ptr->getVariable(variable_idx);
        if (v.getVarType() == Bin) {
            ++bin_count;
        } else if (v.getVarType() == Int) {
            ++int_count;
        } else {
            ++cont_count;
        }
    }
    return std::make_tuple(bin_count, int_count, cont_count);
}

std::vector<double> MIPProblemProbe::getLargestRHSLHSRatios(const std::vector<int>& constraint_idxs)
{
    std::vector<double> ret_vec;
    for (const auto& constraint_idx : constraint_idxs) {
        Constraint con = MP_ptr->getConstraint(constraint_idx);
        ret_vec.push_back(con.getLargestRHSLHSRatio());
    }
    return ret_vec;
}

std::vector<double> MIPProblemProbe::getSumRHSLHSRatios(const std::vector<int>& constraint_idxs)
{
    std::vector<double> ret_vec;
    for (const auto& constraint_idx : constraint_idxs) {
        Constraint con = MP_ptr->getConstraint(constraint_idx);
        ret_vec.push_back(con.getSumRHSLHSRatio());
    }
    return ret_vec;
}

std::vector<double> MIPProblemProbe::getConstraintSumObjs(const std::vector<int>& constraint_idxs)
{
    std::vector<double> ret_vec;
    for (const auto& constraint_idx : constraint_idxs) {
        ret_vec.push_back(MP_ptr->getConstraintSumObj(constraint_idx, false));
    }
    return ret_vec;
}

// return a vector containing the RHS values of the constraint indexes supplied
std::vector<double> MIPProblemProbe::getConstraintRHSVals(const std::vector<int>& constraint_idxs){

    std::vector<double> ret_vec;
    for (const auto& constraint_idx : constraint_idxs) {
        ret_vec.push_back(MP_ptr->getConstraint(constraint_idx).getRHS());
    }
    return ret_vec;
}

std::vector<double> MIPProblemProbe::getConstraintSumAbsObjs(const std::vector<int>& constraint_idxs)
{
    std::vector<double> ret_vec;
    for (const auto& constraint_idx : constraint_idxs) {
        ret_vec.push_back(MP_ptr->getConstraintSumObj(constraint_idx, true));
    }
    return ret_vec;
}


// abs if bool abs is true, else  just sum
// returns the average of the blocks RHS for the constraints the block contains
double MIPProblemProbe::getAverageBlockRHS(const std::vector<int>& constraint_idxs, const bool& abs_flag)
{
    double sum = 0;
    for (const auto& constraint_idx : constraint_idxs) {
        if (abs_flag){
            sum += abs(MP_ptr->getConstraint(constraint_idx).getRHS());
        }
        else{
            sum += MP_ptr->getConstraint(constraint_idx).getRHS();
        }
       
    }
    return (sum / static_cast<double>(constraint_idxs.size()));
}

// returns the average of the blocks abs(Largest RHS/LHS) for the constraints the block contains
double MIPProblemProbe::getAverageBlockLargestRHSLHSRatio(const std::vector<int>& constraint_idxs)
{

    double sum = 0;
    for (const auto& constraint_idx : constraint_idxs) {
        sum += abs(MP_ptr->getConstraint(constraint_idx).getLargestRHSLHSRatio());
    }
    return (sum / constraint_idxs.size());
}

double MIPProblemProbe::getBlockLargestRHSRange(const std::vector<int>& constraint_idxs)
{

    double max_rhs = std::numeric_limits<double>::min();
    double min_rhs = std::numeric_limits<double>::max();
    for (const auto& constraint_idx : constraint_idxs) {
        if (MP_ptr->getConstraint(constraint_idx).getRHS() > max_rhs) {
            max_rhs = MP_ptr->getConstraint(constraint_idx).getRHS();
        }
        if (MP_ptr->getConstraint(constraint_idx).getRHS() < min_rhs) {
            min_rhs = MP_ptr->getConstraint(constraint_idx).getRHS();
        }
    }
    // cout << "max rhs: " << max_rhs << " min rhs: " << min_rhs << endl;
    return (max_rhs - min_rhs);
}

// get the non_zero count for the constraint indices provided
int MIPProblemProbe::getBlockNonZeroes(const std::vector<int>& constraint_idxs)
{
    int non_zero_count = 0;
    for (const auto& constraint_idx : constraint_idxs) {
        non_zero_count += MP_ptr->getConstraint(constraint_idx).getNumVar();
    }
    return non_zero_count;
}



// get the proportion of constraints provided which are equality
double MIPProblemProbe::getEqualityConstraintProp(const std::vector<int>& constraint_idxs)
{

    int equality_const_count = 0;
    for (const auto& constraint_idx : constraint_idxs) {
        if (MP_ptr->constraintIndexValidity(constraint_idx)) {
            if (MP_ptr->constraints[constraint_idx].getBoundType() == Equal) {
                ++equality_const_count;
            }
        } else {
            cout << "Invalid Constraint Index in getBlockEqualityConstraintProp(). Index requested is: " << constraint_idx << endl;
            exit(EXIT_FAILURE);
        }
    }
    return (static_cast<double>(equality_const_count) / static_cast<double>(constraint_idxs.size()));
}

// returns bin,int and cont props of supplied constraints
std::tuple<vector<double>, vector<double>, vector<double>> MIPProblemProbe::getVariableProps(const std::vector<int>& constraint_idxs)
{
    vector<double> bin_props;
    vector<double> int_props;
    vector<double> cont_props;
    
    // need to keep count of bin, int and cont vars as well as total number of variables seen so far
    for (const auto& con_idx : constraint_idxs) {
        std::tuple<double, double, double> con_var_props = getVariablePropsConstraint(con_idx);
        bin_props.push_back(get<0>(con_var_props));
        int_props.push_back(get<1>(con_var_props));
        cont_props.push_back(get<2>(con_var_props));
    }
    return std::make_tuple(bin_props,int_props,cont_props);
}

std::tuple<double, double, double> MIPProblemProbe::getVariablePropsConstraint(const int& constraint_idx){

    //total variable count in the constraint idx's provided
    int var_count = MP_ptr->getConstraint(constraint_idx).getNumVar();
    int bin_count = 0;
    int int_count = 0;
    int cont_count = 0;

    // need to keep count of bin, int and cont vars as well as total number of variables seen so far
  
    if (MP_ptr->constraintIndexValidity(constraint_idx)) {
        vector<int> variable_idxs = MP_ptr->getConstraint(constraint_idx).getVarIndxs();
        for (const auto& var_idx : variable_idxs) {
            Variable v = MP_ptr->getVariable(var_idx);
            if (v.getVarType() == Bin) {
                ++bin_count;
            } else if (v.getVarType() == Int) {
                ++int_count;
            } else {
                ++cont_count;
            }
        }
    }
    

    return std::make_tuple(static_cast<double>(bin_count)/ static_cast<double>(var_count)
    , static_cast<double>(int_count)/ static_cast<double>(var_count)
    , static_cast<double>(cont_count)/ static_cast<double>(var_count));


}

// for the input constraint indices, create a hashmap for
int MIPProblemProbe::getVarCount(const std::vector<int>& constraint_idxs)
{
    int var_count = 0;
    std::unordered_map<int, bool> variable_index_hashmap;
    // for every new variable, add to the hashmap to see if it's been seen previously or not
    for (const auto& con_idx : constraint_idxs) {
        if (MP_ptr->constraintIndexValidity(con_idx)) {
            vector<int> variable_idxs = MP_ptr->getConstraint(con_idx).getVarIndxs();
            for (const auto& var_idx : variable_idxs) {
                if (variable_index_hashmap.find(var_idx) == variable_index_hashmap.end()) {
                    variable_index_hashmap[var_idx] = true;
                    ++var_count;
                }
            }
        }
    }
    return var_count;
}

// returns the vector of non zero props contained within the constriants supplied
std::vector<int> MIPProblemProbe::getConstraintNonZeroCounts(const std::vector<int>& constraint_idxs){
    vector<int> constraint_non_zeroes_counts;
    for (const auto& con_idx : constraint_idxs){
        if (MP_ptr->constraintIndexValidity(con_idx)) {
            int const_num_of_non_zeroes = MP_ptr->getConstraint(con_idx).getNumVar();
            constraint_non_zeroes_counts.push_back(const_num_of_non_zeroes);
        }
    }
    return constraint_non_zeroes_counts;
}

void MIPProblemProbe::getSubproblemVariableStatistics(SubproblemVariableStatistics& svs, const std::vector<int>& variable_idxs){

    double sum_obj = 0.00;
    double sum_abs_obj = 0.00;
    double max = -std::numeric_limits<double>::max();
    double min = std::numeric_limits<double>::max();
    for (const auto& var_idx : variable_idxs) {
        if (MP_ptr->variableIndexValidity(var_idx)) {
            bool var_has_obj_coeff = false;
            double obj_coeff = MP_ptr->getVarObjCoeff(var_idx, var_has_obj_coeff);
            if (var_has_obj_coeff) {
                sum_abs_obj += abs(obj_coeff);
                sum_obj += obj_coeff;
                if(obj_coeff < min){
                   min = obj_coeff;
                }
                if(obj_coeff > max){
                   max = obj_coeff;
                }
            }
        }
    }

    svs.sum_block_obj_val = sum_obj;
    svs.sum_abs_block_obj_val = sum_abs_obj;
    svs.obj_val_range = max - min;

}

void MIPProblemProbe::getSubproblemConstraintStatistics(SubproblemConstraintStatistics& scs, const std::vector<int>& constraint_idxs){
    
    int equality_const_count = 0;
    double RHS_sum = 0.00;
    double abs_RHS_sum = 0.00;
    double largest_RHSLHS_ratio_sum = 0.00;
    double max_rhs = std::numeric_limits<double>::min();
    double min_rhs = std::numeric_limits<double>::max();
    int non_zero_count = 0;
    for (const auto& constraint_idx : constraint_idxs) {
        if (MP_ptr->constraintIndexValidity(constraint_idx)) {
            if (MP_ptr->constraints[constraint_idx].getBoundType() == Equal) {
                ++equality_const_count;
            }
            abs_RHS_sum += abs(MP_ptr->getConstraint(constraint_idx).getRHS());
            RHS_sum += MP_ptr->getConstraint(constraint_idx).getRHS();
            largest_RHSLHS_ratio_sum += abs(MP_ptr->getConstraint(constraint_idx).getLargestRHSLHSRatio());
            if (MP_ptr->getConstraint(constraint_idx).getRHS() > max_rhs) {
                max_rhs = MP_ptr->getConstraint(constraint_idx).getRHS();
            }
            if (MP_ptr->getConstraint(constraint_idx).getRHS() < min_rhs) {
                min_rhs = MP_ptr->getConstraint(constraint_idx).getRHS();
            }
            non_zero_count += MP_ptr->getConstraint(constraint_idx).getNumVar();
        } else {
            cout << "Invalid Constraint Index in getBlockEqualityConstraintProp(). Index requested is: " << constraint_idx << endl;
            exit(EXIT_FAILURE);
        }
    }
 
    scs.equality_prop = (static_cast<double>(equality_const_count) / static_cast<double>(constraint_idxs.size()));
    scs.average_RHS_val = RHS_sum / static_cast<double>(constraint_idxs.size());
    scs.average_abs_RHS_val = abs_RHS_sum / static_cast<double>(constraint_idxs.size());
    scs.average_largest_RHSLHS_ratio = largest_RHSLHS_ratio_sum / static_cast<double>(constraint_idxs.size());
    scs.largest_RHS_range = max_rhs - min_rhs;
    scs.num_non_zeroes = non_zero_count;

}

void MIPProblemProbe::getRelaxedConstraintStatistics(RelaxedConstraintStatistics& rcs, const std::vector<int>& constraint_idxs){

    //   // get block equality/inequality constraint props
    // equality_prop = MPP.getEqualityConstraintProp(relaxed_constraint_indices

    int equality_const_count = 0;
    vector<double> bin_props;
    vector<double> int_props;
    vector<double> cont_props;

    for (const auto& constraint_idx : constraint_idxs) {
        if (MP_ptr->constraintIndexValidity(constraint_idx)) {
            Constraint con = MP_ptr->getConstraint(constraint_idx);

            if (con.getBoundType() == Equal) {
                ++equality_const_count;
            }
            
            // var props of relaxed constraints
            std::tuple<double, double, double> con_var_props = getVariablePropsConstraint(constraint_idx);
            bin_props.push_back(get<0>(con_var_props));
            int_props.push_back(get<1>(con_var_props));
            cont_props.push_back(get<2>(con_var_props));

            rcs.constraint_non_zeroes_counts.push_back(con.getNumVar()); 
            rcs.largest_RHSLHS_ratios.push_back(con.getLargestRHSLHSRatio());
            rcs.sum_obj_coeffs_of_constraints.push_back(MP_ptr->getConstraintSumObj(constraint_idx, false));
            rcs.sum_abs_obj_coeffs_of_constraints.push_back(MP_ptr->getConstraintSumObj(constraint_idx, true));
            rcs.RHS_values.push_back(con.getRHS());
            
        } else {
            cout << "Invalid Constraint Index in getBlockEqualityConstraintProp(). Index requested is: " << constraint_idx << endl;
            exit(EXIT_FAILURE);
        }
    }

    rcs.equality_const_prop = static_cast<double>(equality_const_count) / static_cast<double>(constraint_idxs.size());
    rcs.relaxed_const_var_props = std::make_tuple(bin_props,int_props,cont_props);
    
}

double MIPProblemProbe::getBlockSumObjs(const std::vector<int>& variable_idxs, const bool& abs_flag)
{
    double sum_obj = 0;
    for (const auto& var_idx : variable_idxs) {
        if (MP_ptr->variableIndexValidity(var_idx)) {
            bool var_has_obj_coeff = false;
            double obj_coeff = MP_ptr->getVarObjCoeff(var_idx, var_has_obj_coeff);
            if (var_has_obj_coeff) {
                if (abs_flag){
                    sum_obj += abs(obj_coeff);
                }else{
                    sum_obj += obj_coeff;
                }
            }
        }
    }
    return sum_obj;
}


int MIPProblemProbe::getNumMIPConst(){

    int ret_val = -1;
    if (MP_ptr  != nullptr){
        ret_val =  MP_ptr->getNumConstraints();
    }
    return ret_val;
}

// return min/max
std::pair<double,double> MIPProblemProbe::getObjExtremes(){

    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (const auto& obj_pair : MP_ptr->objective_fn){
        
        if (obj_pair.second < min){
            min = obj_pair.second;
        }
        if(obj_pair.second > max)
            max = obj_pair.second;
    }
    return std::make_pair(min,max);
}

// return min/max
std::pair<double,double> MIPProblemProbe::getRHSExtremes(){
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (auto& constraint : MP_ptr->constraints){
        
        double con_rhs = constraint.getRHS();
        if (con_rhs < min){
            min = con_rhs;
        }
        if(con_rhs > max)
            max = con_rhs;
    }
    return std::make_pair(min,max);
}

// return min/max
std::pair<double,double> MIPProblemProbe::getRHSLHSExtremes(){
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (auto& constraint : MP_ptr->constraints){
        double con_rhslhs = constraint.getLargestRHSLHSRatio();
        if (con_rhslhs < min){
            min = con_rhslhs;
        }
        if(con_rhslhs > max)
            max = con_rhslhs;
    }
    return std::make_pair(min,max);
}

// return min/max
std::pair<double,double> MIPProblemProbe::getSumObjExtremes(){
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (auto& constraint : MP_ptr->constraints){
        double con_sumobj =  MP_ptr->getConstraintSumObj(constraint.getConIdx(), false);
        if (con_sumobj < min){
            min = con_sumobj;
        }
        if(con_sumobj > max)
            max = con_sumobj;
    }
    return std::make_pair(min,max);
}

// return min/max
std::pair<double,double> MIPProblemProbe::getSumAbsObjExtremes(){
    double min = std::numeric_limits<double>::max();
    double max = -std::numeric_limits<double>::max();
    for (auto& constraint : MP_ptr->constraints){
        double con_sumobj =  MP_ptr->getConstraintSumObj(constraint.getConIdx(), true);
        if (con_sumobj < min){
            min = con_sumobj;
        }
        if(con_sumobj > max)
            max = con_sumobj;
    }
    return std::make_pair(min,max);
}



int MIPProblemProbe::getNumMIPVar(){

    int ret_val = -1;
    if (MP_ptr != nullptr){
        ret_val = MP_ptr->getNumVariables();
    }
    return ret_val;
}

int MIPProblemProbe::getNumMIPBin(){

    int ret_val = -1;
    if (MP_ptr != nullptr){
        ret_val = MP_ptr->getNumBin();
    }
    return ret_val;
}
int MIPProblemProbe::getNumMIPCont(){

    int ret_val = -1;
    if (MP_ptr != nullptr){
        ret_val = MP_ptr->getNumCont();
    }
    return ret_val;


}
int MIPProblemProbe::getNumMIPInt(){

    int ret_val = -1;
    if (MP_ptr != nullptr){
        ret_val = MP_ptr->getNumInt();
    }
    return ret_val;

}
int MIPProblemProbe::getNumMIPNonZero(){

    int ret_val = -1;
    if (MP_ptr != nullptr){
        ret_val = MP_ptr->getnumNonZero();
    }
    return ret_val;
}

 double MIPProblemProbe::getBlockObjRange(const std::vector<int>& variable_indexes){

    double max = -std::numeric_limits<double>::max();
    double min = std::numeric_limits<double>::max();
    for (const auto& var_idx : variable_indexes) {
        if (MP_ptr->variableIndexValidity(var_idx)) {
            bool var_has_obj_coeff = false;
            double obj_coeff = MP_ptr->getVarObjCoeff(var_idx, var_has_obj_coeff);
            if (var_has_obj_coeff) {
               if(obj_coeff < min){
                   min = obj_coeff;
               }
               if(obj_coeff > max){
                   max = obj_coeff;
               }
            }
        }
    }
    return (max - min);
}


 vector<int> MIPProblemProbe::getConstraintsContainingVar(const int& var_idx){

     vector<int> constraint_idxs;
    for (auto& con : MP_ptr->constraints){
        if (con.containsVar(var_idx)){
            constraint_idxs.push_back(con.getConIdx());
        }
    }
    return constraint_idxs;

 }