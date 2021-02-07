#include "ConstraintFileProcessing.h"
#include "Util.h"
#include "RelaxedConstraintRedundancyChecker.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <unordered_map>

using std::cout;
using std::endl;
using std::string;

void ConstraintFileProcessing::removeRedundantConstraints(const string& input_file, const string& output_file, Hypergraph& HG)
{
    cout << "running constraint redundancy testing" << endl;

    // open up input file with decompositons
    std::ifstream input_fs(input_file);
    std::ofstream output_fs(output_file);
    // vector of relaxed constraints read in via input file

    RelaxedConstraintRedundancyChecker rcrc;
    // input file successfully opened
    if (input_fs) {
        string line_read;
        while (getline(input_fs, line_read)) {
            vector<string> relaxed_constraints_str;
            vector<int> relaxed_constraints_int;
            // split the line based on ,
            boost::split(relaxed_constraints_str, line_read, boost::is_any_of(","), boost::token_compress_on);
            // first line contains the number of nodes
            // last element will be empty because of ending final comma
            for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
                relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
            }
            // new decomp with redundant constraints removed
            // get 
            vector<int> new_relaxed_constraints = rcrc.removeRedundantConstraints(relaxed_constraints_int, HG);

            // if all constraints are redundant for the current decomposition then do not
            if (new_relaxed_constraints.empty()){
                cout << "all constraints are redundant" << endl;
            }
            else{
                // write out new deompositions to output file
                if (output_fs) {
                    for (const auto& con_val : new_relaxed_constraints) {
                        output_fs << con_val << ",";
                    }
                    output_fs << endl;
                }
            }
        }
    }

    else {
        cout << "redundant constraint input file unable to be found/opened" << endl;
    }
}

void ConstraintFileProcessing::removeDuplicateConstraints(const string& input_file, const string& output_file)
{
    cout << "running constraint duplicate testing" << endl;

    // hashmap to determine unique constraint vectors
    std::unordered_map<vector<int>, bool, container_hash<vector<int>>> decomp_map;
    // open up input file with decompositons
    std::ifstream input_fs(input_file);
    std::ofstream output_fs(output_file);
    // vector of relaxed constraints read in via input file
    cout << "Input file is " << input_file << endl;
    cout << "Output file is " << output_file << endl;
    // input file successfully opened
    if (input_fs) {
        string line_read;
        while (getline(input_fs, line_read)) {
            vector<string> relaxed_constraints_str;
            vector<int> relaxed_constraints_int;
            // split the line based on ,
            boost::split(relaxed_constraints_str, line_read, boost::is_any_of(","), boost::token_compress_on);
            // first line contains the number of nodes
            // last element will be empty because of ending final comma
            for (int i = 0; i < relaxed_constraints_str.size() - 1; ++i) {
                relaxed_constraints_int.push_back(stoi(relaxed_constraints_str[i]));
            }
            // decomp has not been seen before
            if (decomp_map.find(relaxed_constraints_int) == decomp_map.end()) {
                decomp_map[relaxed_constraints_int] = true;
            }
        }
    }
    else {
        cout << "redundant constraint input file unable to be found/opened" << endl;
    }

    // write unique decompositions to output file
    if (output_fs) {
        for (const auto& element : decomp_map) {
            for (const auto& con_idx : element.first) {
                output_fs << con_idx << ",";
            }
            output_fs << endl;
        }
    }
    else{
        cout << "unique decomposition output file unable to be found/opened" << endl;
    }
}
