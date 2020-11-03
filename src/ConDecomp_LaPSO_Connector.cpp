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
    const vector<bool>& con_vec, const bool printing, const double sp_solve_time_limit, 
    std::shared_ptr<Subproblems> subproblem_statistics_ptr)
{
    this->OP = original_problem;
    this->debug_printing = printing;
    this->sp_solve_time_limit = sp_solve_time_limit;
    initOriginalCosts();
    populateDualIdxToOrigIdxMap(con_vec);
    initSubproblems(partitions);
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
void ConDecomp_LaPSO_Connector::populateDualIdxToOrigIdxMap(const vector<bool>& con_relax_vector){

    //dual idx is assigned as the count index
    int con_relaxation_count = 0;
    for (int orig_con_idx=0; orig_con_idx<con_relax_vector.size(); ++orig_con_idx){
        if (con_relax_vector[orig_con_idx] == true){
            dual_idx_to_orig_constraint_idx_map[con_relaxation_count] = orig_con_idx;
            orig_constraint_idx_to_dual_idx_map[orig_con_idx] = con_relaxation_count;
            ++con_relaxation_count;
        }
    }
}

// Initialise the various subproblems based on the partitions created via relaxing constraints
void ConDecomp_LaPSO_Connector::initSubproblems(const vector<Partition_Struct>& partitions)
{
    int subproblem_idx = 0;
    for (auto& partition : partitions) {
        if (subproblem_idx % 10000 == 0) {
            cout << "setting up subproblem " << subproblem_idx << endl;
        }

        CPLEX_MIP_Subproblem sp;

        sp.setSubproblemIdx(subproblem_idx);
        
        // creatre a new IloEnv object which is used for memory management of Ilo objects
        sp.envPtr = new IloEnv;
        // IloEnv *(sp.envPtr);
        IloModel model(*(sp.envPtr));
        IloNumVarArray subproblem_vars_cplex(*(sp.envPtr));
        IloRangeArray subproblem_constraints_cplex(*(sp.envPtr));
        // unordered_map<int, int> subproblemVarIdx_to_originalVarIdx;
        // unordered_map<int, int> originalVarIdx_to_subproblemVarIdx;

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
            sp.subproblemVarIdx_to_originalVarIdx;
            sp.originalVarIdx_to_subproblemVarIdx;
            sp.num_subproblem_vars = subproblem_var_idx;
            if (sp.envPtr != nullptr) {
                if ((*(sp.envPtr)).getImpl() != nullptr) {
                    (*(sp.envPtr)).end();
                }
                delete sp.envPtr;
                sp.envPtr = nullptr;
            }

            // (*(sp.envPtr)).end();
            MS.push_back(sp);
            continue;
        }

        if (debug_printing){
             std::cout << "added in all variables to subproblem " << endl;  
        }
       

        //add constraints to subproblem
        vector<Constraint> subproblem_constraints;
        if (!partition.edge_idxs.empty()) {
            for (auto& constraint_idx : partition.edge_idxs) {
                subproblem_constraints.push_back(OP.constraints[constraint_idx]);
            }

            //add constraints to the model
            for (auto& constraint : subproblem_constraints) {
                IloExpr constraint_exp(*(sp.envPtr));

                for (auto& term : constraint.getConTerms()) {
                    int var_idx = term.first;
                    double var_coeff = term.second;
                    int var_idx_in_sp = sp.originalVarIdx_to_subproblemVarIdx[var_idx];
                    constraint_exp += (var_coeff * subproblem_vars_cplex[var_idx_in_sp]);
                }

                if (constraint.getBoundType() == Greater) {
                    IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, INF);
                    subproblem_constraints_cplex.add(r1);
                } else if (constraint.getBoundType() == Equal) {
                    IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, constraint.getRHS());
                    subproblem_constraints_cplex.add(r1);
                } else if (constraint.getBoundType() == Less) {
                    IloRange r1(*(sp.envPtr), -INF, constraint_exp, constraint.getRHS());
                    subproblem_constraints_cplex.add(r1);
                }
            }
            model.add(subproblem_constraints_cplex);
        }

        sp.model = model;
        sp.variables = subproblem_vars_cplex;
        sp.num_subproblem_vars = subproblem_var_idx;
        MS.push_back(sp);

        ++subproblem_idx;
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

    // // for each dualised constraint, update the red cost by dual[i] * coeff in constraint
    // for (auto& constraint : OP.constraints) {
    //     int constraint_idx = constraint.getConIdx();
    //     for (auto& con_term : constraint.getConTerms()) {
    //         int var_idx = con_term.first;
    //         double var_coeff = con_term.second;
    //         s.rc[var_idx] += s.dual[constraint_idx] * (-1 * var_coeff);
    //     }
    // }

    for (int dual_idx = 0; dual_idx < s.dual.size(); ++dual_idx){
        int original_constraint_idx = dual_idx_to_orig_constraint_idx_map[dual_idx];
        Constraint con = OP.constraints[original_constraint_idx];
        cout << "for dual idx = " << dual_idx << endl;
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
// returns 0 for success, -1 for failure
int ConDecomp_LaPSO_Connector::solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, DblVec& rc, DblVec& x)
{
    int ret_val = 0;
    //if subproblem size is 1 solve without cplex
    if (sp.subproblemVarIdx_to_originalVarIdx.size() == 1) {
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[0];
        double coeff = rc[original_var_idx];
        
        Variable var = OP.getVariable(original_var_idx);

        double upper_bound = var.getUB();
        double lower_bound = var.getLB();

        double mip_soln = 0;
        // if negative coeff, set to max var bound, else if positive coefficient set to lower bound
        if (coeff < 0) {
            x[original_var_idx] = upper_bound;
        } else{
            x[original_var_idx] = lower_bound;
        }
        int subproblem_idx = sp.getSubproblemIdx();
        int mip_soln_time = 0;
        int lp_soln_time = 0;

        // capture the solution statistics
        subproblem_statistics_ptr->mip_times.push_back(0);
        subproblem_statistics_ptr->lp_times.push_back(0);
        subproblem_statistics_ptr->mip_obj_solutions.push_back(coeff * x[original_var_idx]);
        subproblem_statistics_ptr->lp_obj_solutions.push_back(coeff * x[original_var_idx]);
        return 0;
    }

    // otherwise, create the new objective function using the reduced costs
    // add in objective function to the model...
    IloExpr obj_exp(sp.model.getEnv());
    for (int i = 0; i < sp.variables.getSize(); i++) {
        //original index -- get coeff
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        double coeff = rc[original_var_idx];
        obj_exp += (coeff * sp.variables[i]);
    }

    IloObjective obj_fn = IloMinimize(sp.model.getEnv(), obj_exp);
    sp.model.add(obj_fn);

  
    // solve the subproblem as a MIP and get the statistics
    IloCplex cplex(sp.model);
    cplex.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    cplex.setParam(IloCplex::TiLim, sp_solve_time_limit);
    cplex.setOut((*(sp.envPtr)).getNullStream());
    bool feasible_sol = cplex.solve();
    
    // if MIP subproblem is not solved to optimality
    if ((cplex.getStatus() != CPX_STAT_OPTIMAL) && (cplex.getStatus() != CPXMIP_OPTIMAL)){
        cout << "Failed to find optimal MIP subproblem solution in subpoblem: " << sp.getSubproblemIdx() << endl;
        // if no feasible solution is found
        subproblem_statistics_ptr->subproblem_solve_success.push_back(false);
        if (!feasible_sol){
            cout << "Failed to find feasible subproblem solution" << endl;
            subproblem_statistics_ptr->mip_obj_solutions.push_back(-INF);
        }
        sp.model.remove(obj_fn);
        cplex.end();
        ret_val = -1;
    }
    else{
        // capture the mip solution quality
        subproblem_statistics_ptr->mip_obj_solutions.push_back(cplex.getObjValue());
        subproblem_statistics_ptr->subproblem_solve_success.push_back(true);
    }

    //capture the mip runtime in cpu seconds
    subproblem_statistics_ptr->mip_times.push_back(cplex.getTime());

    // solve the subproblem as a LP and get the statistics
    IloModel relax(*sp.envPtr);
    relax.add(sp.model);
    relax.add(IloConversion(*sp.envPtr, sp.variables, ILOFLOAT)); 

    IloCplex cplex_relaxed(relax);
    cplex_relaxed.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    cplex_relaxed.setParam(IloCplex::TiLim, sp_solve_time_limit);
    cplex_relaxed.setOut((*(sp.envPtr)).getNullStream());
    // not able to solve LP subproblem to optimal
    if ((cplex.getStatus() != CPX_STAT_OPTIMAL)) {
        cout << "Failed to find optimal subproblem LP Solution" << endl;
        sp.model.remove(obj_fn);
        cplex_relaxed.end();
        ret_val = -1;
    }

     //capture the lp runtime in cpu seconds
    subproblem_statistics_ptr->lp_times.push_back(cplex_relaxed.getTime());
    // capture the milp solution quality
    subproblem_statistics_ptr->lp_obj_solutions.push_back(cplex_relaxed.getObjValue());
    // end the LP relaxed environment to free up memory
    cplex_relaxed.end();
    

    // capture the variable values from the subproblem solutions
    for (int i = 0; i < sp.variables.getSize(); i++) {
        int orig_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        try {
            IloNum val = cplex.getValue(sp.variables[i]);
            Variable_Type vt = OP.variables[orig_idx].getVarType();
            if (vt == Int || vt == Bin) {
                // in case of rounding errors, add in slight perturbation and then convert to int to round down
                int x_val = (int)(val + 0.1);
                x[orig_idx] = x_val;
            } else if (vt == Cont) {
                x[orig_idx] = val;
            }
        // } catch (IloException& e) {
        //     int x_val = 0;
        //     x[orig_idx] = x_val;
        //     // cout << e << endl;
        //     // cout << "number of variables is " << sp.variables.getSize() << endl;
        } catch (IloException& e) {
            cout << "e" << endl;
            cout << "Exception caught in ConDecomp_LaPSO_Connector::solveSubproblemCplex()" << endl;
        }
    }
    sp.model.remove(obj_fn);
    cplex.end();
    return ret_val;
}

