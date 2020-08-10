#include "Problem_Adapter.h"
#include "Decomp.hpp"

#include <algorithm>

using pagmo::algorithm;
using pagmo::Decomp;
using pagmo::nsga2;
using pagmo::population;
using std::vector;

#define largest_sp_idx 0
#define num_con_relaxed_idx 1

Problem_Adapter::Problem_Adapter(){};

vector<individual_information_struct> Problem_Adapter::createNSGADecomps(Hypergraph& HG, const int& num_gen,
    const vector<double>& subproblem_var_props, const int& pop_size)
{
    vector<individual_information_struct> ris;
    Decomp udp = Decomp(HG.getNumEdges(), HG);
    pagmo::problem prob{udp};

    // single instance
    //algorithm is set to run for 1 generation
    algorithm algo{nsga2(1)};
    // print level set to 1
    algo.set_verbosity(1);
    population pop;
    population pop_total;
    // Greedy Seeding of the initial pop
    bool greedy = true;
    if (greedy == true) {
        // there are currently 14 greedily seeded individuals, therefore population size must be at least 14 to accomodate these greedy individuals
        if (pop_size > 14) {
            vector<vector<double>> greedy_population = udp.greedy_seeding();
            int pop_size_wo_greedy = pop_size - greedy_population.size();
            population pop_greedy{ prob, pop_size_wo_greedy };
            for (auto& individual : greedy_population) {
                pop_greedy.push_back(individual);
            }
            pop = pop_greedy;
        }
    } else {
        population non_greedy{ prob, pop_size };
        pop = non_greedy;
    }
    //
    cout << "initialised initial population" << endl;

    pop_total = pop;
    // capture the inital population before any evolution
    // for (int pop_idx = 0; pop_idx < pop.size(); pop_idx++) {
    //     pop_total.push_back(pop.get_x()[pop_idx], pop.get_f()[pop_idx]);
    // }

    cout << "captured initial population" << endl;
    cout << "starting to evolve population" << endl;
    // evolving the population 1 generation at a time in order to capture all solutions (placed into pop_total) that would otherwise be thrown away
    int gen_counter = 0;
    while (gen_counter < (num_gen - 1)) {
        cout << "number of generations evolved is " << gen_counter << endl;
        algorithm algo_temp{nsga2(1)};
        pop = algo.evolve(pop);
        for (int idx = 0; idx < pop.size(); idx++) {
            pop_total.push_back(pop.get_x()[idx], pop.get_f()[idx]);
        }
        ++gen_counter;
    }

    // pop total can include multiple copies of solutions... Need to find a way to filter these out.
    // if both LSP and No.Constraints relaxed are equal, assume the con vecs are the same

    // Evolve population 1 generation at a time to capture more solutions
    // pop = algo.evolve(pop);

    for (int pop_idx = 0; pop_idx < pop_total.size(); pop_idx++) {
        vector<double> con_vec = pop_total.get_x()[pop_idx];
        unsigned int largest_subproblem = int(floor(pop_total.get_f()[pop_idx][largest_sp_idx] + 0.1));
        unsigned int number_constraints_relaxed = int(floor(pop_total.get_f()[pop_idx][num_con_relaxed_idx] + 0.1));
        individual_information_struct ind = { con_vec, largest_subproblem, number_constraints_relaxed };
        ris.push_back(ind);
    }

    return ris;

    // vector<vector<double>> f_vals = pop_total.get_f();
    // vector<vector<double>> x_vals = pop_total.get_x();

    // plot the population to see the different decompositions available

    // Need to - get all decompositions, calculate fronts, sort by front number, give relaxation number to each relaxation

    // what do I need to store - constraint vec - largest_sp - front number

    // if required subproblem proportions are provided, return these to print
    // if (!subproblem_var_props.empty()) {
    //     int relaxation_num = 0;
    //     for (auto& subproblem_var_prop : subproblem_var_props) {
    //         int largest_sub_limit = subproblem_var_prop * HG.getNumNodes();
    //         int largest_subproblem;
    //         int best_index = 0;
    //         int smallest_num_con_relaxed = HG.getNumEdges();
    //         int idx = 0;
    //         bool prop_found = false;
    //         for (auto& obj_val : pop.get_f()) {
    //             if (obj_val[0] <= largest_sub_limit) {
    //                 prop_found = true;
    //                 if (obj_val[1] < smallest_num_con_relaxed) {
    //                     best_index = idx;
    //                     smallest_num_con_relaxed = obj_val[1];
    //                     // largest_subproblem = obj_val[0];
    //                 }
    //             }
    //             idx++;
    //         }
    //         // To Do - calculate Front Number
    //         // if (prop_found){
    //         //     ris.push_back({relaxation_num, x_vals[best_index], (float(f_vals[best_index][0]) / float(HG.getNumNodes())), });
    //         // }
    //     }
    // }
    // // if no subproblem props are given, return all in first front
    // else {
    //     // To Do - calculate Front Number
    //     // for (int pop_idx = 0; pop_idx < pop.size(); pop_idx++){
    //     //     ris.push_back({relaxation_num, x_vals[pop_idx], (f_vals[pop_idx][0] / HG.getNumNodes())});
    //     // }
    // }
    // return ris;
}
