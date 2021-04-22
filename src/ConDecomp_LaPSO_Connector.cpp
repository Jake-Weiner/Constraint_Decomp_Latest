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

ConDecomp_LaPSO_Connector::ConDecomp_LaPSO_Connector(ConnectorRequirements& CR)
{
    maxsolves = 1;
    total_solve_time_lim = CR.total_LR_time_lim;
    nsolves = 0;
    OP_ptr = CR.MP_ptr;
    debug_printing = CR.debug_printing;
    initOriginalCosts();
    populateDualIdxToOrigIdxMap(*CR.con_relax_vector);
    cplex_subproblem_sum_var_squared = 0;
    initSubproblems(*CR.ps, OP_ptr->getNumVariables());
    subproblem_statistics_ptr = CR.ss_ptr;
    LP_solve_time = 0.00;
}

void ConDecomp_LaPSO_Connector::initOriginalCosts()
{   
    if (debug_printing){
        cout << "Initialisiing Original Costs" << endl;
    }
    original_costs.resize(OP_ptr->getNumVariables(), 0.00);
    for (auto& objective_term : OP_ptr->objective_fn) {
        int var_idx = objective_term.first;
        double var_coeff = objective_term.second;
        if (debug_printing){
            cout << "var_idx is " << var_idx << endl;
            cout << "var_coeff is " << var_coeff << endl;
        }
        original_costs[var_idx] = var_coeff;
    }
}

// return the original constraint index. -1 is returned if not found
int ConDecomp_LaPSO_Connector::getOriginalIdxFromDual(const int dual_idx){

    int ret_val = -1;
    if (dual_idx_to_orig_constraint_idx_map.find(dual_idx) != dual_idx_to_orig_constraint_idx_map.end()){
        ret_val = dual_idx_to_orig_constraint_idx_map[dual_idx];
    }
    return ret_val;
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

    // to keep track of progress, print out every 10000 subproblem set up
    int subproblem_idx = 0;
    for (auto& partition : partitions) {
        if (subproblem_idx % 10000 == 0) {
            cout << "setting up subproblem " << subproblem_idx << endl;
        }

        CPLEX_MIP_Subproblem sp;
        // // set the proportion of var in subproblem as proportion of total number of variables
        // sp.setSubproblemVarProp(static_cast<double>(partition.getNumNodes())  / static_cast<double>(total_num_var));
        sp.setSubproblemIdx(subproblem_idx);
        sp.setNumVars(partition.getNumNodes());
        sp.setNumConstr(partition.getNumEdges());
        // creatre a new IloEnv object which is used for memory management of Ilo objects
        sp.envPtr = new IloEnv;
        IloModel model(*(sp.envPtr));
        IloNumVarArray subproblem_vars_cplex(*(sp.envPtr));
        IloRangeArray subproblem_constraints_cplex(*(sp.envPtr));

        // add the number of variables squared in the subproblem to the total 
        // for deciding on subproblem solve time limit
        cplex_subproblem_sum_var_squared += pow(partition.getNumNodes(), 2);
        // add all variables in partition to subproblem model
        vector<Variable> var_in_partition;
        for (const auto& var_idx : partition.node_idxs) {
            var_in_partition.push_back(OP_ptr->getVariable(var_idx));
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

        // if only 1 variable is involved and there is no constraint, no need to create a mip model
        if (sp.getNumVars() == 1 && sp.getNumConstr() == 0) {
            // if (partition.getNumEdges() != 0){
            //     int orig_var_idx = partition.node_idxs[0];
            //     cout << "Found Subproblem with 1 var and a constraint " << endl;
            //     cout << "Var index is " << orig_var_idx << endl;
            // }
            if (sp.envPtr != nullptr) {
                if ((*(sp.envPtr)).getImpl() != nullptr) {
                    (*(sp.envPtr)).end();
                }
                delete sp.envPtr;
                sp.envPtr = nullptr;
            }
            cplex_subproblems.push_back(sp);
            ++subproblem_idx;
            continue;
        }

        //otherwise continue with creating the cplex model for the subproblem
        else{
            vector<Constraint> subproblem_constraints;
            if (!partition.edge_idxs.empty()) {
                for (auto& constraint_idx : partition.edge_idxs) {
                    subproblem_constraints.push_back(OP_ptr->constraints[constraint_idx]);
                }
                if (debug_printing && subproblem_idx==1){
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
                        if (debug_printing && subproblem_idx==1){
                            cout << var_coeff << "x_" << var_idx <<  " ";
                        }
                    }

                    if (constraint.getBoundType() == Greater) {
                        IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, INF);
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing && subproblem_idx==1){
                            cout << ">= " << constraint.getRHS() << endl;
                        }
                    } else if (constraint.getBoundType() == Equal) {
                        IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, constraint.getRHS());
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing && subproblem_idx==1){
                            cout << "== " << constraint.getRHS() << endl;
                        }
                    } else if (constraint.getBoundType() == Less) {
                        IloRange r1(*(sp.envPtr), -INF, constraint_exp, constraint.getRHS());
                        subproblem_constraints_cplex.add(r1);
                        if (debug_printing && subproblem_idx==1){
                            cout << "<= " << constraint.getRHS() << endl;
                        }
                    }
                }
                model.add(subproblem_constraints_cplex);
            }
            // IloModel model2 = IloModel(*sp.envPtr);
            // model2.add(subproblem_constraints_cplex);
            // model2.add(IloConversion(*sp.envPtr, subproblem_vars_cplex, ILOFLOAT));
            // cout << model2 << endl;
            sp.model = model;
            sp.variables = subproblem_vars_cplex;
            if (debug_printing && subproblem_idx==1){
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
        Constraint con = OP_ptr->constraints[original_constraint_idx];
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
                cout << "-1 * var_coeff = " << (-1.00 * var_coeff) << endl;
            }
            s.rc[var_idx] += s.dual[dual_idx] * (-1.00 * var_coeff);
        }
    }

    // correct any rounding errors that may arise from using dual values from LP solution
    for (int i = 0; i < s.rc.size(); i++) {
        if (abs(s.rc[i]) < 0.00000001){
            s.rc[i] = 0.00;
        }
    }
    return OK;
}

