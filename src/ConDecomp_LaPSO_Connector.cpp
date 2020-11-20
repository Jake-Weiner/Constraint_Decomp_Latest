#include "ConDecomp_LaPSO_Connector.h"
#include "Random.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <math.h>

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <cstdlib>
#include <memory>
#include <string>

using namespace boost;
using namespace std;
using Decomposition_Statistics::Subproblems;

ConDecomp_LaPSO_Connector::ConDecomp_LaPSO_Connector(MIP_Problem& original_problem, const vector<Partition_Struct>& partitions,
    const vector<int>& con_vec, const bool printing, const double& total_solve_time, 
    std::shared_ptr<Subproblems> subproblem_statistics_ptr) : total_solve_time_lim(total_solve_time)
{
    this->OP = original_problem;
    this->debug_printing = printing;
    initOriginalCosts();
    populateDualIdxToOrigIdxMap(con_vec);
    cplex_subproblem_sum_var_squared = 0;
    initSubproblems(partitions, original_problem.getNumVariables());
    this->subproblem_statistics_ptr = subproblem_statistics_ptr;
}

void ConDecomp_LaPSO_Connector::initOriginalCosts()
{   
    if (debug_printing){
        cout << "Initialisiing Original Costs" << endl;
    }
    original_costs.resize(OP.getNumVariables(), 0);
    for (auto& objective_term : OP.objective_fn) {
        int var_idx = objective_term.first;
        double var_coeff = objective_term.second;
        if (debug_printing){
            cout << "var_idx is " << var_idx << endl;
            cout << "var_coeff is " << var_coeff << endl;
        }
        original_costs[var_idx] = var_coeff;
    }
}

// converts the indicies of the equality,less,greater than indexes from the original indices to the dual indices
LaPSO::constraint_type_indicies ConDecomp_LaPSO_Connector::convertOriginalConstraintTypeIndicies(const LaPSO::constraint_type_indicies& cti){

    std::vector<int> dual_equality_idxs;
    std::vector<int> dual_less_than_idxs;
    std::vector<int> dual_greater_than_idxs;

    // convert equality constraint indicies
    for (auto& idx : cti.equality_idxs){
        if (orig_constraint_idx_to_dual_idx_map.find(idx) != orig_constraint_idx_to_dual_idx_map.end()){
            dual_equality_idxs.push_back(orig_constraint_idx_to_dual_idx_map[idx]);
        }
    }

    // convert equality constraint indicies
    for (auto& idx : cti.less_than_idxs){
        if (orig_constraint_idx_to_dual_idx_map.find(idx) != orig_constraint_idx_to_dual_idx_map.end()){
            dual_less_than_idxs.push_back(orig_constraint_idx_to_dual_idx_map[idx]);
        }
    }

    // convert equality constraint indicies
    for (auto& idx : cti.greater_than_idxs){
        if (orig_constraint_idx_to_dual_idx_map.find(idx) != orig_constraint_idx_to_dual_idx_map.end()){
            dual_greater_than_idxs.push_back(orig_constraint_idx_to_dual_idx_map[idx]);
        }
    }

    LaPSO::constraint_type_indicies dual_indicies{dual_equality_idxs, dual_less_than_idxs, dual_greater_than_idxs};
    return dual_indicies;

}

// convert the original constraint indicies with dual values to the dual indicies
vector<initial_dual_value_pair> ConDecomp_LaPSO_Connector::convertOriginalConstraintInitialDualIndicies(const std::vector<initial_dual_value_pair>& original_initial_dual_pairs){
    vector<initial_dual_value_pair> dual_index_pair_values;
    // for each 
    for (const auto& dual_constraint_pair : original_initial_dual_pairs){
        int orig_constraint_idx = dual_constraint_pair.first;
        // check if the original constraint maps to a dual constraint (i.e is one of the relaxed constraints)
        if (orig_constraint_idx_to_dual_idx_map.find(orig_constraint_idx) != orig_constraint_idx_to_dual_idx_map.end()){
            int dual_idx = orig_constraint_idx_to_dual_idx_map[orig_constraint_idx];
            dual_index_pair_values.push_back({dual_idx, dual_constraint_pair.second});
        }
    }
    return dual_index_pair_values;
}

