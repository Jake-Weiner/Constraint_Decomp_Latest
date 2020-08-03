#include "MIP_Problem.h"

bool MIP_Problem::testMIPProblem(const int num_con, const int num_var, const int num_bin, 
const int num_cont, const int num_int, const int num_non_zero){

bool return_val = true;

    // test number of constraints
    if (constraints.size() != num_con){
        return_val = false;
        std::cout << "Incorrect Number of Constraints Parsed" << std::endl;
    }
    // test number of variables
    if (variables.size() != num_var){
        return_val = false;
        std::cout << "Incorrect Number of Variables Parsed" << std::endl;
    }
    // test var types
    int this_bin = 0;
    int this_cont = 0;
    int this_int = 0;
    
    for (auto& var: variables){
        if (var.getVarType() == Bin){
            this_bin += 1;
        }
        else if(var.getVarType() == Cont){
            this_cont += 1;
        }
        else if(var.getVarType() == Int){
            this_int += 1;
        }
    }

    if (this_bin != num_bin){
        return_val = false;
        std::cout << "Incorrect Number of Binary Variables Parsed" << std::endl;
        std::cout << "bin count is: " << this_bin << " and should be: " << num_bin << endl;
    }

    if (this_cont != num_cont){
        return_val = false;
        std::cout << "Incorrect Number of Continuous Variables Parsed" << std::endl;
        std::cout << "cont count is: " << this_cont << " and should be: " << num_cont << endl;
    }

    if (this_int != num_int){
        return_val = false;
        std::cout << "Incorrect Number of Integer Variables Parsed" << std::endl;
        std::cout << "int count is: " << this_int << " and should be: " << num_int << endl;
    }

    // number of non-zeros
    int this_non_zero = 0;

    for (auto& con: constraints){
        this_non_zero += con.getNumVar();
    }
    if (this_non_zero != num_non_zero){
        return_val = false;
        std::cout << "Incorrect Number of Non-Zeroes Parsed" << std::endl;
        std::cout << "nonzero count is: " << this_non_zero << " and should be: " << num_non_zero << endl;
    }

    return return_val;
}
