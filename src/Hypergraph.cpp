#include "Hypergraph.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using std::cout;
using std::vector;
using std::endl;


void Hypergraph::identifyPartitions(const vector<bool>& relaxed_edges)
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
    for (int edge_idx = 0; edge_idx<relaxed_edges.size(); edge_idx++){
        if (relaxed_edges[edge_idx] == true){
            edge_idx_seen[edge_idx] = true;
        }
    }

    int nodes_seen = 0;
    //perform BFS on Hypergraph - stop when all nodes have been considered
    while (nodes_seen != node_idx_seen.size()){

        // pick first unseen node iff it has an edge attached
        int node_idx_selected;
        for (int i = 0; i < node_idx_seen.size(); i++) {
            if (node_idx_seen[i] == false) {
                // if (nodes[i].getEdgeIdxsSize() == 0){
                //     node_idx_seen[i] = true;
                //     continue;
                // }
                node_idx_selected = i;
                break;
            }
        }

        HG_Node node_selected = HG_nodes[node_idx_selected];
        node_idx_seen[node_idx_selected] = true;
        nodes_seen++;
        // find the partition with the the selected node belongs to
        findPartition(node_selected, node_idx_seen, edge_idx_seen, nodes_seen);
    }
    return;
}


void Hypergraph::findPartition(HG_Node starting_node,  vector<bool>& node_idx_seen, vector<bool>& edge_idx_seen, int& nodes_seen)
{
    
    vector<int> node_partition;
    vector<int> edge_partition;
    node_partition.push_back(starting_node.getNodeIdx());
    queue<HG_Edge> Q;
    
    // for each node, add neighbour nodes to the queue which haven't been seen before

    // add edges attached to node which haven't been seen before
    for (auto& edge_idx : starting_node.getEdgeIdxs()) {
        if (edge_idx_seen[edge_idx] == false) {
            Q.push(HG_edges[edge_idx]);
            // cout << "edge idx to be added " << edge_idx << endl;
            edge_partition.push_back(edge_idx);
            edge_idx_seen[edge_idx] = true;
        }
    }

    while (!Q.empty()) {
        HG_Edge current_edge = Q.front();
        // for each node in the hyperedge, add the node to the node_partition if it hasn't already been added
        for (auto& node_idx : current_edge.getNodeIdxs()) {
            if (node_idx_seen[node_idx] == false) {
                node_idx_seen[node_idx] = true;
                nodes_seen++;
                HG_Node node_added = HG_nodes[node_idx];
                node_partition.push_back(node_idx);
                int current_edge_idx = current_edge.getEdgeIdx();
                // add edges attached to the node to the queue if they haven't been seen previously
                for (auto& edge_idx : node_added.getEdgeIdxs()) {
                    if (edge_idx_seen[edge_idx] == false) {
                        Q.push(HG_edges[edge_idx]);
                        edge_idx_seen[edge_idx] = true;
                        edge_partition.push_back(edge_idx);
                    }
                }
            }
        }
        Q.pop();
    }

    // if (edge_partition.empty()){
    //     cout << "empty edge partition" << endl;
    // }
    if (node_partition.empty()){
        cout << "empty node partition" << endl;
        exit(1);
    }

    Partition_Struct ps = {node_partition, edge_partition};
    PS.push_back(ps);
    if (node_partition.size() > largest_partition){
        largest_partition = node_partition.size();
    }
    return;
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
        for (auto& edge_idx : partition.edge_idxs){
            cout << edge_idx << " ";
        }
        cout << endl;

        cout << "Nodes in partition: " << endl;
        for (auto& node_idx : partition.node_idxs){
            cout << node_idx << " ";
        }
        cout << endl;
    }
}

