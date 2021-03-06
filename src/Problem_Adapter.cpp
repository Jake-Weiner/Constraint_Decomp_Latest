#include "Problem_Adapter.h"
#include "Decomp.hpp"
#include "fstream"
#include <map>
#include <utility>

#include <algorithm>

using pagmo::algorithm;
using pagmo::Decomp;
using pagmo::nsga2;
using pagmo::population;
using std::vector;

using std::ostream;

#define largest_sp_idx 0
#define num_con_relaxed_idx 1

typedef std::pair<unsigned int,unsigned int> pair_int;

void writeDecompToFile(ofstream& outfile, const vector<double>& con_vec, double con_relaxed, double LSP, bool print_objectives);
void writeParetoOptimalToFile(const string& outfile_name, const population& pop, const vector<size_t>& pareto_idxs);

Problem_Adapter::Problem_Adapter(){};

void Problem_Adapter::createNSGADecomps(Hypergraph& HG, const int& num_gen,
    const string& output_file, const string& pareto_optimal_sols, const int& pop_size, const bool& print_objectives)
{
    // vector<individual_information_struct> ris;
    Decomp udp = Decomp(HG.getNumEdges(), HG);
    pagmo::problem prob{udp};

    // single instance
    //algorithm is set to run for 1 generation
    algorithm algo{nsga2(1)};
    // print level set to 1
    algo.set_verbosity(1);
    population pop;
    //population pop_total;
    // Greedy Seeding of the initial pop

    // pop size must be at least 14 to greedily seed, although it must be in multiples
    // of 4
    bool greedy = true;
    if (pop_size > 14 && greedy == true){
        vector<vector<double>> greedy_population = udp.greedy_seeding();
        int pop_size_wo_greedy = pop_size - greedy_population.size();
        population pop_greedy{ prob, pop_size_wo_greedy };
        for (auto& individual : greedy_population) {
            pop_greedy.push_back(individual);
        }
        pop = pop_greedy;
    }
    else{
        population non_greedy{ prob, pop_size };
        pop = non_greedy;
    }

    cout << "initialised initial population" << endl;
    cout << "starting to evolve population" << endl;

    // Decompositions are written out to a file in case the process is interrupted on MASSIVE
    // if file doesn't exist, create it 
    std::ofstream outfile;
    outfile.open(output_file, std::ofstream::app);

    if(outfile){
        // evolving the population 1 generation at a time in order to capture all solutions (placed into pop_total) that would otherwise be thrown away
        int gen_counter = 0;
        while (gen_counter < (num_gen - 1)) {
            cout << "number of generations evolved is " << gen_counter << endl;
            for (int pop_idx = 0; pop_idx < pop.size(); pop_idx++) {
                // write every individual to the output file, duplicates can be processed later
                writeDecompToFile(outfile, pop.get_x()[pop_idx], pop.get_f()[pop_idx][0], pop.get_f()[pop_idx][1], print_objectives);
            }
            // evolve the population
            pop = algo.evolve(pop);
            std::vector<size_t> non_dom_sols = pagmo::non_dominated_front_2d(pop.get_f());
            cout << "printing out non-dominated sol idxs" << endl;
            for (const auto& non_dom_idx : non_dom_sols){
                cout << non_dom_idx << " " ;
            }
            cout << endl;
            writeParetoOptimalToFile(pareto_optimal_sols, pop, non_dom_sols);
            ++gen_counter;
            //pareto optimal convergence... 
        }
        outfile.close();
    }
    else{
        cout << "NSGA Decomp Output file unable to be created" << endl;
    }
}

// write out NSGA decompositions as the constraint indexes relaxed to the file specified 
void writeDecompToFile(ofstream& outfile, const vector<double>& con_vec, double LSP, double con_relaxed, bool print_objectives){

    cout << "writing decomp to file" << endl;
    for (int i = 0; i< con_vec.size(); ++i){
        // in case of rounding errors, cast to an int
        if (static_cast<int>(con_vec[i] + 0.1) == 1){
            outfile << i << ",";
        }
    }

    if (print_objectives){
        outfile << "," << con_relaxed << "," << LSP;
    }
    outfile << endl;
}


void writeParetoOptimalToFile(const string& outfile_name, const population& pop, const vector<size_t>& pareto_idxs){
    // append to the end of the file

    std::ofstream outfile;
    outfile.open(outfile_name, std::ofstream::app);
    cout << "writing pareto optimal sols" << endl;
    cout << outfile_name << endl;
    if(outfile){
        cout << "writing pareto optimal sols" << endl;
        for (const auto& pop_pareto_idx : pareto_idxs){
            int pareto_idx_int = static_cast<int>(pop_pareto_idx);
            for (int i = 0; i< pop.get_x()[pareto_idx_int].size(); ++i){
        // in case of rounding errors, cast to an int
                if (static_cast<int>(pop.get_x()[pareto_idx_int][i] + 0.1) == 1){
                    outfile << i << ",";
                }
            }
            // separate pareto individuals by a space
            outfile << " ";
        }
        outfile << endl;
    }
}