// create a mapping between the dual variable indexes and the original constraint indexes
void ConDecomp_LaPSO_Connector::populateDualIdxToOrigIdxMap(const vector<int>& con_relax_vector){

    //dual idx is assigned as the count index
    int con_relaxation_count = 0;
    for (const auto& orig_con_idx : con_relax_vector){
        dual_idx_to_orig_constraint_idx_map[con_relaxation_count] = orig_con_idx;
        orig_constraint_idx_to_dual_idx_map[orig_con_idx] = con_relaxation_count;
        ++con_relaxation_count;
    }
}

// Initialise the various subproblems based on the partitions created via relaxing constraints
// total number of variables squared in each subproblem is also counted
void ConDecomp_LaPSO_Connector::initSubproblems(const vector<Partition_Struct>& partitions, const int& total_num_var)
{
    int subproblem_idx = 0;
    for (auto& partition : partitions) {
        if (subproblem_idx % 10000 == 0) {
            cout << "setting up subproblem " << subproblem_idx << endl;
        }

        CPLEX_MIP_Subproblem sp;

        // // set the proportion of var in subproblem as proportion of total number of variables
        // sp.setSubproblemVarProp(static_cast<double>(partition.getNumNodes())  / static_cast<double>(total_num_var));
        sp.setSubproblemIdx(subproblem_idx);
        
        // creatre a new IloEnv object which is used for memory management of Ilo objects
        sp.envPtr = new IloEnv;
        IloModel model(*(sp.envPtr));
        IloNumVarArray subproblem_vars_cplex(*(sp.envPtr));
        IloRangeArray subproblem_constraints_cplex(*(sp.envPtr));

        // add all variables in partition to subproblem model
        vector<Variable> var_in_partition;
        for (const auto& var_idx : partition.node_idxs) {
            var_in_partition.push_back(OP.getVariable(var_idx));
        }

        int subproblem_var_idx = 0;
        sp.subproblemVarIdx_to_originalVarIdx.reserve(var_in_partition.size());
        sp.originalVarIdx_to_subproblemVarIdx.reserve(var_in_partition.size());
        for (auto& variable : var_in_partition) {
            int orig_var_idx = variable.getVarIndx();
            double var_lb = variable.getLB();
            double var_ub = variable.getUB();
            sp.originalVarIdx_to_subproblemVarIdx[orig_var_idx] = subproblem_var_idx;
            sp.subproblemVarIdx_to_originalVarIdx[subproblem_var_idx] = orig_var_idx;
            if (variable.getVarType() == Int) {
                IloNumVar x(*(sp.envPtr), var_lb, var_ub, ILOINT);
                subproblem_vars_cplex.add(x);
            } else if (variable.getVarType() == Bin){
                IloNumVar x(*(sp.envPtr), var_lb, var_ub, ILOBOOL);
                subproblem_vars_cplex.add(x);
            } else if (variable.getVarType() == Cont) {
                IloNumVar x(*(sp.envPtr), var_lb, var_ub, ILOFLOAT);
                subproblem_vars_cplex.add(x);
            }
            ++subproblem_var_idx;
        }
        if (subproblem_idx % 10000 == 0) {
            cout << "number of variables in subproblem is " << subproblem_var_idx << endl;
        }

        // if only 1 variable is involved, no need to create a mip model
        if (subproblem_var_idx == 1) {
            sp.num_subproblem_vars = subproblem_var_idx;
            if (sp.envPtr != nullptr) {
                if ((*(sp.envPtr)).getImpl() != nullptr) {
                    (*(sp.envPtr)).end();
                }
                delete sp.envPtr;
                sp.envPtr = nullptr;
            }
            cplex_subproblems.push_back(sp);
            ++subproblem_idx;
            // tally sum of square of number of variables
            cplex_subproblem_sum_var_squared += 1;
            continue;
        }

        //otherwise continue with creating the cplex model for the subproblem
        else{
            vector<Constraint> subproblem_constraints;
            if (!partition.edge_idxs.empty()) {
                for (auto& constraint_idx : partition.edge_idxs) {
                    subproblem_constraints.push_back(OP.constraints[constraint_idx]);
                }
                if (debug_printing){
                    cout << "For subproblem " << subproblem_idx << " Constaints are: " << endl;
                }
                //add constraints to the model
                for (auto& constraint : subproblem_constraints) {
                    IloExpr constraint_exp(*(sp.envPtr));
                    for (auto& term : constraint.getConTerms()) {
                        int var_idx = term.first;
                        double var_coeff = term.second;
                        int var_idx_in_sp = sp.originalVarIdx_to_subproblemVarIdx[var_idx];
                        constraint_exp += (var_coeff * subproblem_vars_cplex[var_idx_in_sp]);
                        if (debug_printing){
                            cout << var_coeff << "x_" << var_idx <<  " ";
                        }
                    }
                    if (constraint.getBoundType() == Greater) {
                        IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, INF);
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing){
                            cout << ">= " << constraint.getRHS();
                        }
                    } else if (constraint.getBoundType() == Equal) {
                        IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, constraint.getRHS());
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing){
                            cout << "== " << constraint.getRHS();
                        }
                    } else if (constraint.getBoundType() == Less) {
                        IloRange r1(*(sp.envPtr), -INF, constraint_exp, constraint.getRHS());
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing){
                            cout << "<= " << constraint.getRHS();
                        }
                    }
                }
                model.add(subproblem_constraints_cplex);
            }
            sp.model = model;
            sp.variables = subproblem_vars_cplex;
            sp.num_subproblem_vars = subproblem_var_idx;
            // tally sum of square of number of variables

            cplex_subproblem_sum_var_squared += pow(subproblem_var_idx,2);
            if (debug_printing){
                cout << "subproblem var idx is " << subproblem_var_idx << endl;
                cout << "cplex_subproblem_sum_var_squared is " << cplex_subproblem_sum_var_squared << endl;
                cout << "subprovlem var idx squared is " << pow(subproblem_var_idx,2) << endl;
            }
            cplex_subproblems.push_back(sp);
            ++subproblem_idx;
            
        }
    }
    cout << "finished initialising subproblems " << endl;
    return;
}

