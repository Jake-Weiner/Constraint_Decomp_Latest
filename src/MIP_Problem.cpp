#include "MIP_Problem.h"
#include <math.h>

bool MIP_Problem::testMIPProblem(const int num_con, const int num_var, const int num_bin,
    const int num_cont, const int num_int, const int num_non_zero)
{

    bool return_val = true;

    // test number of constraints
    if (constraints.size() != num_con) {
        return_val = false;
        std::cout << "Incorrect Number of Constraints Parsed" << std::endl;
    }
    // test number of variables
    if (variables.size() != num_var) {
        return_val = false;
        std::cout << "Incorrect Number of Variables Parsed" << std::endl;
    }
    // test var types
    int this_bin = 0;
    int this_cont = 0;
    int this_int = 0;

    for (auto& var : variables) {
        if (var.getVarType() == Bin) {
            this_bin += 1;
        } else if (var.getVarType() == Cont) {
            this_cont += 1;
        } else if (var.getVarType() == Int) {
            this_int += 1;
        }
    }

    if (this_bin != num_bin) {
        return_val = false;
        std::cout << "Incorrect Number of Binary Variables Parsed" << std::endl;
        std::cout << "bin count is: " << this_bin << " and should be: " << num_bin << endl;
    }

    if (this_cont != num_cont) {
        return_val = false;
        std::cout << "Incorrect Number of Continuous Variables Parsed" << std::endl;
        std::cout << "cont count is: " << this_cont << " and should be: " << num_cont << endl;
    }

    if (this_int != num_int) {
        return_val = false;
        std::cout << "Incorrect Number of Integer Variables Parsed" << std::endl;
        std::cout << "int count is: " << this_int << " and should be: " << num_int << endl;
    }

    // number of non-zeros
    int this_non_zero = 0;

    for (auto& con : constraints) {
        this_non_zero += con.getNumVar();
    }
    if (this_non_zero != num_non_zero) {
        return_val = false;
        std::cout << "Incorrect Number of Non-Zeroes Parsed" << std::endl;
        std::cout << "nonzero count is: " << this_non_zero << " and should be: " << num_non_zero << endl;
    }

    return return_val;
}

// sets the number of variables count to the correct values and sets the boolean flag var_types_counted -> true
void MIP_Problem::countVarTypes()
{

    if (variable_types_counted == true) {
        cout << "error, already counted the number of variable types" << endl;
        return;
    }
    for (auto& var : variables) {
        if (var.getVarType() == Bin) {
            num_bin += 1;
        } else if (var.getVarType() == Cont) {
            num_cont += 1;
        } else if (var.getVarType() == Int) {
            // change var type to BOOL if UB is 1 and LB is 0 (depends on the MPS file format...)
            if (var.getLB() == 0 && var.getUB() == 1) {
                var.setVarType(Bin);
                num_bin += 1;
            } else {
                num_int += 1;
            }
        }
    }
    variable_types_counted = true;
}

void MIP_Problem::countNonZeros()
{

    if (non_zeros_counted == true) {
        cout << "error, already counted the number of non zeroes" << endl;
    }
    for (auto& con : constraints) {
        num_non_zero += con.getNumVar();
    }

    non_zeros_counted = true;
}

int MIP_Problem::getNumBin()
{

    if (variable_types_counted == false) {
        countVarTypes();
    }
    return num_bin;
}

int MIP_Problem::getNumInt()
{

    if (variable_types_counted == false) {
        countVarTypes();
    }
    return num_int;
}

int MIP_Problem::getNumCont()
{

    if (variable_types_counted == false) {
        countVarTypes();
    }
    return num_cont;
}

int MIP_Problem::getnumNonZero()
{
    if (non_zeros_counted == false) {
        countNonZeros();
    }
    return num_non_zero;
}

void MIP_Problem::calculateAverageConstraintDensity()
{
    // make sure the number of non zeroes has been counted
    if (non_zeros_counted == false) {
        countNonZeros();
    }
    average_constraint_density = double(num_non_zero) / getNumConstraints();
}

double MIP_Problem::getAverageConstraintDensity()
{

    calculateAverageConstraintDensity();
    return average_constraint_density;
}
double MIP_Problem::getStddevConstraintDensity()
{
    // stddev is sqrt(1/n sum(x_i - x_mean)^2)
    double average_constraint_density_local = getAverageConstraintDensity();
    double variance = 0.00;
    for (auto& constraint : constraints) {
        variance += pow(double(constraint.getNumVar()) - average_constraint_density_local, 2);
    }
    variance = variance / getNumConstraints();
    double standard_dev = sqrt(variance);
    return standard_dev;
}

double MIP_Problem::getEqualityConstraintProp()
{
    if (equality_constraints_counted == false) {
        setEqualityConstraintCount();
    }
    return (double(equality_constraint_count) / getNumConstraints());
}

double MIP_Problem::getInequalityConstraintProp()
{
    if (equality_constraints_counted == false) {
        setEqualityConstraintCount();
    }
    return (1 - (double(equality_constraint_count) / getNumConstraints()));
}

void MIP_Problem::setEqualityConstraintCount()
{
    if (equality_constraints_counted == false) {
        for (auto& constraint : constraints) {
            if (constraint.getBoundType() == Equal) {
                ++equality_constraint_count;
            }
        }
        equality_constraints_counted = true;
    }
}



Constraint MIP_Problem::getConstraint(const int& constraint_idx){
    Constraint ret_val;
    if (constraint_idx < getNumConstraints()){
        ret_val = constraints[constraint_idx];
    }
    else {
        cout << "Error, Attempted to Access Constraint Index from MIP_Problem which is invalid" << endl;
        exit(EXIT_FAILURE);
    }
    return ret_val;
}

// returns the sum(obj coeff) for the supplied constraint
double MIP_Problem::getConstraintSumObj(const int& constraint_idx, const bool& abs_flag){
    double sum_obj_ceoff = 0.00;
    // get the variables in the constraint
    if (constraint_idx < getNumConstraints()){
        for (const auto& variable_index : constraints[constraint_idx].getVarIndxs()){
            //search through the objective function for the variable 
            for (const auto& obj_pair : objective_fn){
                if (obj_pair.first == variable_index){
                    if (abs_flag){
                        sum_obj_ceoff += abs(obj_pair.second);
                    }
                    else{
                        sum_obj_ceoff += obj_pair.second;
                    }
                    break;
                }
            }
        }
    }
    return sum_obj_ceoff;   
}



// returns the objective coefficient of the variable if the variable is contained within the objective function

double MIP_Problem::getVarObjCoeff(const int var_idx, bool& success){

    double obj_coeff = 0.0;
    for (const auto& obj_pair : objective_fn){
        if (obj_pair.first == var_idx){
            obj_coeff = obj_pair.second;
            success = true;
            break;
        }
    }
    return obj_coeff;
}

int MIP_Problem::getEqualityConstraintCount(const std::vector<int>& edge_indexes){

    int equality_const_count = 0;
    for (const auto& edge_idx : edge_indexes){
        if(constraintIndexValidity(edge_idx)){
            if (constraints[edge_idx].getBoundType() == Equal){
                ++equality_const_count;
            }
        }   
        else{
            cout << "Invalid Constraint Index in getEqualityConstraintCount(). Index requested is: " << edge_idx << endl;
            exit(EXIT_FAILURE);
        }
    }
    return equality_const_count;
}