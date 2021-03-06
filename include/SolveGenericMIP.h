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
        CPLEX_Return_struct solve(bool randomSeed, bool LP) override;
    private:
       
        string MIP_input_filename = "";

};


#endif