#ifndef __CONSTRAINT__
#define __CONSTRAINT__

#include <cmath>
#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

enum bound_type { Equal,
    Less,
    Greater };

class Constraint {

public:
    Constraint(){};
    Constraint(int constraint_idx)
    {
        this->constraint_idx = constraint_idx;
    };
    ~Constraint(){};
    void addVarIdx(const int& var_idx_to_add)
    {
        for (auto& var_idx : variable_indxs) {
            if (var_idx == var_idx_to_add) {
                return;
            }
        }
        variable_indxs.push_back(var_idx_to_add);
    };
    void addVarCoeff(const int& var_coeff_to_add)
    {
        variable_coeffs.push_back(var_coeff_to_add);
    }

    void addConTerm(const pair<int, double> term)
    {
        constraint_terms.push_back(term);
    }

    vector<int> getVarIndxs()
    {
        return variable_indxs;
    }

    vector<double> getVarCoeffs()
    {
        return variable_coeffs;
    }

    int getConIdx()
    {
        return constraint_idx;
    }

    void setRHS(double bound)
    {
        RHS = bound;
    }

    void setBoundType(bound_type bt)
    {
        this->bt = bt;
    }

    bound_type getBoundType()
    {
        return bt;
    }

    double getRHS()
    {
        return RHS;
    }

    void printInfo()
    {
        cout << "index = " << constraint_idx << " : Bound Type = " << boundtypes_strings[bt] << " : RHS = " << RHS << endl;
        cout << "constraint is: ";
        for (auto& term : constraint_terms) {
            cout << term.second << "*" << term.first << " ";
        }
        cout << endl;
    }

    vector<pair<int, double>> getConTerms()
    {
        return constraint_terms;
    }

    int getNumVar()
    {
        return variable_indxs.size();
    }

    double getLargestRHSLHSRatio()
    {
        double largest_ratio = 0.00;
        for (auto& var_coeff : variable_coeffs) {
            if (std::abs(RHS / var_coeff) > largest_ratio) {
                largest_ratio = std::abs(RHS / var_coeff);
            }
        }
        return largest_ratio;
    }

    double getSumRHSLHSRatio()
    {
        double sum_RHS_ratio = 0.00;
        for (auto& var_coeff : variable_coeffs) {
            sum_RHS_ratio += std::abs(RHS / var_coeff);
        }
        return sum_RHS_ratio;
    }

private: 
    vector<string> boundtypes_strings = { "Equal", "Less", "Greater" };
    double RHS = 0.00;
    int constraint_idx;
    bound_type bt;
    vector<pair<int, double>> constraint_terms;
    vector<int> variable_indxs;
    vector<double> variable_coeffs;
};

#endif