Status ConDecomp_LaPSO_Connector::reducedCost(Solution& s)
{


    //reset the reduced costs to the original costs
    for (int i = 0; i < s.rc.size(); i++) {
        s.rc[i] = original_costs[i];
    }

    // for each dual variable, update the associated costs for each variable involved
    for (int dual_idx = 0; dual_idx < s.dual.size(); ++dual_idx){
        // get original constraint idx from dual index
        int original_constraint_idx = dual_idx_to_orig_constraint_idx_map[dual_idx];
        Constraint con = OP.constraints[original_constraint_idx];
        if (debug_printing){
            cout << "for dual idx = " << dual_idx << endl;
        }
        
        // con terms are pair<var_idx, var_coeff>
        for (auto& con_term : con.getConTerms()) {
            int var_idx = con_term.first;
            double var_coeff = con_term.second;
            if(debug_printing){
                cout << "var idx is " << var_idx << endl;
                cout << "s.rc[" << var_idx << "] = " << s.rc[var_idx] << endl;
                cout << "s.dual[" << dual_idx << "] = " << s.dual[dual_idx] << endl;
                cout << "-1 * var_coeff = " << (-1 * var_coeff) << endl;
            }
            s.rc[var_idx] += s.dual[dual_idx] * (-1 * var_coeff);
        }
    }
    return OK;
}

