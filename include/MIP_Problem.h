#ifndef __MIP_PROBLEM__
#define __MIP_PROBLEM__

#include "Constraint.h"
#include "Variable.h"
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

enum Optimisation_Type{Minimise,Maximise};

class MIP_Problem{

    public:
        void addConstraint(Constraint constraint){
            constraints.push_back(constraint);
        };
        void addObjVarIdx(const int& var_idx){
            variables_obj_fn.push_back(var_idx);
        }
        void addObjVarCoeff(const int& var_coeff){
            coeff_obj_fn.push_back(var_coeff);
        }

        Variable getVariable(const int& index){
            if (index < variables.size() && index >= 0){
                return variables[index];
            }
            else{
                cout << "out of bounds array index for accessing Variable in MIP_Problem" << endl;
                cout << "index is -" << index << " vector size is - " << variables.size() << endl;
            }
            return NULL;
        }

        int getNumConstraints(){
            return constraints.size();
        }

        int getNumVariables(){
            return variables.size();
        }
        
        void printVariables(){
            for (auto& variable : variables){
                variable.printInfo();
            }
        }

        void printConstraints(){
            for (auto& constraint : constraints){
                constraint.printInfo();
            }
        }

        void printObjectiveFn(){
            cout << "Objective function : ";
            for (auto& obj_term : objective_fn){
                cout << obj_term.second  << "*" << obj_term.first  << " ";
            }
            cout <<endl;
        }

        vector<int> getConGreaterBounds(){
            vector<int> idxs;
            for (auto& constraint: constraints){
                if (constraint.getBoundType() == Greater){
                    idxs.push_back(constraint.getConIdx());
                }
            }
            return idxs;
        }
        vector<int> getConLesserBounds(){
            vector<int> idxs;
            for (auto& constraint: constraints){
                if (constraint.getBoundType() == Less){
                    idxs.push_back(constraint.getConIdx());
                }
            }
            return idxs;
        }
        vector<int> getConEqualBounds(){
            vector<int> idxs;
            for (auto& constraint: constraints){
                if (constraint.getBoundType() == Equal){
                    idxs.push_back(constraint.getConIdx());
                }
            }
            return idxs;
        }

        void addObjTerm(pair<int,double> term){
            objective_fn.push_back(term);
        }

        int getNumBin();
        int getNumInt();
        int getNumCont();
        int getnumNonZero();

        bool testMIPProblem(const int num_con, const int num_var, const int bin, const int cont, const int integer, const int non_zero);


        int number_variables;
        vector<Constraint> constraints;
        vector<Variable> variables;
        vector<pair<int,double>> objective_fn; 
        vector<int> variables_obj_fn;
        vector<double> coeff_obj_fn;
        Optimisation_Type OT;
    
    private:
        bool variable_types_counted = false;
        bool non_zeros_counted = false;
        int num_bin = 0;
        int num_int = 0;
        int num_cont = 0;
        int num_non_zero = 0;

        void countVarTypes();
        void countNonZeros();
};  

#endif