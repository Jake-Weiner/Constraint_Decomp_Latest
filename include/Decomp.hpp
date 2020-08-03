/* Copyright 2017-2018 PaGMO development team

This file is part of the PaGMO library.

The PaGMO library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 3 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The PaGMO library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the PaGMO library.  If not,
see https://www.gnu.org/licenses/. */

#ifndef PAGMO_PROBLEM_Decomp_HPP
#define PAGMO_PROBLEM_Decomp_HPP

#include <string>
#include <utility>

#include <pagmo/detail/visibility.hpp>
#include <pagmo/problem.hpp>
#include <pagmo/types.hpp>

#include "Hypergraph.h"

namespace pagmo {

class Decomp {
    /// Constructor from dimension
    /**
     * Constructs a Decomposition problem
     *
     * @param dim the problem dimensions.
     *
     * @throw std::invalid_argument if \p dim is < 1
     */
    public:
    Decomp(){};
    Decomp(int dim, const Hypergraph& hypergraph);
    // Fitness computation
    vector_double fitness(const vector_double &dv) const;
    // Box-bounds
    std::pair<vector_double, vector_double> get_bounds() const;
    /// Problem name
    /**
         * @return a string containing the problem name
         */
    std::string get_name() const
    {
        return "Decomp Function";
    }
//    vector_double::size_type get_nix();

    // Optimal solution
    vector_double best_known() const;
    // Object serialization
    template <typename Archive>
    void serialize(Archive &, unsigned);
    /// Problem dimensions
    int m_dim;
    mutable Hypergraph HG;
    vector_double::size_type get_nix() const;
    // void updateFitness();
    // void updateHG(const vector_double &dv){
    //     HG.partitionWithRelaxation(dv);
    // };
    vector_double::size_type get_nobj() const{
        return 2;
    }
    vector<vector<double>> greedy_seeding();
    // vector_double::size_type get_nic() const{
    //     return 1;
    // }
    private: 
        int num_nodes;
        int num_edges;
};

} // namespace pagmo

#endif
