#include "Util.h"
#include <iostream>
#include <fstream>

using std::ifstream;

bool fileExists (const std::string& filename) {
    ifstream f(filename.c_str());
    return f.good();
}

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