// at this point the problem is solved as a MIP and then as a LP. For multiple iterations, this needs to be changed
// so that the variables are converted back to their original types after being converted for floats for LP
// solution
// the solve time remaining is also updated for each subproblem solved
// returns 0 for success, -1 for failure
int ConDecomp_LaPSO_Connector::solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, Solution& s, double& solve_time_remaining)
{
    int ret_val = 0;
    //if subproblem size is 1 solve without cplex
    if (sp.subproblemVarIdx_to_originalVarIdx.size() == 1) {
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[0];
        double var_reduced_cost = s.rc[original_var_idx];
        
        Variable var = OP.getVariable(original_var_idx);

        double upper_bound = var.getUB();
        double lower_bound = var.getLB();

        double mip_soln = 0;
        // if negative coeff, set to max var bound, else if positive coefficient set to lower bound
        if (var_reduced_cost < 0) {
            s.x[original_var_idx] = upper_bound;
        } else{
            s.x[original_var_idx] = lower_bound;
        }
        // update lower bound
        s.lb += (s.x[original_var_idx] * var_reduced_cost);
       
        
        // capture the solution statistics
        subproblem_statistics_ptr->mip_times.push_back(0);
        subproblem_statistics_ptr->lp_times.push_back(0);
        subproblem_statistics_ptr->mip_obj_solutions.push_back(var_reduced_cost * s.x[original_var_idx]);
        subproblem_statistics_ptr->lp_obj_solutions.push_back(var_reduced_cost * s.x[original_var_idx]);
        subproblem_statistics_ptr->subproblem_optimality_success.push_back(true);
        subproblem_statistics_ptr->subproblem_attempted[sp.getSubproblemIdx()] = true;
        return 0;
    }

    // otherwise, create the new objective function using the reduced costs
    // add in objective function to the model...
    IloExpr obj_exp(sp.model.getEnv());
    // for each var in subproblem, add in reduced costs of objective function

    if (debug_printing){
            cout << "Obj Function: ";
    }
    for (int i = 0; i < sp.variables.getSize(); i++) {
        //original index -- get coeff
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        // get the reduced cost of the variable and add to objective function
        double coeff = s.rc[original_var_idx];
        obj_exp += (coeff * sp.variables[i]);
        if (debug_printing){
           cout << coeff << "x_" << original_var_idx << " ";
        }
    }

    // add the objective function to the model
    IloObjective obj_fn = IloMinimize(sp.model.getEnv(), obj_exp);
    sp.model.add(obj_fn);

  
    // solve the subproblem as a MIP and get the statistics
    IloCplex cplex(sp.model);
    if (debug_printing){
        cout << "MIP subproblem solve time is " << sp.getSubproblemRunTime() << endl;
    }
    cplex.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    double mip_subproblem_solve_time = (0.9 * sp.getSubproblemRunTime()) + 0.1;
    double lp_subproblem_solve_time = (0.1 * sp.getSubproblemRunTime()) + 0.1;
    cout << "MIP subproblem solve time is " << mip_subproblem_solve_time << endl;
    // subproblem time should be based on var prop
    if (debug_printing){
        cout << "MIP subproblem solve time is " << mip_subproblem_solve_time << endl;
    }
    cplex.setParam(IloCplex::TiLim, mip_subproblem_solve_time);
    cplex.setParam(IloCplex::EpGap, 0.99);
    cplex.setOut((*(sp.envPtr)).getNullStream());
    bool feasible_sol = cplex.solve();
    subproblem_statistics_ptr->subproblem_attempted[sp.getSubproblemIdx()] = true;
    // if MIP subproblem is not solved to optimality or at least within optimality tolerance
    if (cplex.getCplexStatus() != IloCplex::Status::Optimal && cplex.getCplexStatus() != IloCplex::Status::OptimalTol){
        cout << "Failed to find optimal MIP subproblem solution in subpoblem: " << sp.getSubproblemIdx() << endl;
        // print out the subproblem to see why it can't be solved to optimality
        // get the best bound if optimal solution not available        
        s.lb += cplex.getBestObjValue();
        // flag subproblem as having non optimal solution
        subproblem_statistics_ptr->subproblem_optimality_success.push_back(false);
        subproblem_statistics_ptr->mip_obj_solutions.push_back(cplex.getBestObjValue());
        ret_val = -1;
    }
    else{
        s.lb += cplex.getObjValue();
        subproblem_statistics_ptr->subproblem_optimality_success.push_back(true);
        // capture the mip solution quality
        subproblem_statistics_ptr->mip_obj_solutions.push_back(cplex.getObjValue());
    }

    //capture the mip runtime in cpu seconds
    subproblem_statistics_ptr->mip_times.push_back(cplex.getTime());
    solve_time_remaining -= cplex.getTime();
        // capture the variable values from the subproblem solutions
    for (int i = 0; i < sp.variables.getSize(); ++i) {
        int orig_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        try {
            IloNum val = cplex.getValue(sp.variables[i]);
            Variable_Type vt = OP.variables[orig_idx].getVarType();
            if (vt == Int || vt == Bin) {
                // in case of rounding errors, add in slight perturbation and then convert to int to round down
                int x_val = (int)(val + 0.1);
                s.x[orig_idx] = x_val;
            } else if (vt == Cont) {
                s.x[orig_idx] = val;
            }
        } catch (IloException& e) {
            cout << "e" << endl;
            cout << "Exception caught in ConDecomp_LaPSO_Connector::solveSubproblemCplex()" << endl;
        }
    }
    
    // end the cplex enviroment to free up memory
    cplex.end();
    // solve the subproblem as a LP and get the statistics
    IloModel relax(*sp.envPtr);
    relax.add(sp.model);
    relax.add(IloConversion(*sp.envPtr, sp.variables, ILOFLOAT)); 

    IloCplex cplex_relaxed(relax);
    cplex_relaxed.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    cplex_relaxed.setParam(IloCplex::TiLim, lp_subproblem_solve_time);
    cplex_relaxed.setOut((*(sp.envPtr)).getNullStream());
    // get the best dual bound
    bool solve_relaxed_status = cplex_relaxed.solve();
    // if optimal LP solution can't be found
    if (cplex_relaxed.getCplexStatus() != IloCplex::Status::Optimal) {
        cout << "Failed to find optimal LP Solution" << endl;
        subproblem_statistics_ptr->subproblem_lp_found.push_back(false);
        // get dual solution if lp solution is not available?
        subproblem_statistics_ptr->lp_obj_solutions.push_back(cplex_relaxed.getBestObjValue());
        ret_val = -1;
    }
    else{
        subproblem_statistics_ptr->subproblem_lp_found.push_back(true);
        // capture the lp solution quality
        subproblem_statistics_ptr->lp_obj_solutions.push_back(cplex_relaxed.getObjValue());
    }

     //capture the lp runtime in cpu seconds
    subproblem_statistics_ptr->lp_times.push_back(cplex_relaxed.getTime());
    solve_time_remaining -= cplex_relaxed.getTime();
    // end the LP relaxed environment to free up memory
    cplex_relaxed.end();
    // remove the obj_fn from the model object
    sp.model.remove(obj_fn);    
    return ret_val;
}

