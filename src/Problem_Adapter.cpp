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

void writeDecompToFile(ofstream& outfile, const vector<double>& con_vec);

Problem_Adapter::Problem_Adapter(){};

void Problem_Adapter::createNSGADecomps(Hypergraph& HG, const int& num_gen,
    const string& output_file, const int& pop_size)
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
                writeDecompToFile(outfile, pop.get_x()[pop_idx]);
                // unsigned int largest_subproblem = int(floor(pop.get_f()[pop_idx][largest_sp_idx] + 0.1));
                // unsigned int number_constraints_relaxed = int(floor(pop.get_f()[pop_idx][num_con_relaxed_idx] + 0.1));
                // pair_int map_key = std::make_pair(largest_subproblem, number_constraints_relaxed);
                // // if key doesn't exist, at it to the map and write out the individual to the file
                // if (uo_map.find(map_key) == uo_map.end()){
                //     uo_map[map_key] = true;
                     // write out each individual to the required file
                    
                
            }
            // evolve the population
            pop = algo.evolve(pop); 
            ++gen_counter;
        }
        outfile.close();
    }
    else{
        cout << "NSGA Decomp Output file unable to be created" << endl;
    }
}

// write out NSGA decompositions as the constraint indexes relaxed to the file specified 
void writeDecompToFile(ofstream& outfile, const vector<double>& con_vec){
    
    cout << "writing decomp to file" << endl;
    for (int i = 0; i< con_vec.size(); ++i){
        // in case of rounding errors, cast to an int
        if (static_cast<int>(con_vec[i] + 0.1) == 1){
            outfile << i << ",";
        }
    }
    outfile << endl;
}