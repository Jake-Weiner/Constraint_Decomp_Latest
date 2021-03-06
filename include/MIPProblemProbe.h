#ifndef __MIP_PROBLEM_PROBE__
#define __MIP_PROBLEM_PROBE__


#include "MIP_Problem.h"
#include "Util.h"


#include <vector>
#include <tuple>
#include <utility>

// forward declare decomposition statistics namespace with structs. Otherwise
// circular dependency occurs
namespace DecompositionStatistics{
    struct SubproblemConstraintStatistics;
    struct SubproblemVariableStatistics;
    struct RelaxedConstraintStatistics;
    struct RawInstanceConstrVals;
}




using DecompositionStatistics::SubproblemConstraintStatistics;
using DecompositionStatistics::SubproblemVariableStatistics;
using DecompositionStatistics::RelaxedConstraintStatistics;
using DecompositionStatistics::RawInstanceConstrVals;

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
        void getSubproblemConstraintStatistics(SubproblemConstraintStatistics& scs, const std::vector<int>& constraint_idxs);
        void getSubproblemVariableStatistics(SubproblemVariableStatistics& svs, const std::vector<int>& variable_idxs);
        void getRelaxedConstraintStatistics(RelaxedConstraintStatistics& rcs, const std::vector<int>& constraint_idxs);
        std::vector<int> getConstraintNonZeroCounts(const std::vector<int>& constraint_idxs);
        double getBlockSumObjs(const std::vector<int>& variable_idxs, const bool& abs);
        // get prop of var types from constraint indices provided
        std::tuple<vector<double>, vector<double>, vector<double>> getVariableProps(const std::vector<int>& constraint_idxs);
        // get variable props from single constraint
        std::tuple<double, double, double> getConstraintVarTypes(const int& constraint_idx);

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

        void getInstanceConstraintRawValues(RawInstanceConstrVals& ricv);

        // populates the given vector with the coefficients of the obj function
        void getInstanceObjTerms(vector<double>& obj_terms);
        // returns number of equality type constraints 
        int getInstanceEquality();
        // get constraints containing a specific variable index
        vector<int> getConstraintsContainingVar(const int& var_idx);
    
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