#ifndef __LaPSOHandler__
#define __LaPSOHandler__

#include <string>
#include <tuple>
#include "ConDecomp_LaPSO_Connector.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"
#include "Util.h"


using namespace std;


class LaPSOHandler{

    public:
        LaPSOHandler(int& argc, const char** argv, LaPSO::LaPSORequirements& LR);
        
        void solve(ConDecomp_LaPSO_Connector& CLC);

        std::tuple<double,double> getLaPSOOutputs();

        std::vector<double> getSolverTimingTracking(){
            return solver.timing_tracking;
        }

        std::vector<double> getSolverAverageLBTracking(){
             return solver.average_lb_tracking;
        }
         std::vector<double> getSolverBestLBTracking(){
             return solver.best_lb_tracking;
        }

        LaPSO::IncorrectInitialDualIndices getIncorrectInitialDualIndicies() const{
            return solver.getIncorrectInitialDualIndices();
        }

    private:
        void initSolver(int& argc, const char** argv,LaPSO::LaPSORequirements& LR);
        
       
        LaPSO::Problem solver;

};


#endif