bool Hypergraph::partitionValidity(const vector<int>& relaxed_edge_idxs){

    bool flag = true;
    vector<bool> nodes_seen;
    nodes_seen.resize(HG_nodes.size(),false);
    vector<bool> edges_seen;
    edges_seen.resize(HG_edges.size(),false);

    for (auto& relaxed_edge_idx : relaxed_edge_idxs){
        edges_seen[relaxed_edge_idx] = true;
    }

    for (auto& partition : PS) {
        for (auto& node_idx : partition.node_idxs){
            if (nodes_seen[node_idx] == true){
                cout << "node idx " << node_idx << " appears in multiple partitions " << endl;
                flag = false;
            }
            nodes_seen[node_idx] = true;
        }
        for (auto& edge_idx : partition.edge_idxs){
            if (edges_seen[edge_idx] == true){
                cout << "edge idx " << edge_idx << " appears in multiple partitions " << endl;
                flag = false;
            }
            edges_seen[edge_idx] = true;
        }
    }
    return flag;
}

/* Reduce the graph by removing edges based on the constraints relaxed */
// 

void Hypergraph::updateNodes(const vector<double>& constraints_selected, vector<HG_Node>& updated_nodes)
{
    // copy the original node information
    for (auto& node : HG_nodes){
        updated_nodes[node.getNodeIdx()] = HG_Node{node.getNodeIdx(),node.getEdgeIdxs()};
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
            for (auto& node_idx : current_edge.getNodeIdxs()){
                //remove edge idx attached to node
                updated_nodes[node_idx].removeEdgeIdx(current_edge.getEdgeIdx());
            }
        }
        edge_idx++;
    }
}

bool Hypergraph::partition(const vector<bool>& constraints_selected){

    bool partition_success_flag = false;
     // reduce graph edges
    // logic behind a partition -- if you remove edges from the graph, can't reach other nodes... 

    // clear any partitions that exist and set the largest partition size to 0
    largest_partition = 0;
    PS.clear();
    // identify partitions with new edges...
    identifyPartitions(constraints_selected);
    vector<int> relaxed_constraint_idxs;
    for (int con_idx=0; con_idx<constraints_selected.size(); con_idx++){
        if (constraints_selected[con_idx] == true){
            relaxed_constraint_idxs.push_back(con_idx);
        }
    }

    partition_success_flag = partitionValidity(relaxed_constraint_idxs);
    return partition_success_flag;
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

void Hypergraph::constraintNumberCheck(const int& num_constraints_relaxed){

    int num_edges_in_partitions = getNumEdgesInPartitions();
    if ((num_edges_in_partitions+ num_constraints_relaxed) != getNumEdges()){
        cout << "number of constraints in partitions: " << num_edges_in_partitions << endl;
        cout << "number of constraints relaxed: " << num_constraints_relaxed << endl;
        cout << "total number of constraints " <<  getNumEdges() << endl;
        cout << "missing " << (getNumEdges() - (num_edges_in_partitions + num_constraints_relaxed)) << " constraints" << endl;
    } 
    return;
}

void Hypergraph::variableNumberCheck(){
    int num_nodes_in_partitions = getNumNodesInPartitions();
     if (num_nodes_in_partitions  != getNumNodes()){
        cout << "number of nodes in partitions: " << num_nodes_in_partitions  << endl;
        cout << "total number of nodes " <<  getNumNodes() << endl;
        cout << "missing " << (getNumEdges() - (num_nodes_in_partitions)) << " constraints" << endl;
    } 
    return;
}

// Two checks for success : 1) Successful partition 2) Correct Proportion found.
 bool Hypergraph::getPartitionStruct(const vector<bool>& con_relax_vec, const double& sp_prop, vector<Partition_Struct>& ps_input){

    bool success_flag = partition(con_relax_vec);
    if (success_flag == false){
        cout << "partition unsuccessful" << endl;
    }
    int num_con_relaxed = 0;
    for (auto&& val: con_relax_vec){
        if (val == true){
            num_con_relaxed++;
        }
    }

    if (getLargestPartition() > (sp_prop * getNumNodes())) {
        success_flag = false;
        cout << "decomp proportion not found " << endl;
    }

    cout << "finished running HG partitioning" << endl;
    ps_input = PS;
    return success_flag;
 }



