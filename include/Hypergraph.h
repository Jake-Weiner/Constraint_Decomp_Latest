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

    int getNumNodes() const {
        return node_idxs.size();
    }

    int getNumEdges() const {
        return edge_idxs.size();
    }
    
};

class Hypergraph {
public:
 
    Hypergraph(){};
    Hypergraph(const vector<HG_Edge>& HG_edges, const vector<HG_Node>& HG_nodes){

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
    
    void printPartitions();
    void partitionValidity(const vector<int>& relaxed_edge_idxs);
    int getLargestPartition();
    void partition(const vector<int>& constraints_selected, bool test_partition_validity);

    vector<Partition_Struct> getPartitionStruct(const vector<int>& con_relax_vec, bool check_partition_validity);
    
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

    vector<int> removeRelaxedConstraintRedundancies(const vector<int>& relaxed_edges);
    
    HG_Edge getEdge(const int& edge_idx){
        HG_Edge e;
        if ((edge_idx >=0) && (edge_idx < num_edges)){
            e = HG_edges[edge_idx];
        }
        else{
            cout << "Invalid Edge index supplied" << endl;
            exit(EXIT_FAILURE);
        }
        return e;
    }

    
private:
    void findPartition(const HG_Node& starting_node,  vector<bool>& node_idx_seen, vector<bool>& edge_idx_seen, int& nodes_seen);
    int num_nodes;
    int num_edges;
    vector<HG_Edge> HG_edges;
    vector<HG_Node> HG_nodes;
    void printPartitions(vector<vector<int>> partitions);
    void identifyPartitions(const vector<int>& relaxed_edges);
    void updateNodes(const vector<double>& constraints_selected, vector<HG_Node>& updated_nodes);
    int largest_partition = 0;
    vector<Partition_Struct> PS;
    bool partitioned = false;
    bool isConstraintRedundant(HG_Edge& edge_to_check, bool print);

};


#endif