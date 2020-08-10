#ifndef __HG__
#define __HG__

#include <vector>
#include <string>
#include "HG_Node.h"
#include "HG_Edge.h"
#include <iostream>
using namespace std;
using std::cout;
using std::endl;
using std::vector;
using std::string;

struct Partition_Struct{
    vector<int> node_idxs;
    vector<int> edge_idxs;
    void printNodes() const{
        cout << "total nodes : " << node_idxs.size() << " nodes are :" << endl;
        for (auto& node : node_idxs){
            cout << node << " ";
        }
        cout << endl;
    }
    void printEdges() const{
         cout << "total edges : " << edge_idxs.size() << "edges are" << endl;
         for (auto& edge : edge_idxs){
            cout << edge << " ";
        }
        cout << endl;
    }
    
};

class Hypergraph {
public:
 
    Hypergraph(){};
    Hypergraph(vector<HG_Edge> HG_edges, vector<HG_Node> HG_nodes){

        this->HG_edges = HG_edges;
        this->HG_nodes = HG_nodes;
        this->num_nodes = HG_nodes.size();
        this->num_edges = HG_edges.size();
    };

    Hypergraph(int num_edges, int num_nodes){
        this->num_nodes = num_nodes;
        this->num_edges = num_edges;
        this->HG_nodes.resize(num_nodes);
        this->HG_edges.resize(num_edges);
    };
    
    void printEdges();
    void printNodes();
    void printPartitions();
    bool partitionValidity(const vector<int>& relaxed_edge_idxs);
    int getLargestPartition();
    bool partition(const vector<bool>& constraints_selected);

    bool getPartitionStruct(const vector<bool>& con_relax_vec, const double& sp_prop, vector<Partition_Struct>& ps_input);
    
    int getNumEdges(){
        return HG_edges.size();
    };
    int getNumNodes(){
        return HG_nodes.size();
    };
    vector<HG_Edge> getHGEdges(){
        return HG_edges;
    };
    vector<HG_Node> getHGNodes(){
        return HG_nodes;
    };

    int getNumEdgesInPartitions(){
        int count = 0;
        for (auto& partition : PS){
            count += partition.edge_idxs.size();
        }
        return count;
    };

    int getNumNodesInPartitions(){
        int count = 0;
        for (auto& partition : PS){
            count += partition.node_idxs.size();
        }
        return count;
    };

    void constraintNumberCheck(const int& num_constraints_relaxed);
    void variableNumberCheck();
    
private:
    void findPartition(HG_Node starting_node,  vector<bool>& node_idx_seen, vector<bool>& edge_idx_seen, int& nodes_seen);
    int num_nodes;
    int num_edges;
    vector<HG_Edge> HG_edges;
    vector<HG_Node> HG_nodes;
    void printPartitions(vector<vector<int>> partitions);
    void identifyPartitions(const vector<bool>& relaxed_edges);
    void updateNodes(const vector<double>& constraints_selected, vector<HG_Node>& updated_nodes);
    int largest_partition = 0;
    vector<Partition_Struct> PS;
    bool partitioned = false;

};


#endif