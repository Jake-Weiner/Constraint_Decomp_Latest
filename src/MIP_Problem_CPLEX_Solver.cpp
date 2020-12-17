#include "MIP_Problem_CPLEX_Solver.h"
#include <ilcplex/ilocplex.h>
#include <algorithm>


using std::cout;
using std::vector;

    //This method returns a bound on the optimal solution value of the problem. When a model has been solved to optimality, 
    //this value matches the optimal solution value. If a MIP optimization is terminated before optimality has been proven, 
    //this value is computed for a minimization (maximization) problem as the minimum (maximum) objective function value of 
    //all remaining unexplored nodes.

CPLEX_Return_struct MIP_Problem_CPLEX_Solver::solve(bool randomSeed, bool LP)
{
    IloEnv env;
    IloModel model(env);
    IloNumVarArray subproblem_vars_cplex(env);
    IloRangeArray subproblem_constraints_cplex(env);

    if (LP){
        cout << "solving MIP Problem as an LP" << endl;
    }
    // add all variables in original problem to CPLEX environment
    cout << "number of variables in mip are " << MP.getNumVariables() << endl;
    cout << "adding in variables" << endl;
    for (const auto& var : MP.variables) {
        // cout << "var idx is " << var.getVarIndx() << endl;
        double var_lb = var.getLB();
        double var_ub = var.getUB();
        if ((var.getVarType() == Int || var.getVarType() == Bin) && (!LP)){
            IloNumVar x(env, var_lb, var_ub, ILOINT);
            subproblem_vars_cplex.add(x);        
        }
        // if the problem is to be solved as a LP, set all variables to float types
        else if(var.getVarType() == Cont || LP){
            IloNumVar x(env, var_lb, var_ub, ILOFLOAT);
            subproblem_vars_cplex.add(x); 
        }
    }

    cout << "adding in constraints" << endl;
    //add all constraints to the model
    int constraints_added = 0;
    for (auto& constraint : MP.constraints) {
        IloNumExpr constraint_exp(env);
        for(auto& term: constraint.getConTerms()){
            int var_idx = term.first;
            double var_coeff = term.second;
            constraint_exp += (var_coeff * subproblem_vars_cplex[var_idx]);
        }
        // cout << "const exp is " << constraint_exp << " : RHS is " << constraint.getRHS() << endl;
        double RHS = constraint.getRHS();
        if (constraint.getBoundType() == Greater) {
            // IloRange r1(env, RHS, constraint_exp, std::numeric_limits<double>::max());
            IloRange r1(env, RHS, constraint_exp, std::numeric_limits<double>::max());
            subproblem_constraints_cplex.add(r1);
            ++constraints_added;
        } else if (constraint.getBoundType() == Equal) {
            IloRange r1(env, RHS, constraint_exp, RHS);
            subproblem_constraints_cplex.add(r1);
            ++constraints_added;
        } else if (constraint.getBoundType() == Less) {
            // IloRange r1(env, -infinity,constraint_exp, RHS);
            IloRange r1(env, -std::numeric_limits<double>::max(),constraint_exp, RHS);
            subproblem_constraints_cplex.add(r1);
            ++constraints_added;
        }
      
    }
    cout << "number of constraints added is " << constraints_added << endl;
    model.add(subproblem_constraints_cplex);
    
    cout << "creating obj function" << endl;
    // add objective function to cplex model
    IloNumExpr obj_exp(env);
    int obj_term_count = 0;
    for (auto& obj_term : MP.objective_fn){
        int var_idx = obj_term.first;
        double coeff = obj_term.second;
        obj_exp += (coeff * subproblem_vars_cplex[var_idx]);
        ++obj_term_count;
    }
    cout << "number of objective function terms is " << obj_term_count << endl;
    IloObjective obj_fn = IloMinimize(env, obj_exp);
    model.add(obj_fn);

    cout << "creating cplex object" << endl;
    IloCplex cplex(model);
    cplex.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    // if solving as a ILP, set a time limit, otherwise solve the LP to optimality
    if (!LP){
        cplex.setParam(IloCplex::TiLim, solver_time_lim); // solve for solver_time_lim
    }
    cplex.setParam(IloCplex::ClockType, 1); // use CPU runtime not wallclock.
    
    if (randomSeed == false){
        cplex.setParam(IloCplex::Param::RandomSeed, 0); // set random seed to 0 for consistency in testing
    }

    // solving mip
    cout << "solving MIP" << endl;
    bool solve_status = cplex.solve();
    cout << "finished Solving MIP" << endl;
    vector<double> dual_vals;
    vector<int> basic_variable_idxs;
    if (LP){
        cout << "Collecting Dual values " << endl;
        IloNumArray dual_values_arr(env);
        // places the dual values from the cplex constraint model into dual_values_arr
        cplex.getDuals(dual_values_arr,subproblem_constraints_cplex);
        dual_vals.resize(MP.getNumConstraints(), 0.0);
        // copy dual values into vector
        for (int con_idx = 0; con_idx < dual_values_arr.getSize(); ++con_idx){
            dual_vals[con_idx] = dual_values_arr[con_idx];
        }

    // store the basic variable indexes
    for (int var_idx = 0; var_idx < MP.getNumVariables(); ++var_idx){
        try{
            if (cplex.getBasisStatus(subproblem_vars_cplex[var_idx]) ==  IloCplex::BasisStatus::Basic){
                basic_variable_idxs.push_back(var_idx);
                }
        }
        catch(IloCplex::Exception e){
            cout << "error status is " << e.getStatus() << " with message " << e.getMessage() << endl;
            cout << "error found when testing whether var idx " << var_idx << " is basic " << endl;
            cout << "number of variables is " << MP.getNumVariables() << endl;
        }
    }
    cout << "collected basic variables status" << endl;
    }
    
   
    //default bound and integer solution values
    double best_primal_sol = -999999999999999999;
    double best_bound_val = cplex.getBestObjValue();
    double solve_time = cplex.getTime();

    // cplex.solve() status indicates if a feasible solution was found
    if (solve_status == false) {
        env.end();
        std::cerr << "CPLEX failed to find a feasible solution in MIP_Problem_CPLEX_Solver.cpp" << endl;
    }
    else{
        best_primal_sol = cplex.getObjValue();
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        env.end();
    }
   
    
    CPLEX_Return_struct CPLEX_results = {best_bound_val, best_primal_sol, solve_time,basic_variable_idxs, dual_vals};
    return CPLEX_results;
}

