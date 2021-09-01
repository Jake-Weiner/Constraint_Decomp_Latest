#include "Hypergraph.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <ctime>



using std::cout;
using std::endl;
using std::vector;

void Hypergraph::identifyPartitions(const vector<int>& relaxed_edges)
{

    

    // overall algorithm
    // List keeping track of seen edges
    // vector<bool> edge_idx_seen;
    // edge_idx_seen.resize(HG_edges.size(), false);
    // List keeping track of nodes seen

    bool return_flag = true;
    vector<bool> node_idx_seen;
    node_idx_seen.resize(HG_nodes.size(), false);
    vector<bool> edge_idx_seen;
    edge_idx_seen.resize(HG_edges.size(), false);

    // cout<< "No. Nodes " << HG_nodes.size() << endl;
    cout<< "No. Relaxed Edges " << relaxed_edges.size() << endl;
    for (const auto& edge_idx : relaxed_edges) {
        edge_idx_seen[edge_idx] = true;        
    }

    int nodes_seen = 0;

    cout << "Starting Find Partition Loop" << endl;
    std::clock_t c_start = std::clock();
    //perform BFS on Hypergraph - stop when all nodes have been considered
    int last_seen_node_idx = 0;
    while (nodes_seen != node_idx_seen.size()) {
        // pick first unseen node
        int node_idx_selected;
        for (int i = last_seen_node_idx; i < node_idx_seen.size(); ++i) {
            if (node_idx_seen[i] == false) {
                node_idx_selected = i;
                break;
            }
        }
       
        // HG_Node node_selected = HG_nodes[node_idx_selected];
        node_idx_seen[node_idx_selected] = true;
        // another node has been seen
        ++nodes_seen;
        // find the edges and nodes belonging to the partition with the the selected node
        findPartition(HG_nodes[node_idx_selected], node_idx_seen, edge_idx_seen, nodes_seen);
        last_seen_node_idx = node_idx_selected;
    }


    cout << "Finished Hypergraph Partitioning" << endl;
    std::clock_t c_end = std::clock();
    long double time_elapsed_ms = 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC;
    std::cout << "CPU time used: " << time_elapsed_ms << " ms\n";
    return;
}

void Hypergraph::findPartition(const HG_Node& starting_node, vector<bool>& node_idx_seen, vector<bool>& edge_idx_seen, int& nodes_seen)
{
    vector<int> node_partition;
    vector<int> edge_partition;
    node_partition.push_back(starting_node.getNodeIdx());
    queue<HG_Edge*> Q;
    // for each node add edges attached to node which haven't been seen before
    for (auto& edge_idx : starting_node.getEdgeIdxs()) {
        if (edge_idx_seen[edge_idx] == false) {
            Q.push(&HG_edges[edge_idx]);
            edge_partition.push_back(edge_idx);
            edge_idx_seen[edge_idx] = true;
        }
    }
    while (!Q.empty()) {
        HG_Edge* current_edge = Q.front();
        // for each node in the hyperedge, add the node to the node_partition if it hasn't already been added
        for (auto& node_idx : current_edge->getNodeIdxs()) {
            if (node_idx_seen[node_idx] == false) {
                node_idx_seen[node_idx] = true;
                ++nodes_seen;
                HG_Node node_added = HG_nodes[node_idx];
                node_partition.push_back(node_idx);
                // add edges attached to the node to the queue if they haven't been seen previously
                for (auto& edge_idx : node_added.getEdgeIdxs()) {
                    if (edge_idx_seen[edge_idx] == false) {
                        Q.push(&HG_edges[edge_idx]);
                        edge_idx_seen[edge_idx] = true;
                        edge_partition.push_back(edge_idx);
                    }
                }
            }
        }
        Q.pop();
    }
    if (node_partition.empty()) {
        cout << "empty node partition" << endl;
        exit(1);
    }
    // create partition struct containg indicies of all nodes and edges
    Partition_Struct ps = {node_partition, edge_partition};
    PS.push_back(ps);
    if (node_partition.size() > largest_partition) {
        largest_partition = node_partition.size();
    }
}

void Hypergraph::printEdges()
{
    for (auto& edge : HG_edges) {
        cout << "edge no. " << edge.getEdgeIdx() << " ";
        edge.printNodes();
    }
}

void Hypergraph::printPartitions()
{
    for (auto& partition : PS) {
        cout << "Edges in partition: " << endl;
        for (auto& edge_idx : partition.edge_idxs) {
            cout << edge_idx << " ";
        }
        cout << endl;

        cout << "Nodes in partition: " << endl;
        for (auto& node_idx : partition.node_idxs) {
            cout << node_idx << " ";
        }
        cout << endl;
    }
}

// unit testing for Hypergraph validity
void Hypergraph::partitionValidity(const vector<int>& relaxed_edge_idxs)
{

    bool partition_validity_flag = true;
    vector<bool> nodes_seen;
    // initialise all nodes and edges as unseen
    nodes_seen.resize(HG_nodes.size(), false);
    vector<bool> edges_seen;
    edges_seen.resize(HG_edges.size(), false);

    // vector containing the indexes of relaxed edges
    for (auto& relaxed_edge_idx : relaxed_edge_idxs) {
        edges_seen[relaxed_edge_idx] = true;
    }

    // loop through the various partitions to determine if nodes or edges
    // appear in multiple partitions
    for (auto& partition : PS) {
        for (auto& node_idx : partition.node_idxs) {
            if (nodes_seen[node_idx] == true) {
                cout << "node idx " << node_idx << " appears in multiple partitions " << endl;
                partition_validity_flag = false;
            } else {
                nodes_seen[node_idx] = true;
            }
        }
        for (auto& edge_idx : partition.edge_idxs) {
            if (edges_seen[edge_idx] == true) {
                cout << "edge idx " << edge_idx << " appears in multiple partitions " << endl;
                partition_validity_flag = false;
            } else {
                edges_seen[edge_idx] = true;
            }
        }
    }
    // make sure that all edges and nodes have been seen
    for (int i = 0; i < nodes_seen.size(); i++) {
        if (nodes_seen[i] == false) {
            cout << "error, node_idx: " << nodes_seen[i] << " is unallocated in partitioning " << endl;
            partition_validity_flag = false;
        }
    }

    for (int i = 0; i < edges_seen.size(); i++) {
        if (edges_seen[i] == false) {
            cout << "error, edge_idx: " << edges_seen[i] << " is unallocated in partitioning " << endl;
            partition_validity_flag = false;
        }
    }

    if (partition_validity_flag == false) {
        exit(1);
    }
}

