#ifndef __MIP_PROBLEM_CPLEX_SOLVER__
#define __MIP_PROBLEM_CPLEX_SOLVER__

#include "MIP_Problem.h"
#include "Constraint.h"
#include "Variable.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "Util.h"
#include "GenericMIPInterface.h"

using std::vector;


class MIP_Problem_CPLEX_Solver: public GenericMIPInterface{
    public:
        MIP_Problem_CPLEX_Solver(const MIP_Problem& MP, const double& solver_time_lim) :
        GenericMIPInterface(solver_time_lim) {
            this->MP = MP;
        }
        // override the base class solve method
        CPLEX_Return_struct solve(bool randomSeed) override;
    
    private:
        MIP_Problem MP;
        vector<double> original_costs;
};

#endif