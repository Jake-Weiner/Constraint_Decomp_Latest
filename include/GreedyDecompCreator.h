#ifndef __GREEDY_DECOMP_CREATOR__
#define __GREEDY_DECOMP_CREATOR__

#include "MIP_Problem.h"
#include <vector>

using std::vector;

class GreedyDecompCreator{
    public:
        GreedyDecompCreator(){};
        vector<bool> createGreedyDecomp(MIP_Problem& MP,const double& desired_prop);
    private:

        
};


#endif