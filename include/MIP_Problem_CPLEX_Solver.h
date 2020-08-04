#ifndef __MIP_PROBLEM_CPLEX_SOLVER__
#define __MIP_PROBLEM_CPLEX_SOLVER__

#include "MIP_Problem.h"
#include "Constraint.h"
#include "Variable.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "Util.h"

using namespace std;


class MIP_Problem_CPLEX_Solver{
    public:
        MIP_Problem_CPLEX_Solver(const MIP_Problem& MP, const double& solver_time_lim){
            this->MP = MP;
            this->solver_time_lim = solver_time_lim;
        }
        CPLEX_Return_struct solve();
    
    private:

        double solver_time_lim = 0;
        MIP_Problem MP;
        vector<double> original_costs;
};

#endif