void ConDecomp_LaPSO_Connector::updateParticleLB(ConDecomp_LaPSO_Connector_Solution& s)
{   
    // lb = cx + \lambda(b-Ax) = (c- \lambda * A)x + \lambda * b 
    // lb = s.rc * s.x + s.dual* b
    
    double lb = 0;
    for (int var_idx = 0; var_idx < s.x.size(); var_idx++) {
        lb += (s.x[var_idx] * s.rc[var_idx]);
        if (debug_printing){
            cout << "for nsolves " << nsolves << " x[" << var_idx << "] is: " << s.x[var_idx] << endl;
            cout << "for nsolves " << nsolves << " rc[" << var_idx << "] is: " << s.rc[var_idx] << endl;
        }
    }
    for (int dual_idx = 0; dual_idx < s.dual.size(); dual_idx++) {
        int constraint_orig_idx = dual_idx_to_orig_constraint_idx_map[dual_idx];
        double constraint_bound = OP.constraints[constraint_orig_idx].getRHS();
        lb += (s.dual[dual_idx] * constraint_bound);
        if (debug_printing){
            cout << "for nsolves " << nsolves << " dual[" << dual_idx << "] is: " << s.dual[dual_idx] << endl;
        }
    }
    s.lb = lb;
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

    int subproblem_solve_error = 0;
    // for each subproblem, feed in new objective function, solve and update x
    for (auto& subproblem : MS) {
        int subproblem_status = solveSubproblemCplex(subproblem, s.rc, s.x);
        if (subproblem_status == -1) {
            subproblem_solve_error = 1;
            break;
        }
    }

    // if one or more subproblems can't be solved, set the output as -INF (for minimisation problems)
    if (subproblem_solve_error){
        s.lb = -INF;
    }
    else{
        updateParticleViol(s);
        updateParticleLB(s);
    }
    

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
    for (auto& subproblem : MS) {
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