// at this point the problem is solved as a MIP and then as a LP. For multiple iterations, this needs to be changed
// so that the variables are converted back to their original types after being converted for floats for LP
// solution
// the solve time remaining is also updated for each subproblem solved
// returns 0 for success, -1 for failure
int ConDecomp_LaPSO_Connector::solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, Solution& s, double& solve_time_remaining, bool debug_print)
{
    int ret_val = 0;
    bool check_subproblem = false;
    // if (debug_print){
    //     cout << "number of variables in sp is " << sp.subproblemVarIdx_to_originalVarIdx.size() << endl;
    // }
    //if subproblem size is 1 and there are no constraints involved, solve without cplex
    if (sp.getNumVars() == 1 && sp.getNumConstr() == 0) {
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[0];
        long double var_reduced_cost = s.rc[original_var_idx];
        
        Variable var = OP_ptr->getVariable(original_var_idx);

        double upper_bound = var.getUB();
        double lower_bound = var.getLB();
        // if negative coeff, set to max var bound, else if positive coefficient set to lower bound
        if (var_reduced_cost < 0.00l) {
            s.x[original_var_idx] = upper_bound;
        } else{
            s.x[original_var_idx] = lower_bound;
        }
        if (debug_print){
            cout << "reduced cost in problem subproblem is " << var_reduced_cost << endl;
            cout << "problem var value is  " << s.x[original_var_idx] << endl;
            cout << "contribution to bound is " << s.x[original_var_idx] * var_reduced_cost << endl;
        }


        // update lower bound
        s.lb += (s.x[original_var_idx] * var_reduced_cost);
        // capture the solution statistics
        int subproblem_idx = sp.getSubproblemIdx();
        subproblem_statistics_ptr->mip_times[subproblem_idx] = 0.00;
        subproblem_statistics_ptr->lp_times[subproblem_idx] = 0.00;
        subproblem_statistics_ptr->mip_obj_solutions[subproblem_idx] = var_reduced_cost * s.x[original_var_idx];
        subproblem_statistics_ptr->lp_obj_solutions[subproblem_idx] = var_reduced_cost * s.x[original_var_idx];
        subproblem_statistics_ptr->subproblem_optimality_success[subproblem_idx] = true;
        subproblem_statistics_ptr->subproblem_attempted[subproblem_idx] = true;
        return 0;
    }

    // otherwise, create the new objective function using the reduced costs
    // add in objective function to the model...
    IloExpr obj_exp(sp.model.getEnv());
    if (debug_printing){
            cout << "Obj Function: ";
    }
    // create the obj function for the subproblem
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

    // 90% of the allocate subproblem solve time is dedicated to solving the MIP
    // 10% is dedicated to solving the LP
    double mip_subproblem_solve_time = max(0.9 * sp.getSubproblemRunTime(),0.1);
    // double lp_subproblem_solve_time = max(0.1 * sp.getSubproblemRunTime(),0.1);
    // 1 minute max to solving the LP of subproblem
    double lp_subproblem_solve_time = 60;
    // flag to see if CPLEX at least finishes processing the root node
    bool root_node_finished = true;
    int subproblem_idx = sp.getSubproblemIdx();
    // add the objective function to the model
    IloObjective obj_fn = IloMinimize(sp.model.getEnv(), obj_exp);
    sp.model.add(obj_fn);
    // try and gather statistics from cplex object. If there is an error, set the LB to inf so the solution can be discarded
    // and analysed for any problems
    try{
        // solve the subproblem as a MIP and get the statistics
        IloCplex cplex(sp.model);
        if (debug_printing){
            cout << "MIP subproblem solve time is " << sp.getSubproblemRunTime() << endl;
        }
        cplex.setParam(IloCplex::Threads, 1); // solve using 1 thread only
        // subproblem time should be based on var prop
        if (debug_printing){
            cout << "MIP subproblem solve time is " << mip_subproblem_solve_time << endl;
        }
        cplex.setParam(IloCplex::TiLim, mip_subproblem_solve_time);
        cplex.setParam(IloCplex::EpGap, 0.01);
        cplex.setOut((*(sp.envPtr)).getNullStream());
        bool feasible_sol = cplex.solve();
        subproblem_statistics_ptr->subproblem_attempted[subproblem_idx] = true;
        // if MIP subproblem is not solved to optimality or at least within optimality tolerance
        if (cplex.getCplexStatus() != IloCplex::Status::Optimal && cplex.getCplexStatus() != IloCplex::Status::OptimalTol){
            cout << "Failed to find optimal MIP subproblem solution in subproblem: " << sp.getSubproblemIdx() << endl;
            // if MIP subproblem has root node which is not yet finished
            // set the subproblem solve time to be 60s and solve the root node 
            if (cplex.getNnodes() == 0){
                cplex.setParam(IloCplex::TiLim, 60);
                cplex.setParam(IloCplex::NodeLim,1);
                cplex.solve();
        
                cout << "cplex solve status is " << cplex.getCplexStatus() << endl;
                cout << "cplex primal status " << cplex.isPrimalFeasible() << endl;
                cout << "cplex dual status is " << cplex.isDualFeasible() << endl;
                // if after this cplex is still not able to solve the root node,
                // use the LP solution
                if (cplex.getNnodes() == 0){
                    root_node_finished = false;
                    cout << "root node still not finished after an additional 60s" << endl;
                    // cplex.exportModel("/home/jake/PhD/Decomposition/Massive/Machine_Learning/MIP_problem.mps");

                }
                // otherwise take the objective from the root node
                else{
                    s.lb += cplex.getBestObjValue();
                }
            }
            // otherwise use the best objective from all nodes explored as the bound         
            else{
                s.lb += cplex.getBestObjValue();
            }
            cout << "testing it problem reaches here" << endl;
            // flag subproblem as having non optimal solution
            subproblem_statistics_ptr->subproblem_optimality_success[subproblem_idx] = false;
            if (root_node_finished){
                subproblem_statistics_ptr->mip_obj_solutions[subproblem_idx] = cplex.getBestObjValue();
            }
            ret_val = 0;
        }
        // subproblem solve to optimality
        else{
            s.lb += cplex.getObjValue();
            // if (cplex.getObjValue() < -10000000000){
            //     cout << "Successful MIP producing bounds value of " << cplex.getObjValue() << endl
            // }
            if (debug_printing){
                cout << "MIP Solution status is " << cplex.getCplexStatus() << endl;
                cout << "MIP subproblem value is " <<  cplex.getObjValue() << endl;
                cout << "MIP subproblem value is " <<  cplex.getBestObjValue() << endl;
            }
            subproblem_statistics_ptr->subproblem_optimality_success[subproblem_idx] = true;
            // capture the mip solution quality
            subproblem_statistics_ptr->mip_obj_solutions[subproblem_idx] = cplex.getObjValue();
        }
        // take time away from remaining runtime
        solve_time_remaining -= cplex.getTime();
        //capture the mip runtime in cpu seconds
        subproblem_statistics_ptr->mip_times[subproblem_idx] = cplex.getTime();
        
        //capture the variable values from the subproblem solutions if a solution has been found
        if (cplex.isPrimalFeasible() == IloTrue){
            for (int i = 0; i < sp.variables.getSize(); ++i) {
                int orig_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
                IloNum val = cplex.getValue(sp.variables[i]);
                Variable_Type vt = OP_ptr->variables[orig_idx].getVarType();
                if (vt == Int || vt == Bin) {
                    // in case of rounding errors, add in slight perturbation and then convert to int to round down
                    int x_val = (int)(val + 0.1);
                    s.x[orig_idx] = x_val;
                } else if (vt == Cont) {
                    s.x[orig_idx] = val;
                }
            }
        }
        cplex.end();
    }
    // try and gather statistics from cplex object. If there is an error, set the LB to inf so the solution can be dicarded
    catch (IloCplex::Exception &e){
        cout << e << endl;
        cout << "Exception caught in ConDecomp_LaPSO_Connector when accessing MIP Solution" << endl;
        s.lb = std::numeric_limits<double>::max();
        //cout << obj_exp << endl;
        //cout << sp.model << endl;
        cout << "solve time is " << mip_subproblem_solve_time << endl;
        ret_val = -1;
        // end the cplex enviroment to free up memory
    }
    // solve the subproblem as a LP and get the statistics
    IloModel relax(*sp.envPtr);
    relax.add(sp.model);
    relax.add(IloConversion(*sp.envPtr, sp.variables, ILOFLOAT));

     // try and gather statistics from cplex object. If there is an error, set the LB to inf so the solution can be dicarded
    try{
        IloCplex cplex_relaxed(relax);
       // cout << cplex_relaxed.getModel() << endl;
        cplex_relaxed.setParam(IloCplex::Threads, 1); // solve using 1 thread only
        cplex_relaxed.setParam(IloCplex::TiLim, lp_subproblem_solve_time);
        cplex_relaxed.setParam(IloCplex::RootAlg, IloCplex::Barrier); // use barrier method
        cplex_relaxed.setOut((*(sp.envPtr)).getNullStream());
        // get the best dual bound
        bool solve_relaxed_status = cplex_relaxed.solve();
        // if optimal LP solution can't be found
        if (cplex_relaxed.getCplexStatus() != IloCplex::Status::Optimal) {
            cout << "Failed to find optimal LP Solution" << endl;
            subproblem_statistics_ptr->subproblem_lp_found[subproblem_idx] = false;
            // get dual solution if lp solution is not available?
            subproblem_statistics_ptr->lp_obj_solutions[subproblem_idx] = cplex_relaxed.getBestObjValue();
            // if the root node from MIP could not be solved, us the lp value from the dual simplex method for the subproblem
            if (root_node_finished == false){
                // s.lb += cplex_relaxed.getBestObjValue();
                // s.lb += cplex_relaxed.getObjValue();
                // cplex_relaxed.exportModel("/home/jake/PhD/Decomposition/Massive/Machine_Learning/LP_problem.mps");
                // exit(0);
                cout << "cplex_relaxed best obj val is " << cplex_relaxed.getBestObjValue() << endl;
                cout << "cplex_relaxed obj val is " << cplex_relaxed.getObjValue() << endl;
                cout << "cplex solve status is " << cplex_relaxed.getCplexStatus() << endl;
                cout << "cplex primal status " << cplex_relaxed.isPrimalFeasible() << endl;
                cout << "cplex dual status is " << cplex_relaxed.isDualFeasible() << endl;
                // cout << relax << endl;
                subproblem_statistics_ptr->mip_obj_solutions[subproblem_idx] = cplex_relaxed.getBestObjValue();
            }
            if (check_subproblem){
                cout << "non optimal LP is " << cplex_relaxed.getBestObjValue() << endl;
            }
            ret_val = 0;
        }
        else{
            subproblem_statistics_ptr->subproblem_lp_found[subproblem_idx] = true;
            // capture the lp solution quality<
            subproblem_statistics_ptr->lp_obj_solutions[subproblem_idx] = cplex_relaxed.getObjValue();
            if (root_node_finished == false){
                s.lb += cplex_relaxed.getObjValue();
                subproblem_statistics_ptr->mip_obj_solutions[subproblem_idx] = cplex_relaxed.getObjValue();
            }
            if (check_subproblem){
                cout << "optimal LP is " << cplex_relaxed.getObjValue() << endl;
            }
        }

        //capture the lp runtime in cpu seconds
        subproblem_statistics_ptr->lp_times[subproblem_idx] = cplex_relaxed.getTime();
        LP_solve_time += cplex_relaxed.getTime();
        solve_time_remaining -= cplex_relaxed.getTime();
         // end the LP relaxed environment to free up memory
        cplex_relaxed.end();
    }
    catch (IloCplex::Exception &e){
        cout << e << endl;
        cout << "Exception caught in ConDecomp_LaPSO_Connector when accessing LP Solution" << endl;
        cout << obj_exp << endl;
        ret_val = -1;
    }
    // remove the obj_fn from the model object
    sp.model.remove(obj_fn);    
    return ret_val;
}

