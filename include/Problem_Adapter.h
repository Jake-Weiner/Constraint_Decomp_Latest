
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


class Problem_Adapter{

    public:

        Problem_Adapter();

        void createNSGADecomps(Hypergraph& HG, const int& num_gen, 
        const string& output_file, const string& pareto_optimal_sols, const int& pop_size, const bool& print_objective);
};

#endif
