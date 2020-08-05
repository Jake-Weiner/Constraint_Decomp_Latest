#ifndef __SOLVEGENERICMIP__
#define __SOLVEGENERICMIP__

#include "Util.h"
#include "GenericMIPInterface.h"
#include <string>

using std::string;

class SolveGenericMIP: public GenericMIPInterface{

    public:
        SolveGenericMIP(){};
        SolveGenericMIP(const string MIP_input_filename, double solver_time_lim):
        GenericMIPInterface(solver_time_lim) {
            this->MIP_input_filename = MIP_input_filename;
        };
        CPLEX_Return_struct solve(bool randomSeed) override;
    private:
        double solver_time_lim = 0;
        string MIP_input_filename = "";

};


#endif