#ifndef __DecompMIP__
#define __DecompMIP__

#include <vector>
#include <string>
#include "HG_Node.h"
#include "HG_Edge.h"
#include <ilcplex/ilocplex.h>

typedef IloArray<IloNumVarArray> NumVarMatrix;

using namespace std;


struct DecompInfo{
    vector<int> constraints_relaxed;
    int largest_subproblem;
};

class DecompMIP {
    public:
    DecompMIP(){};
    DecompInfo solveMIP(vector<HG_Edge> HG_edges, vector<HG_Node> HG_Nodes, int largest_subproblem, 
    int num_subproblems, bool print = false, const char* outfile = "", const int solver_time = 300,  bool warm_start=false, const char* warm_start_file = "");
    
    private:
    vector<bool> z; // constraint relaxation array
    NumVarMatrix x; // variable matrix
    

};

#endif