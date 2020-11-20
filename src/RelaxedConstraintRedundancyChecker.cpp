#include "RelaxedConstraintRedundancyChecker.h"
#include <algorithm>

vector<int> RelaxedConstraintRedundancyChecker::removeRedundantConstraints(const vector<int>& original_relaxed_constraints, Hypergraph& HG)
{
    vector<int> new_constraint_vector;
    // new_constraint_vector.resize(relaxed_edges.size());

    // relaxed edges as bool vec
    bool print = false;
    vector<Partition_Struct> partitions = HG.getPartitionStruct(original_relaxed_constraints, false);
    // create hashmap of single variable problems
    //reset the hashmap
    single_var_subproblem_idxs.clear();
    populateSingleVarHashmap(partitions);

    // partition the hypergraph based on the relaxed edges provided

    // generate hashmap of all single variable indicies
    //loop through the constraints and see if the variables in each constraint is contained within any of the subproblems
    for (const auto& con_idx : original_relaxed_constraints) {
        // if constraint is redundant, discard it as part of relaxed onstraints
        // otherwise keep it
        HG_Edge edge_to_check = HG.getEdge(con_idx);

        // constraint is not redundant if it does not only contain single var problems
        // and it does not a subset of a subproblem variable
        if (!containsOnlySingleVarSubproblems(edge_to_check) && !isConstraintRedundant(partitions, edge_to_check, print)){
            new_constraint_vector.push_back(con_idx);
        }
        // // first check is if constraint only contains single subproblems
        // if (containsOnlySingleVarSubproblems(edge_to_check)){
        //     std::cout << "redundant constraint found " << std::endl;
        // }
        // // second check is if constraint is already found within a subproblem
        // else if (isConstraintRedundant(partitions, edge_to_check, print)){
        //      std::cout << "redundant constraint found " << std::endl;
        // }
        // // if constraint passes both checks, keep it relaxed still
        // else {
        //     new_constraint_vector.push_back(con_idx);
        // }
    }
    return new_constraint_vector;
}

// there are 2 ways a relaxed constraint is redundant
// 1) a relaxed constraint contains only variables already belonging to a subproblem
// 2) a relaxed constraint contains only a subset of variables in single variable problems
bool RelaxedConstraintRedundancyChecker::isConstraintRedundant(const vector<Partition_Struct>& partitions,const HG_Edge& edge_to_check, bool print)
{
    bool ret_val = false;
    // check if constraint only containts single variable subproblems
    // loop through each partition to see if constraint variables are a subset of a subproblem. If they are the relaxed constraint
    // is redundant (provides no decomposition benefit)
    for (auto& partition : partitions) {
        bool all_nodes_found = true;
        // number of nodes in partition must be greater/equal than the number of non zeroes/variables in the constraint being tested
        if (edge_to_check.getNumNodes() <= partition.getNumNodes()) {
            if (print) {
                std::cout << "partition nodes are " << std::endl;
                for (auto& partition_node : partition.node_idxs) {
                    std::cout << partition_node << " ";
                }
                std::cout << std::endl;
                std::cout << "constraint nodes are " << std::endl;
                for (auto& node_idx_to_find : edge_to_check.getNodeIdxs()) {
                    std::cout << node_idx_to_find << " ";
                }
                std::cout << std::endl;
            }
            // check if each node contained within the edge is also in the partition
            for (auto& node_idx_to_find : edge_to_check.getNodeIdxs()) {
                std::vector<int>::const_iterator it = std::find(partition.node_idxs.begin(), partition.node_idxs.end(), node_idx_to_find);
                // node not found
                if (it == partition.node_idxs.end()) {
                    // node idx is not in the current partition and so move onto the next partition
                    // without checking the rest of the nodes in the constraint
                    all_nodes_found = false;
                    break;
                }
            }
        }
        // if number of nodes in partition is not greater/equal than the number of variables
        // in the constraint then there is no need to check if the constraint is redundant w.r.t
        // that partition
        else {
            all_nodes_found = false;
            continue;
        }

        // if constraint nodes are a subset of partition nodes, redundant constaint is found and there is no need to
        // check the remaining partitions
        if (all_nodes_found == true) {
            ret_val = true;
            break;
        }
    }
    return ret_val;
}

bool RelaxedConstraintRedundancyChecker::containsOnlySingleVarSubproblems(const HG_Edge& edge_to_check){

    bool ret_val = true;
    // if constraint contains a variable that is NOT a single var subproblem, it is not redundant in this check
    for (const auto& var_idx : edge_to_check.getNodeIdxs()){
        // edge contains a variable that is a variable that is not! in the single var subproblem
        if (single_var_subproblem_idxs.find(var_idx) == single_var_subproblem_idxs.end()){
            ret_val = false;
            break;
        }
    }
    return ret_val;
}

void RelaxedConstraintRedundancyChecker::populateSingleVarHashmap(const vector<Partition_Struct>& partitions)
{
    // every subproblem with only 1 variable
    for (const auto& partition : partitions) {
        if (partition.getNumNodes() == 1) {
            single_var_subproblem_idxs[partition.node_idxs[0]] = true;
        }
    }
}
