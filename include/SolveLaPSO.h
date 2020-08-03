#ifndef __SolveLaPSO__
#define __SolveLapso__

#include <string>
#include "ConDecomp_LaPSO_Connector.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"

using namespace std;


class SolveLaPSO{

    public:
        SolveLaPSO(int& argc, const char** argv, int num_var, int num_con, const double& best_ub_sol,
        LaPSO::constraint_type_indicies& cti);
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
        void initSolver(int& argc, const char** argv,int num_var, int num_con, const double& best_ub_sol,LaPSO::constraint_type_indicies& cti);
        LaPSO::Problem solver;

};


#endif