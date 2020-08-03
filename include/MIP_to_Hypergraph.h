#ifndef __MIP_to_Hypergraph__
#define __MIP_to_Hypergraph__

#include <vector>
#include <string>
#include "MIP_Problem.h"
#include "HG_Node.h"
#include "HG_Edge.h"
#include "MIP_Indexing_Info.h"


class MIP_to_Hypergraph{
    public:

        MIP_to_Hypergraph(){};
        vector<HG_Edge> getHGEdges(){
            return HG_edges;
        };
        vector<HG_Node> getHGNodes(){
            return HG_nodes;
        };
        void convertToHypergraph(MIP_Problem& MP);
        
    private:

        
        void printPartitions(vector<vector<int>> partitions);
        void setNumEdges(int num_edges);
        void setHGEdgesSize(int size);
        void setHGNodesSize(int size);
        void initNodes(int size);
        int num_nodes;
        int num_edges;
        vector<HG_Edge> HG_edges;
        vector<HG_Node> HG_nodes;
};

#endif