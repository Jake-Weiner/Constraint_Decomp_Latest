#include "SolveGenericMIP.h"
#include <ilcplex/ilocplex.h>
#include <algorithm>
#include "Util.h"

using std::cout;
using std::endl;

CPLEX_Return_struct SolveGenericMIP::solve(bool randomSeed){

    bool solve_as_LP = false;
    IloEnv env;
    IloModel model(env);
    IloCplex cplex(env);
    IloObjective obj;
    IloNumVarArray vars(env);
    IloRangeArray rng(env);
    
    // solve problem as LP not ILP

    if (!fileExists(MIP_input_filename)){
        cout << "error, MIP input filename does not exist " << endl;
        exit(1);
    }
    cplex.importModel(model, MIP_input_filename.c_str(), obj, vars, rng);
    if (solve_as_LP){
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
    if (!cplex.solve()) {
        env.error() << "Failed to optimize original problem LP" << endl;
        env.end();
        std::cerr << "CPLEX failure in SolveGenericMIP.cpp" << endl;
        exit(1);
    }
    IloNumArray vals(env);
    cplex.getValues(vals, vars);
    env.out() << "Solution status = " << cplex.getStatus() << endl;
    env.out() << "Solution value  = " << cplex.getObjValue() << endl;
    // double obj_val = double(cplex.getObjValue());
    CPLEX_Return_struct CPLEX_results = {cplex.getBestObjValue(), cplex.getObjValue()};
    env.end();

    
    return CPLEX_results;


}