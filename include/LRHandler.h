#ifndef __LRHandler__
#define __LRHandler__

#include <string>
#include <tuple>
#include "ConDecomp_LaPSO_Connector.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"
#include "Util.h"

using std::vector;

class LRHandler{

    public:
        LRHandler();
        ~LRHandler();
        void initLaPSOSolver(int& argc, const char** argv, LaPSO::LaPSORequirements& LR);
        // initialise the LaPSO connector which is used in conjunction with the LaPSO solver_ptr
        void initConnector(ConnectorRequirements& CR);
        void solve();

        std::tuple<double,double> getLaPSOOutputs();

        std::vector<double> getSolverTimingTracking(){
            return solver_ptr->timing_tracking;
        }

        std::vector<double> getSolverAverageLBTracking(){
             return solver_ptr->average_lb_tracking;
        }
         std::vector<double> getSolverBestLBTracking(){
             return solver_ptr->best_lb_tracking;
        }

        LaPSO::IncorrectInitialDualIndices getIncorrectInitialDualIndicies() const{
            return solver_ptr->getIncorrectInitialDualIndices();
        }

        std::shared_ptr<ConDecomp_LaPSO_Connector> getCLCPointer() const {
            return CLC_ptr;
        }

    private:
      
        std::shared_ptr<ConDecomp_LaPSO_Connector> CLC_ptr;
        bool LaPSO_initialised = false;
        bool Connector_initialised = false;
        std::shared_ptr<LaPSO::Problem> solver_ptr;

};


// struct LaPSORequirements{

// };


#endif