// this method adds in the constant factor * Lag multipliers to the LB
void ConDecomp_LaPSO_Connector::addConstLagMult(ConDecomp_LaPSO_Connector_Solution& s)
{   
    // lb = cx + \lambda(b-Ax) = (c- \lambda * A)x + \lambda * b 
    // lb = s.rc * s.x + s.dual* b
    
    // for each dual variable, add to lower bound dual * rhs
    for (int dual_idx = 0; dual_idx < s.dual.size(); dual_idx++) {
        //get RHS for the constraint
        int constraint_orig_idx = dual_idx_to_orig_constraint_idx_map[dual_idx];
        double constraint_bound = OP.constraints[constraint_orig_idx].getRHS();
        s.lb += (s.dual[dual_idx] * constraint_bound);
        if (debug_printing){
            cout << "for nsolves " << nsolves << " dual[" << dual_idx << "] is: " << s.dual[dual_idx] << endl;
        }
    }
    if (debug_printing){
        cout << "solution lower bound is " << s.lb << endl;
    }
}

// loop through all constraints, calculate Ax, viol = bound - Ax
void ConDecomp_LaPSO_Connector::updateParticleViol(ConDecomp_LaPSO_Connector_Solution& s)
{   
    //reset viol
    s.viol = 0;

    for (std::pair<int, int> element : dual_idx_to_orig_constraint_idx_map){
        int dual_constraint_idx = element.first;
        int original_constraint_idx = element.second;
        Constraint con = OP.constraints[original_constraint_idx];
        double constraint_bound = con.getRHS();
        //calculate constraint value
        double constraint_value = 0;

        for (auto& con_term : con.getConTerms()) {
            int var_idx = con_term.first;
            double var_coeff = con_term.second;
            constraint_value += (var_coeff * s.x[var_idx]);
        }
       
        s.viol[dual_constraint_idx] = constraint_bound - constraint_value;
    }
}

