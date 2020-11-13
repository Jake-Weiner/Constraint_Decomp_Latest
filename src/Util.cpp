#include "Util.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>

using std::cout;
using std::endl;
using std::ifstream;

bool fileExists(const std::string& filename)
{
    ifstream f(filename.c_str());
    return f.good();
}

// read in all decompositions from a file
vector<vector<int>> readInConVecFromFile(const std::string& filename)
{

    // open up input file with decompositons
    std::ifstream input_fs(filename);

    vector<vector<int>> decompositions_read;
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
            decompositions_read.push_back(relaxed_constraints_int);
        }
    }
    return decompositions_read;
}

// // this is only for testing purposes, read in dual values of 0
// vector<double> readInDualsFromFile(const std::string& filename){

//     vector<double> dual_vec;

//     ifstream input_fs(filename);

//     // File structure of von vec decomp_idx, con vec
//     if (input_fs) {
//         string line_read;
//         while (std::getline(input_fs, line_read)) {
//             // input_fs.get(character_read);
//             // cout << "string read: " << line_read << endl;
//             // convert char to int (decimal 48 is 0, 49 is 1 )
//             for (auto& char_in_string : line_read) {
//                 int character_read_as_int = (int)char_in_string - 48;
//                 if (character_read_as_int >=0 ) {
//                     con_vec.push_back(true);
//                 } else if (character_read_as_int == 0) {
//                     con_vec.push_back(false);
//                 } else {
//                     cout << "incorrect character read in in con vec in filename:" << filename << endl;
//                     cout << "character is: " << char_in_string << endl;
//                     exit(EXIT_FAILURE);
//                 }
//             }
//         }
//     }

//     return con_vec;

// }



// vector<bool> readInConVecFromFile(const std::string& input_filename){

//     vector<bool> con_vec;
//     ifstream input_fs(input_filename);

//     // File structure of von vec decomp_idx, con vec
//     if (input_fs){

//         getline(fin, line);

//         // used for breaking words
//         stringstream s(line);

//         // read every column data of a row and
//         // store it in a string variable, 'word'
//         while (getline(s, word, ', ')) {

//             // add all the column data
//             // of a row to a vector
//             row.push_back(word);
//         }
//     }

// }