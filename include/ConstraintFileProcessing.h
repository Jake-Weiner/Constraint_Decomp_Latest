#ifndef __CONSTRAINTFILEPROCESSING__
#define __CONSTRAINTFILEPROCESSING__

#include "Hypergraph.h"
#include <string>
using std::string;

class ConstraintFileProcessing{


    public:
    ConstraintFileProcessing(){};

    void removeRedundantConstraints(const string& input_file, const string& output_file, Hypergraph& HG);
    void removeDuplicateConstraints(const string& input_file, const string& output_file);

    private:

};


#endif