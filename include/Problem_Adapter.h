
/* This class interacts with the pagmo::problem class to create and feedback the nsga decompositions created
*/

#ifndef __Problem_Adapter__
#define __Problem_Adapter__


#include <pagmo/algorithm.hpp>
#include <pagmo/algorithms/nsga2.hpp>
#include <pagmo/archipelago.hpp>
#include <pagmo/problem.hpp>
#include <pagmo/utils/multi_objective.hpp>
#include <pagmo/utils/hypervolume.hpp>
#include <vector>
#include <algorithm>
#include "Util.h"
#include "Hypergraph.h"

struct NSGAParameters{
    int num_gen;
    int pop_size;
};

struct NSGAOutputs{
    const string all_decomps_output_filepath;
    const string pareto_decomps_output_filepath;
};

class ProblemAdapter{

    public:
        ProblemAdapter();
        void createNSGADecomps(Hypergraph& HG,
                                const NSGAParameters& nsga_params,
                                const NSGAOutputs& output_files,
                                const bool& print_objectives);
};

#endif