// this method adds in the constant factor * Lag multipliers to the LB
void ConDecomp_LaPSO_Connector::addConstLagMult(ConDecomp_LaPSO_Connector_Solution& s)
{   
    // lb = cx + \lambda(b-Ax) = (c- \lambda * A)x + \lambda * b 
    // lb = s.rc * s.x + s.dual* b
    double const_mult_add = 0.0;
    // for each dual variable, add to lower bound dual * rhs
    for (int dual_idx = 0; dual_idx < s.dual.size(); dual_idx++) {
        //get RHS for the constraint
        int constraint_orig_idx = dual_idx_to_orig_constraint_idx_map[dual_idx];
        double constraint_bound = OP_ptr->constraints[constraint_orig_idx].getRHS();
        s.lb += (s.dual[dual_idx] * constraint_bound);
        const_mult_add += (s.dual[dual_idx] * constraint_bound);
        if (debug_printing){
            cout << "for nsolves " << nsolves << " dual[" << dual_idx << "] is: " << s.dual[dual_idx] << endl;
            cout << "for nsolves " << nsolves << " constraint bound[" << dual_idx << "] is: " << constraint_bound << endl;
        }
    }
    if (debug_printing){
        cout << "solution lower bound is " << s.lb << endl;
    }
    cout << "Constants added to lower bound is " << const_mult_add << endl;
}

