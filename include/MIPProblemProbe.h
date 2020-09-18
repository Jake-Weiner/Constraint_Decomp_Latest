#ifndef __MIP_PROBLEM_PROBE__
#define __MIP_PROBLEM_PROBE__

#include "MIP_Problem.h"
#include "Util.h"

class MIPProblemProbe{

    public:
        MIPProblemProbe(){};
        void populateInstanceStatistics(instance_statistics& is, MIP_Problem& MP);
        void populateNonruntimeDecompositionStatistics(const int& decomposition_idx, const individual_information_struct& decomp, Nonruntime_Relaxed_Constraint_Statistics& nrcs, MIP_Problem& MP);
        

    private:

        int getNumberEqualityConstraintsRelaxed(const vector<double>& con_vec, MIP_Problem& MP);
        double getAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        double getStddevNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        void calculateAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        double getAverageLargestRatioInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);

        void populateNonRuntimeAverages(const individual_information_struct& decomp, MIP_Problem& MP, Nonruntime_Relaxed_Constraint_Statistics& nds);

        
        // bool calculate_average_statistics_flag = false;
        unsigned int number_constraints_relaxed = 0;
        // bool calculuated_average_nonzeroes_flag = false;
        // double average_nonzero_in_relaxed_constraints = 0;



};

#endif