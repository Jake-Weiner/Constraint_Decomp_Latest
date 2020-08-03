#ifndef Convert_To_Decomposed_MIP_H
#define Convert_To_Decomposed_MIP_H

#include <vector>
#include "ConDecomp_LaPSO_Connector.h"


using namespace std;

struct subproblem_mip{
  vector<int> variable_idxs;
  unordered_map<int,string> idx_to_constraint;
};




class Convert_To_Decomposed_MIP{

    public:
        Convert_To_Decomposed_MIP(){};
        void convertPartition();

    private:
        

};

#endif