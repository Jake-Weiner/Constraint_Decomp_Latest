#ifndef __MIP_PROBLEM_PROBE__
#define __MIP_PROBLEM_PROBE__

#include "MIP_Problem.h"
#include "Util.h"
#include <vector>
#include <tuple>
#include <utility>

class MIPProblemProbe{

    public:
        MIPProblemProbe(MIP_Problem* MP_ptr);
        // void populateInstanceStatistics(instance_statistics& is, MIP_Problem& MP);
        // void populateNonruntimeDecompositionStatistics(const int& decomposition_idx, const individual_information_struct& decomp, Nonruntime_Relaxed_Constraint_Statistics& nrcs, MIP_Problem& MP);
        std::vector<double> getLargestRHSLHSRatios(const std::vector<int>& constraint_idxs);
        std::vector<double> getSumRHSLHSRatios(const std::vector<int>& constraint_idxs);
        std::vector<double> getConstraintSumObjs(const std::vector<int>& constraint_idxs);
        std::vector<double> getConstraintSumAbsObjs(const std::vector<int>& constraint_idxs);
        std::vector<double> getConstraintRHSVals(const std::vector<int>& constraint_idxs);
        
        std::vector<int> getConstraintNonZeroCounts(const std::vector<int>& constraint_idxs);
        double getBlockSumObjs(const std::vector<int>& variable_idxs, const bool& abs);
        // get prop of var types from constraint indices provided
        std::tuple<vector<double>, vector<double>, vector<double>> getVariableProps(const std::vector<int>& constraint_idxs);
        // get variable props from single constraint
        std::tuple<double, double, double> getVariablePropsConstraint(const int& constraint_idx);

        // given a vector of constraints, count the total number of variables
        int getVarCount(const std::vector<int>& constraint_idxs);
        double getAverageBlockRHS(const std::vector<int>& constraint_idxs, const bool& abs_flag);
        double getAverageBlockLargestRHSLHSRatio(const std::vector<int>& constraint_idxs);
        double getBlockLargestRHSRange(const std::vector<int>& constraint_idxs);
        int getBlockNonZeroes(const std::vector<int>& constraint_idxs);
        std::tuple<int, int, int> getVariableCounts(const std::vector<int>& variable_indexes);
        double getBlockObjRange(const std::vector<int>& variable_indexes);

        double getEqualityConstraintProp(const std::vector<int>& constraint_idxs);
        int getNumMIPConst();
        int getNumMIPVar();
        int getNumMIPBin();
        int getNumMIPCont();
        int getNumMIPInt();
        int getNumMIPNonZero();
        
        // Instance min/max's
        std::pair<double,double> getObjExtremes();
        std::pair<double,double> getRHSExtremes();
        std::pair<double,double> getRHSLHSExtremes();
        std::pair<double,double> getSumObjExtremes();
        std::pair<double,double> getSumAbsObjExtremes();
     


    private:

        int getNumberEqualityConstraintsRelaxed(const vector<double>& con_vec, MIP_Problem& MP);
        double getAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        double getStddevNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        void calculateAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);
        double getAverageLargestRatioInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);

        // void populateNonRuntimeAverages(const individual_information_struct& decomp, MIP_Problem& MP, Nonruntime_Relaxed_Constraint_Statistics& nds);

        MIP_Problem* MP_ptr;
        
        // bool calculate_average_statistics_flag = false;
        unsigned int number_constraints_relaxed = 0;
        // bool calculuated_average_nonzeroes_flag = false;
        // double average_nonzero_in_relaxed_constraints = 0;



};

#endif