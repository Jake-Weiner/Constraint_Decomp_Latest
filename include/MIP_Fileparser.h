#ifndef __MIP_FILEPARSER__
#define __MIP_FILEPARSER__

#include <vector>
#include <string>
#include "MIP_Problem.h"
#include "HG_Node.h"
#include "HG_Edge.h"
#include "MIP_Indexing_Info.h"
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;

enum file_type{USER, MPS};

enum Bounds_type{Upper,Lower, Bool, Fix, Free, FreeNegative};



void trim(string& s);

class MIP_Fileparser {
public:
    
    MIP_Fileparser(){};

    bool constraintNumTest(const int test_num){
        if (test_num == MP.constraints.size()){
            return true;
        }
        else{
            return false;
        }
    }

    void printConstraints();
    void printVariables();
    void parse(file_type ft, string filename);
    MIP_Problem getMIPProblem(){
        return MP;
    }
    MIP_Indexing_Info getMIP_Indx_Info(){
        return MII;
    }
    
private:
  
    bool constraintCheck(HG_Edge constraint_to_check);
    void parserUser(string filename);
    void parserMps(string filename);
    void createConstraint(const vector<string>& line_split,unsigned int& current_constraint_number);
    void createVariable(const vector<string>& line_split,unsigned int& current_constraint_number);
    bool constraintInfoCheck(const string& word);
    void extractVariableInfo(vector<string>& line_split, const string& variable_name);
    void extractRHSInfo(const vector<string>& line_split);
    void extractBoundsInfo(const vector<string>& line_split);
    string obj_function_symbol;
    MIP_Problem MP;
    MIP_Indexing_Info MII;
        


};


#endif