#ifndef __MIP_IDX_INFO__
#define __MIP_IDX_INFO__

#include <vector>
#include <string>
#include "HG_Node.h"
#include "HG_Edge.h"
#include <unordered_map>
#include <fstream>
#include <iostream>

using namespace std;

struct MIP_Indexing_Info{
    //insert hashmaps for mps info 
    unordered_map<int,string> idx_to_constraint;
    unordered_map<string,int> constraint_to_idx;
    unordered_map<int,string> idx_to_var;
    unordered_map<string, int> var_to_idx;

    
    bool constraintNameExists(const string& name){
        if (constraint_to_idx.find(name) != constraint_to_idx.end()){
            return true;
        }
        return false;
    }

    bool varNameExists(const string& name){
        if (var_to_idx.find(name) != var_to_idx.end()){
            return true;
        }
        return false;
    }

    string getConstraintName(int idx){
        if (idx_to_constraint.find(idx) != idx_to_constraint.end()){
            return idx_to_constraint[idx];
        }
        else{
            cout << "constraint idx does not exist" << endl;
        }
        return "";
    };
    string getVariableName(int idx){
        if (idx_to_var.find(idx) != idx_to_var.end()){
            return idx_to_var[idx];
        }
        else{
            cout << "variable idx does not exist" << endl;
        }
        return "";
    }
    int getConstraintIdx(string constraint){
         if (constraint_to_idx.find(constraint) != constraint_to_idx.end()){
            return constraint_to_idx[constraint];
        }
        else{
            cout << "constraint name does not exist" << endl;
        }
        return -1;
    }
    int getVariableIdx(string variable){
          if (var_to_idx.find(variable) != var_to_idx.end()){
            return var_to_idx[variable];
        }
        else{
            cout << "variable name does not exist" << endl;
        }
        return -1;
    }

    void addConstraintName(string name, int idx){

        if (constraint_to_idx.find(name) == constraint_to_idx.end()){
            constraint_to_idx[name] = idx ;
        }
        else{
            cout << "constraint name already exists - MPS_Info.h" << endl;
        }
    }
    void addVariableName(string name, int idx){
        if (var_to_idx.find(name) == var_to_idx.end()){
            var_to_idx[name] = idx ;
        }
        else{
            cout << "variable name already exists - MPS_Info.h" << endl;
        }
    }

    void addConstraintIdx(int idx, string name){
        if (idx_to_constraint.find(idx) == idx_to_constraint.end()){
            idx_to_constraint[idx] = name;
        }
        else{
            cout << "constraint idx already exists - MPS_Info.h" << endl;
        }
    }
    void addVariableIdx(int idx, string name){
        if (idx_to_var.find(idx) == idx_to_var.end()){
            idx_to_var[idx] = name ;
        }
        else{
            cout << "variable idx already exists - MPS_Info.h" << endl;
        }
    }
};

#endif