#include "Constraint.h"
#include <algorithm>

void Constraint::addVarIdx(const int& var_idx_to_add){

     // if variable is already contained within the constraint, don't add it
    if(std::find(variable_indxs.begin(), variable_indxs.end(), var_idx_to_add) == variable_indxs.end()) {
        variable_indxs.push_back(var_idx_to_add);
    }
   
};


double Constraint::getLargestRHSLHSRatio()
{
    double largest_ratio = 0.00;
    for (auto& var_coeff : variable_coeffs) {
        if (std::abs(RHS / var_coeff) > largest_ratio) {
            largest_ratio = std::abs(RHS / var_coeff);
        }
    }
    return largest_ratio;
}

double Constraint::getSumRHSLHSRatio()
{
    double sum_RHS_ratio = 0.00;
    for (auto& var_coeff : variable_coeffs) {
        sum_RHS_ratio += std::abs(RHS / var_coeff);
    }
    return sum_RHS_ratio;
}