// loop through all constraints, calculate Ax, viol = bound - Ax
void ConDecomp_LaPSO_Connector::updateParticleViol(ConDecomp_LaPSO_Connector_Solution& s)
{   
    //reset viol
    s.viol = 0;

    for (std::pair<int, int> element : dual_idx_to_orig_constraint_idx_map){
        int dual_constraint_idx = element.first;
        int original_constraint_idx = element.second;
        Constraint con = OP_ptr->constraints[original_constraint_idx];
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
        bool debug_print = false;
        if (debug_printing){
            cout << "number of variables in subproblem to be solved is " << cplex_subproblems[sp_idx].getNumVars() << endl;
        }
        double allocated_subproblem_solve_time = 0.00;
        if (sp_idx < number_of_subproblems - 1){
            // assign a proportion of the total solve time proportional to the square of the number of variables in the subproblem
            allocated_subproblem_solve_time = (static_cast<long double>(pow(cplex_subproblems[sp_idx].getNumVars(),2)) / static_cast<long double>(cplex_subproblem_sum_var_squared)) * total_solve_time_lim;
            cplex_subproblems[sp_idx].setSubproblemRunTime(allocated_subproblem_solve_time);
        }
        // for the last and largest subproblem, give all of the remaining solve time 
        else{
            allocated_subproblem_solve_time = solve_time_remaining;
            if (debug_printing){
                double original_subproblem_solve_time = (static_cast<double>(pow(cplex_subproblems[sp_idx].getNumVars(),2)) / static_cast<double>(cplex_subproblem_sum_var_squared)) * total_solve_time_lim;
                cout << "solving last subproblem, solve time allocated is " << allocated_subproblem_solve_time 
                << ". If subproblem solve time was proportional to variable size, it would've been " << original_subproblem_solve_time
                << endl;
            }
            cplex_subproblems[sp_idx].setSubproblemRunTime(allocated_subproblem_solve_time);
        }

        int subproblem_status = solveSubproblemCplex(cplex_subproblems[sp_idx], s, solve_time_remaining,debug_print);
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

    // if (debug_printing == true) {
        std::cout << "Subproblem solve " << nsolves << "/" << maxsolves << ": "
                  << " lb=" << s.lb << " ub=" << s.ub ;
        if (s.dual.size() > 0){
            std::cout << "\trange of dual = " << s.dual.min() << " to " << s.dual.max() << std::endl
                  << "\trange of viol = " << s.viol.min() << " to " << s.viol.max() << std::endl;
        }
                  
        std::cout << std::endl;
    // }

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