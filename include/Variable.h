#ifndef __VARIABLE__
#define __VARIABLE__

#include <vector>
#include <iostream>
#include <limits>

using namespace std;

enum Variable_Type{Bin, Cont, Int};

const double default_variable_upper_bound = 100000000.00;

class Variable{
    public:
        Variable(){};
        Variable(int variable_idx){
            this->variable_idx = variable_idx;
        };
        Variable(int variable_idx, Variable_Type vt){
            this->variable_idx = variable_idx;
            this->vt = vt;
            if (vt == Int || vt == Cont){
                upper_bound = default_variable_upper_bound;
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

        void setMaxUB(){
            if(vt == Int || vt == Cont){
                upper_bound = default_variable_upper_bound;
            }
        }

        // set LB
        void setMinLB(){
            if(vt == Int || vt == Cont){
                lower_bound = -default_variable_upper_bound;
            }
        }

        double getUB() const {
            return upper_bound;
        }

        void setLB(double bound){
            lower_bound = bound;
        }

        double getLB() const {
            return lower_bound;
        }
        
        // for type, bin = 0, cont = 1, int = 2
        void printInfo() const{
            cout << "index = " << variable_idx << " : upper bound = " << upper_bound << " : lower bound = " << lower_bound
            << " bound type = " << vt << endl; 
        }

        inline bool operator< ( const Variable& rhs ) const { 
            return variable_idx < rhs.getVarIndx(); 
        };

        inline bool operator> ( const Variable& rhs ) const { 
            return variable_idx > rhs.getVarIndx();  
        };

        const Variable_Type getVarType() const {
            return vt;
        }

        void setVarType(Variable_Type vt){
            this->vt = vt;
        }

    private:
        double upper_bound = 1.00;
        double lower_bound = 0.00;
        int variable_idx;
        Variable_Type vt; 
        
};

#endif