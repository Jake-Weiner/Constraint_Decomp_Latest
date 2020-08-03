#include "HG_Edge.h"
#include <iostream>
using namespace std;



vector<int> HG_Edge::getNodeIdxs(){
    return node_idxs;
}

int HG_Edge::getEdgeIdx(){
    return edge_idx;
}

int HG_Edge::getNumNodes(){
    return node_idxs.size();
}

void HG_Edge::printNodes(){
    cout << "Edge Contains Nodes - "; 
    for (auto& node : node_idxs){
        cout << node << " ";
    }
    cout << endl;
}

void HG_Edge::addNodeIdx(int node_idx_to_add){
    
    // if node already exists return
    for (auto& node_val : node_idxs){
        if (node_val == node_idx_to_add){
            return;
        }
    }
    node_idxs.push_back(node_idx_to_add);
}