// solveSubproblem updates both solution viol and LB
Status ConDecomp_LaPSO_Connector::solveSubproblem(Solution& p_)
{
    ++nsolves;
    ConDecomp_LaPSO_Connector_Solution& s(static_cast<ConDecomp_LaPSO_Connector_Solution&>(p_));
    //reset previous primal sol
    s.x = 0;
    // reset lower bound
    s.lb = 0;
    int subproblem_solve_error = 0;
    // for each subproblem, feed in new objective function, solve and update x and lower bound for
    // the solution object
    // sort subproblems based on variable size, solving the small ones first and larger ones later. The largest problem will be proportional
    // to the number of variables or the time remaining, whichever is larger
    std::sort(cplex_subproblems.begin(), cplex_subproblems.end());
    int number_of_subproblems = cplex_subproblems.size();
    double solve_time_remaining = total_solve_time_lim;
    if (debug_printing){
        cout << "total solve time initially is " << total_solve_time_lim << endl;
        cout << "total num var squared is " << cplex_subproblem_sum_var_squared << endl;
    }
    for (int sp_idx = 0; sp_idx< number_of_subproblems; ++sp_idx) 
    {
        if (debug_printing){
            cout << "number of variables in subproblem to be solved is " << cplex_subproblems[sp_idx].num_subproblem_vars << endl;
        }
        double allocated_subproblem_solve_time = 0;
        if (sp_idx < number_of_subproblems - 1){
            // assign a proportion of the total solve time proportional to the square of the number of variables in the subproblem
            allocated_subproblem_solve_time = (static_cast<double>(pow(cplex_subproblems[sp_idx].num_subproblem_vars,2)) / static_cast<double>(cplex_subproblem_sum_var_squared)) * total_solve_time_lim;
            cplex_subproblems[sp_idx].setSubproblemRunTime(allocated_subproblem_solve_time);
        }
        // for the last and largest subproblem, give all of the remaining solve time 
        else{
            allocated_subproblem_solve_time = solve_time_remaining;
            if (debug_printing){
                double original_subproblem_solve_time = (static_cast<double>(pow(cplex_subproblems[sp_idx].num_subproblem_vars,2)) / static_cast<double>(cplex_subproblem_sum_var_squared)) * total_solve_time_lim;
                cout << "solving last subproblem, solve time allocated is " << allocated_subproblem_solve_time 
                << ". If subproblem solve time was proportional to variable size, it would've been " << original_subproblem_solve_time
                << endl;
            }
            cplex_subproblems[sp_idx].setSubproblemRunTime(allocated_subproblem_solve_time);
        }
        int subproblem_status = solveSubproblemCplex(cplex_subproblems[sp_idx], s, solve_time_remaining);
        if (subproblem_status == -1) {
            subproblem_solve_error = 1;
        }
    }

    // if subproblem solve was successful, update the violation vector 
    if (subproblem_solve_error == 0){
        updateParticleViol(s);
    }
    
    // to calculate the true lower bound, add in lamba*RHS constants
    addConstLagMult(s);

    if (debug_printing == true) {
        std::cout << "Subproblem solve " << nsolves << "/" << maxsolves << ": "
                  << " lb=" << s.lb << " ub=" << s.ub
                  << "\trange of dual = " << s.dual.min() << " to " << s.dual.max() << std::endl
                  << "\trange of viol = " << s.viol.min() << " to " << s.viol.max() << std::endl;
        std::cout << std::endl;
    }

    LaPSO::Status status;
    if ((nsolves < maxsolves) && (subproblem_solve_error == 0)) {
        status = OK;
    } else {
        status = ABORT;
    }
    return status;
}

Status ConDecomp_LaPSO_Connector::fixConstraint(const int constraint,
    const Solution& s,
    SparseVec& feas) { return OK; }

Status ConDecomp_LaPSO_Connector::heuristics(Solution& p_)
{
    nsolves++;
    return (nsolves < maxsolves) ? OK : ABORT;
}

Status ConDecomp_LaPSO_Connector::updateBest(Solution& p_)
{
    return OK;
}

void ConDecomp_LaPSO_Connector::endCplexEnvs()
{
    for (auto& subproblem : cplex_subproblems) {
        if (subproblem.envPtr != nullptr) {
            if ((*(subproblem.envPtr)).getImpl() != nullptr) {
                (*(subproblem.envPtr)).end();
            }
            delete subproblem.envPtr;
            subproblem.envPtr = nullptr;
        }
        subproblem.subproblemVarIdx_to_originalVarIdx.clear();
        subproblem.originalVarIdx_to_subproblemVarIdx.clear();
    }
}

// get the original constraint index from the supplied dual index
int ConDecomp_LaPSO_Connector::getOriginalConIdx(int dual_idx){

    int ret_val = -1;
    // dual index found
    if (dual_idx_to_orig_constraint_idx_map.find(dual_idx) != dual_idx_to_orig_constraint_idx_map.end()){
        ret_val = dual_idx_to_orig_constraint_idx_map[dual_idx];
    }
    return ret_val;
}