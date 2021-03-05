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
        // default rhs value is 0 if rhs is not included
        RHS = 0.0;
    };
    ~Constraint(){};

    void addVarIdx(const int& var_idx_to_add);
    void addVarCoeff(const double& var_coeff_to_add)
    {
        variable_coeffs.push_back(var_coeff_to_add);
    }

    void addConTerm(const pair<int, double> term)
    {
        constraint_terms.push_back(term);
    }

    vector<int> getVarIndxs() const
    {
        return variable_indxs;
    }

    vector<double> getVarCoeffs() const
    {
        return variable_coeffs;
    }

    int getConIdx() const
    {
        return constraint_idx;
    }

    void setRHS(const double bound)
    {
        RHS = bound;
    }

    void setBoundType(bound_type bt)
    {
        this->bt = bt;
    }

    bound_type getBoundType() const{
        return bt;
    }

    double getRHS() const
    {
        return RHS;
    }

    void printInfo() const
    {
        cout << "index = " << constraint_idx << " : Bound Type = " << boundtypes_strings[bt] << " : RHS = " << RHS << endl;
        cout << "constraint is: ";
        for (auto& term : constraint_terms) {
            cout << term.second << "*" << term.first << " ";
        }
        cout << endl;
    }

    vector<pair<int, double>> getConTerms() const
    {
        return constraint_terms;
    }

    int getNumVar() const
    {
        return variable_indxs.size();
    }

    // does the constraint contain this index
    bool containsVar(const int& var_idx) const;

    double getLargestRHSLHSRatio() const;
    double getSumRHSLHSRatio() const;

private: 
    vector<string> boundtypes_strings = { "Equal", "Less", "Greater" };
    //default bound is 0
    double RHS;
    int constraint_idx;
    bound_type bt;
    vector<pair<int, double>> constraint_terms;
    vector<int> variable_indxs;
    vector<double> variable_coeffs;
};


#endif