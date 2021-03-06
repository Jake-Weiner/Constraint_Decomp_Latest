#include "GreedyDecompCreator.h"
#include <random>
#include <algorithm>

  


vector<bool> GreedyDecompCreator::createGreedyDecomp(MIP_Problem& MP, const double& desired_prop)
{
    int total_nonzeroes = MP.getnumNonZero();
    int total_num_constraints = MP.getNumConstraints();

    std::vector<bool> greedy_decomp;
    //initialise
    greedy_decomp.resize(total_num_constraints, false);
    int number_relaxed_constraints = 0;
    std::random_device rd; //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, 1.0);
    int individual_idx = 0;
    int desired_number_relaxed_constraints = desired_prop * total_num_constraints;
    // cout << "running greedy decomposition in function" << endl;
    
    vector<double> constraint_probabilities;
    constraint_probabilities.resize(total_num_constraints,0.0);
    double probability_accumulative_count = 0.0;
    for (int con_idx = 0; con_idx < total_num_constraints; ++con_idx) {
        // if (MP.ch)
        Constraint con;
        if (MP.constraintIndexValidity(con_idx)){
            con = MP.getConstraint(con_idx);
        }else{
            cout << "Invalid constraint idx supplied in GreedyDecompCreator.cpp " << endl;
            exit(EXIT_FAILURE);
        }
        
        int num_non_zeroes_in_constraint = con.getNumVar();
     
        // probability accumulative count
        double constraint_probability = (static_cast<double>(num_non_zeroes_in_constraint) / static_cast<double>(total_nonzeroes));
        probability_accumulative_count += constraint_probability;
        constraint_probabilities[con_idx] = probability_accumulative_count;
        // in case of rounding errors, assign the final accumulative probability 1
        if (con_idx == total_num_constraints - 1){
            constraint_probabilities[con_idx] = 1.0;
        }
    }

    // for (int con_idx = 0; con_idx < total_num_constraints; ++con_idx) {
    //     cout << "constraint probabilities index: " << con_idx << endl;
    //     cout << constraint_probabilities[con_idx] << endl;
    // }
    // keep relaxing constraints one at a time until the desired number of constraints are relaxed
    while (number_relaxed_constraints < (desired_prop * total_num_constraints)) {

        double random_num = dis(gen);
        // search for random number in the cumulative probability vector
        std::vector<double>::iterator low;
        // low returns the index of the first element which is greater than / equal to the element to search for
        // using a binary search
        low = std::lower_bound(constraint_probabilities.begin(), constraint_probabilities.end(), random_num); //       
        int index_position = low - constraint_probabilities.begin();
        //cout << "random number: " << random_num << endl;
        //cout << "index is: " << index_position << endl;
        if(greedy_decomp[index_position] == false){
            greedy_decomp[index_position] = true;
            ++number_relaxed_constraints;
        }
    }

    return greedy_decomp;
}
