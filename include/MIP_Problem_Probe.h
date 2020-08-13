#ifndef __MIP_PROBLEM_PROBE__
#define __MIP_PROBLEM_PROBE__

#include "MIP_Problem.h"
#include "Util.h"

class MIP_Problem_Probe{

    public:
        MIP_Problem_Probe(){};
        void populateInstanceStatistics(NSGA_ii_instance_statistics& nis, MIP_Problem& MP);
        void populateRelaxedConstraintsStatistics(const int& decomposition_idx, const individual_information_struct& decomp, NSGA_ii_relaxed_constraint_statistics& nrcs, MIP_Problem& MP);
        

    private:

};

#endif