#ifndef __VARIABLE__
#define __VARIABLE__

#include <vector>
#include <iostream>
#include <limits>

using namespace std;

enum Variable_Type{Bin,Cont, Int};

class Variable{

    public:
        Variable(){};
        Variable(int variable_idx){
            this->variable_idx = variable_idx;
        };
        Variable(int variable_idx, Variable_Type vt){
            this->variable_idx = variable_idx;
            this->vt = vt;
            if (vt == Int){
                upper_bound = std::numeric_limits<int>::max();
                lower_bound = 0;
            }
            else if(vt == Cont){
                upper_bound = std::numeric_limits<double>::max();
                lower_bound = 0.00;
            }
        }
        ~Variable(){};
        
        void setVarIdx(int idx){
            variable_idx = idx;
        }

        int getVarIndx() const {
            return variable_idx;
        }

        void setUB(double bound){
            upper_bound = bound;
        }

        double getUB(){
            return upper_bound;
        }

        void setLB(double bound){
            lower_bound = bound;
        }

        double getLB(){
            return lower_bound;
        }

        void printInfo(){
            cout << "index = " << variable_idx << " : upper bound = " << upper_bound << " : lower bound = " << lower_bound << endl; 
        }

        inline bool operator< ( const Variable& rhs ) const { 
            return variable_idx < rhs.getVarIndx(); 
        };

        inline bool operator> ( const Variable& rhs ) const { 
            return variable_idx > rhs.getVarIndx();  
        };

        const Variable_Type getVarType(){
            return vt;
        }

        void setVarType(Variable_Type vt){
            this->vt = vt;
        }
        
    private:
        double upper_bound = 1;
        double lower_bound = 0;
        int variable_idx;
        Variable_Type vt; 
};

#endif