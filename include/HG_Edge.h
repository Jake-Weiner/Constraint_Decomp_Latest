#ifndef __HG_Edge__
#define __HG_Edge__


#include <vector>

using namespace std;



class HG_Edge {
public:
    HG_Edge(){};
    HG_Edge(int edge_idx, vector<int> node_idxs){
        this->edge_idx = edge_idx;
        this->node_idxs = node_idxs;
    };
    HG_Edge(int edge_idx){
        this->edge_idx = edge_idx;
    };
    int getEdgeIdx() const;
    vector<int> getNodeIdxs() const;
    int getNumNodes() const;
    
    void printNodes();
    void addNodeIdx(int node_idx);
    double getUpperBound(){
        if (upperbound_initialised == true){
            return upperbound;
        }
        return 99999999999999999;
    };

    double getLowerBound(){
        if (lowerbound_initialised == true){
            return lowerbound;
        }
        return -99999999999999999;
    };
    
    inline bool operator< ( const HG_Edge& rhs ) const { 
        return node_idxs.size() < rhs.node_idxs.size(); 
    };

    inline bool operator> ( const HG_Edge& rhs ) const { 
        return node_idxs.size() > rhs.node_idxs.size(); 
    };

private:
    int edge_idx;
    vector<int> node_idxs;
    double upperbound;
    double lowerbound;
    bool upperbound_initialised = false;
    bool lowerbound_initialised = false;
};


#endif