#include "ConstraintFileProcessing.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>

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
                cout << "constraint idx is " << relaxed_constraints_str[i] << endl;
            }
            // new decomp with redundant constraints removed
            vector<int> new_relaxed_constraints = HG.removeRelaxedConstraintRedundancies(relaxed_constraints_int);

            // write out new deompositions to output file
            if (output_fs) {
                for (const auto& con_val : new_relaxed_constraints) {
                    output_fs << con_val << ",";
                }
                output_fs << endl;
            }
        }
    }

    else {
        cout << "redundant constraint input file unable to be found/opened" << endl;
    }
}

void ConstraintFileProcessing::removeDuplicateConstraints(const string& input_file, const string& output_file)
{
     cout << "running constraint redundancy testing" << endl;

    // open up input file with decompositons
    std::ifstream input_fs(input_file);
    std::ofstream output_fs(output_file);
    // vector of relaxed constraints read in via input file

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
           
           

            // write out new deompositions to output file
            if (output_fs) {
                for (const auto& con_val : new_relaxed_constraints) {
                    output_fs << con_val << ",";
                }
                output_fs << endl;
            }
        }
    }

    else {
        cout << "redundant constraint input file unable to be found/opened" << endl;
    }
}
