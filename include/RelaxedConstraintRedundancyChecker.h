
#ifndef __RELAXEDCONSTRAINTREDUNDANCYCHECKER__
#define __RELAXEDCONSTRAINTREDUNDANCYCHECKER__

#include "Hypergraph.h"
#include <unordered_map>
#include <vector>

class RelaxedConstraintRedundancyChecker{

    public:
        RelaxedConstraintRedundancyChecker(){};
        vector<int> removeRedundantConstraints(const vector<int>& original_relaxed_constraints, Hypergraph& HG);
        

    private:
        std::unordered_map<int,bool> single_var_subproblem_idxs;
        void populateSingleVarHashmap(const vector<Partition_Struct>& partitions);
        bool isConstraintRedundant(const vector<Partition_Struct>& partitions, const HG_Edge& edge_to_check, bool print);
        bool containsOnlySingleVarSubproblems(const HG_Edge& edge_to_check);
};

#endif