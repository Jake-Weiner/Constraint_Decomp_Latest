#include "SolveLaPSO.h"
#include "Random.h"

SolveLaPSO::SolveLaPSO(int& argc, const char** argv, int num_var, int num_con, const double& best_ub_sol,
        LaPSO::constraint_type_indicies& cti){
    initSolver(argc, argv, num_var,  num_con, best_ub_sol, cti);

}

void SolveLaPSO::initSolver(int& argc, const char** argv,int num_var, int num_con, const double& best_ub_sol,
        LaPSO::constraint_type_indicies& cti){
    solver.initProblem(num_var, num_con, cti, best_ub_sol); // number of variables & (relaxed) constraints
    //-------- set default parameter values
    // override defaults with command line arguments
    solver.param.parse(argc, argv);


    solver.best->lb = -INF;
    // solver.best.ub = best_ub_sol;
    solver.best->ub = best_ub_sol;
    // solver.best. = -INF; // all constraints are <= so lagrange multipliers are <= 0
    
    bool printing = solver.param.printLevel > 0;
    Uniform rand;
    if (solver.param.randomSeed == 0)
        rand.seedTime();
    else
        rand.seed(solver.param.randomSeed);

}

void SolveLaPSO::solve(ConDecomp_LaPSO_Connector& CLC){

    solver.solve(CLC);

    // end all CPLEX environments and clear all subproblem information
    CLC.endCplexEnvs();

    //if (printing == true) { // always show the final result
    std::cout << "Best Upper Bound solution  " << solver.best->ub
              << " , Best lower bound  solution " << solver.best->lb
              << std::endl
              << "CPU time = " << solver.cpuTime()
              << " elapsed = " << solver.wallTime() << " sec"
              << " primal cpu time " << solver.primal_cpu_time
              << " dual cpu time " << solver.dual_cpu_time
              << std::endl;

}