#include "SolveGenericMIP.h"
#include <ilcplex/ilocplex.h>
#include <algorithm>
#include "Util.h"

using std::cout;
using std::endl;

CPLEX_Return_struct SolveGenericMIP::solve(bool randomSeed, bool LP){

    IloEnv env;
    IloModel model(env);
    IloCplex cplex(env);
    IloObjective obj;
    IloNumVarArray vars(env);
    IloRangeArray rng(env);

    // solve problem as LP not ILP

    if (!fileExists(MIP_input_filename)){
        cout << "error,MIP input filename - "<< MIP_input_filename << " does not exist" << endl;
        exit(1);
    }
    cplex.importModel(model, MIP_input_filename.c_str(), obj, vars, rng);
    if (LP){
        for (int i =0; i<vars.getSize(); i++){
        model.add(IloConversion(env, vars[i], ILOFLOAT));
        }
    }
    
    // cplex.setParam(IloCplex::Param::MIP::Limits::Solutions, 1);
    cplex.setParam(IloCplex::TiLim, solver_time_lim);
    cplex.setParam(IloCplex::Threads, 1);
    cplex.setParam(IloCplex::ClockType, 1); // use CPU runtime not wallclock.
    if (randomSeed == true){
        cplex.setParam(IloCplex::Param::RandomSeed, 0); // set random seed to 0 for consistency in testing
    }
    // cplex.setParam(IloCplex::RootAlg,IloCplex::Dual);
    // cplex.setParam(IloCplex::Param::MIP::Limits::Nodes, 0);
    cplex.extract(model);
    // cplex.exportModel(new_filename.c_str());

    bool solve_status = cplex.solve();

    //default bound and integer solution values
    double best_int_sol = -999999999999999999;
    double best_bound_val = -999999999999999999;
    // cplex.solve() status indicates if a feasible solution was found
    if (solve_status == false) {
        best_bound_val = cplex.getBestObjValue();
        cout << cplex.getBestObjValue() << endl;
        env.end();
        std::cerr << "CPLEX failed to find a feasible solution in SolveGenericMIP.cpp" << endl;
    }
    else{
        best_int_sol = cplex.getObjValue();
        best_bound_val = cplex.getBestObjValue();
        env.out() << "Solution value  = " << cplex.getObjValue() << endl;
        env.end();
    }
    // if you want to get the values of individual variables
    // IloNumArray vals(env);
    // cplex.getValues(vals, vars);
    // env.out() << "Solution status = " << cplex.getStatus() << endl;
 
    // double obj_val = double(cplex.getObjValue());
    CPLEX_Return_struct CPLEX_results = {best_bound_val, best_int_sol};
   
    
    return CPLEX_results;


}