#include "MIP_Problem_Probe.h"

void MIP_Problem_Probe::populateInstanceStatistics(NSGA_ii_instance_statistics& nis, MIP_Problem& MP)
{
    nis.bin_prop = double(MP.getNumBin()) / double(MP.getNumVariables());
    nis.int_prop = double(MP.getNumInt()) / double(MP.getNumVariables());
    nis.cont_prop = double(MP.getNumCont()) / double(MP.getNumVariables());
    nis.average_constraint_density = MP.getAverageConstraintDensity();
    nis.stddev_constraint_density = MP.getStddevConstraintDensity();
    nis.equality_constraints_prop = MP.getEqualityConstraintProp();
    nis.inequality_constraints_prop = MP.getInequalityConstraintProp();
}

void MIP_Problem_Probe::populateRelaxedConstraintsStatistics(const int& decomposition_idx, const individual_information_struct& decomp, NSGA_ii_relaxed_constraint_statistics& nrcs, MIP_Problem& MP)
{

    // based on the relaxed constraints vector
    nrcs.LSP_prop = double(decomp.largest_sp) / double(MP.getNumVariables());
    nrcs.num_constraints_relaxed_prop = double(decomp.number_constraints_relaxed) / double(MP.getNumConstraints());
    nrcs.equality_constaints_relaxed_prop = double(getNumberEqualityConstraintsRelaxed(decomp.con_vec,MP)) / double(decomp.number_constraints_relaxed);
    nrcs.inequality_constaints_relaxed_prop = 1.00 - nrcs.equality_constaints_relaxed_prop;

    //how to return constraint_type at idx.. if it exists...

    // check to see if

    // logic begind getting proportion of constraints which are equality..
    // loop through con vec and if == 1, get constraint type at idx...

    // int decomposition_idx;
    // vector<double>* con_vec_ptr;
    // double LSP_prop;
    // double num_constraints_relaxed_prop;
    // // proportion of relaxed constraints which are equality
    // double equality_constaints_relaxed_prop;
    // // proportion of relaxed constraints which are inequality = 1-equality_constaints_relaxed_prop
    // double inequality_constaints_relaxed_prop;
    // // non_zero prop in constraints relaxed
    // double average_nonzero_prop;
    // double stddev_nonzero_prop;
    // // ratio of RHS to largest coeff in constraint
    // double average_largest_ratio;
    // double stddev_largest_ratio;
}

int MIP_Problem_Probe::getNumberEqualityConstraintsRelaxed(const vector<double>& con_vec, MIP_Problem& MP)
{   
    int num_equality_constraints = 0;
    for (int con_idx = 0; con_idx < con_vec.size(); ++con_idx) {
        // account for potential rounding errors
        if (int(con_vec[con_idx] + 0.1) == 1){
            bool success_flag = false;
            bound_type data = MP.getConstraintType(con_idx,success_flag);
            if (success_flag){
                if (data == Equal){
                    ++num_equality_constraints;
                }
            }
        }
    }
}

double MIP_Problem_Probe::getAverageNonZeroInRelaxedConstraints(const vector<double>& con_vec, MIP_Problem& MP);