/* Reduce the graph by removing edges based on the constraints relaxed */
//

void Hypergraph::updateNodes(const vector<double>& constraints_selected, vector<HG_Node>& updated_nodes)
{
    // copy the original node information
    for (auto& node : HG_nodes) {
        updated_nodes[node.getNodeIdx()] = HG_Node{ node.getNodeIdx(), node.getEdgeIdxs() };
    }
    // ensure constraint vector input matches number of edges in HG
    if (constraints_selected.size() != HG_edges.size()) {
        cout << "incorrect input vector size of relaxed constraints" << endl;
    }
    int edge_idx = 0;
    for (auto& constraint_decision : constraints_selected) {
        HG_Edge current_edge = HG_edges[edge_idx];
        // 1 is edge relaxation
        if (constraint_decision == 1) {
            for (auto& node_idx : current_edge.getNodeIdxs()) {
                //remove edge idx attached to node
                updated_nodes[node_idx].removeEdgeIdx(current_edge.getEdgeIdx());
            }
        }
        edge_idx++;
    }
}
//unit testing flag to test if partioning algorithm should be checked

// partition the hypergraph based on selected constraints
void Hypergraph::partition(const vector<int>& constraints_selected, bool test_partition_validity)
{

    // reduce graph edges
    // logic behind a partition -- if you remove edges from the graph, can't reach other nodes...
    // clear any partitions that exist and set the largest partition size to 0
    largest_partition = 0;
    PS.clear();
    // identify partitions with new edges...
    identifyPartitions(constraints_selected);
    if (test_partition_validity) {
        // check partition validity
        partitionValidity(constraints_selected);
    }
}

// Largest partition is updated during paritioning search
int Hypergraph::getLargestPartition()
{
    return largest_partition;
}

void Hypergraph::printPartitions(vector<vector<int>> partitions)
{
    for (auto& partition : partitions) {
        cout << "Partition - ";
        for (auto& node : partition) {
            cout << " " << node;
        }
        cout << endl;
    }
}

void Hypergraph::constraintNumberCheck(const int& num_constraints_relaxed)
{

    int num_edges_in_partitions = getNumEdgesInPartitions();
    if ((num_edges_in_partitions + num_constraints_relaxed) != getNumEdges()) {
        cout << "number of constraints in partitions: " << num_edges_in_partitions << endl;
        cout << "number of constraints relaxed: " << num_constraints_relaxed << endl;
        cout << "total number of constraints " << getNumEdges() << endl;
        cout << "missing " << (getNumEdges() - (num_edges_in_partitions + num_constraints_relaxed)) << " constraints" << endl;
    }
    return;
}

void Hypergraph::variableNumberCheck()
{
    int num_nodes_in_partitions = getNumNodesInPartitions();
    if (num_nodes_in_partitions != getNumNodes()) {
        cout << "number of nodes in partitions: " << num_nodes_in_partitions << endl;
        cout << "total number of nodes " << getNumNodes() << endl;
        cout << "missing " << (getNumEdges() - (num_nodes_in_partitions)) << " constraints" << endl;
    }
    return;
}

// Two checks for success : 1) Successful partition 2) Correct Proportion found.
vector<Partition_Struct> Hypergraph::getPartitionStruct(const vector<int>& con_relax_vec, bool check_partition_validity)
{


    partition(con_relax_vec, check_partition_validity);
    cout << "finished running HG partitioning" << endl;
    return PS;
}

//
vector<int> Hypergraph::removeRelaxedConstraintRedundancies(const vector<int>& relaxed_edges)
{

    vector<int> new_constraint_vector;
    // new_constraint_vector.resize(relaxed_edges.size());

    // relaxed edges as bool vec
    bool print = false;
 
    // partition the hypergraph based on the relaxed edges provided
    partition(relaxed_edges, false);
    // generate hashmap of all single variable indicies
    //loop through the constraints and see if the variables in each constraint is contained within any of the subproblems
    for (const auto& con_idx : relaxed_edges) {
        // if constraint is redundant, discard it as part of relaxed onstraints
        // otherwise keep it
        if (isConstraintRedundant(HG_edges[con_idx], print)) {
            std::cout << "redundant constraint found " << std::endl;
        } else {
            new_constraint_vector.push_back(con_idx);
        }
    }
    return new_constraint_vector;
}

// check if the edge in question is already contained within one of the subproblem partitions
// if it is, unrelax the constraint as relaxing this constraint doesn't provide any benefit
bool Hypergraph::isConstraintRedundant(HG_Edge& edge_to_check, bool print)
{
    bool ret_val = false;
    // loop through each partition
    for (auto& partition : PS) {
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
                std::vector<int>::iterator it = std::find(partition.node_idxs.begin(), partition.node_idxs.end(), node_idx_to_find);
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
