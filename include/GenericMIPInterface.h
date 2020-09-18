#ifndef __GENERICMIPINTERFACE__
#define __GENERICMIPINTERFACE__

#include "Util.h"

class GenericMIPInterface{
    public:
        GenericMIPInterface(){};
        GenericMIPInterface(double solver_time_lim){
            this->solver_time_lim = solver_time_lim;
        };
        virtual CPLEX_Return_struct solve(bool randomSeed, bool LP) = 0;

    protected:
        // void setSolverTime(double solver_time_lim){
        //     this->solver_time_lim = solver_time_lim;
        // }
        double solver_time_lim;
};

#endif


