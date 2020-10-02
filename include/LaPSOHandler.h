#ifndef __LaPSOHandler__
#define __LaPSOHandler__

#include <string>
#include "ConDecomp_LaPSO_Connector.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"
#include "Util.h"

using namespace std;


class LaPSOHandler{

    public:
        LaPSOHandler(int& argc, const char** argv, LaPSO::LaPSORequirements& LR);
        void solve(ConDecomp_LaPSO_Connector& CLC);
        std::vector<double> getSolverTimingTracking(){
            return solver.timing_tracking;
        }

        std::vector<double> getSolverAverageLBTracking(){
             return solver.average_lb_tracking;
        }
         std::vector<double> getSolverBestLBTracking(){
             return solver.best_lb_tracking;
        }

        // std::vector<double> average_lb_tracking;
		// std::vector<double> average_ub_tracking;
		// std::vector<double> average_viol_tracking;
		// std::vector<double> average_path_saved_tracking;
		// std::vector<double> best_lb_tracking;
		// std::vector<double> lb_comparisons;
		// std::vector<double> best_ub_tracking;
		// std::vector<double> dual_0_tracking;
		// std::vector<double> dual_euclid;
		// std::vector<double> perturb_euclid;
		// std::vector<double> timing_tracking;
    private:
        void initSolver(int& argc, const char** argv,LaPSO::LaPSORequirements& LR);
        
       
        LaPSO::Problem solver;

};


#endif