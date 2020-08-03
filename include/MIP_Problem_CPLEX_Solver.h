#ifndef __MIP_PROBLEM_CPLEX_SOLVER__
#define __MIP_PROBLEM_CPLEX_SOLVER__

#include "MIP_Problem.h"
#include "Constraint.h"
#include "Variable.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;


class MIP_Problem_CPLEX_Solver{
    public:
        MIP_Problem_CPLEX_Solver(const MIP_Problem& MP){
            this->MP = MP;
           
        }
        void solve();
    
    private:

        MIP_Problem MP;
        vector<double> original_costs;
};

#endif