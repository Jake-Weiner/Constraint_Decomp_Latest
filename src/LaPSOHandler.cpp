#include "LaPSOHandler.h"
#include "Random.h"


LaPSOHandler::LaPSOHandler(int& argc, const char** argv, LaPSO::LaPSORequirements& lr){

    initSolver(argc, argv, lr);

}

void LaPSOHandler::initSolver(int& argc, const char** argv,LaPSO::LaPSORequirements& lr){


    //initialise the solver 
    solver.initProblem(lr); // 
    
    // override default parameter values with command line arguments
    solver.param.parse(argc, argv);


    // solver.best->lb = -INF;
    // // solver.best.ub = best_ub_sol;
    // solver.best->ub = best_ub_sol;
    // solver.best. = -INF; // all constraints are <= so lagrange multipliers are <= 0
    
    bool printing = solver.param.printLevel > 0;
    // Uniform rand;
    // if (solver.param.randomSeed == 0)
    //     rand.seedTime();
    // else
    //     rand.seed(solver.param.randomSeed);

}

void LaPSOHandler::solve(ConDecomp_LaPSO_Connector& CLC){

    solver.solve(CLC);
    // // // // // // // // // // // \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\
    // end all CPLEX environments and clear all subproblem information
    CLC.endCplexEnvs();

    //if (printing == true) { // always show the final result
    std::cout << "Best Upper Bound solution  " << solver.best_solution->ub
              << " , Best lower bound  solution " << solver.best_solution->lb
              << std::endl
              << "CPU time = " << solver.cpuTime()
              << " elapsed = " << solver.wallTime() << " sec"
              << " primal cpu time " << solver.primal_cpu_time
              << " dual cpu time " << solver.dual_cpu_time
              << std::endl;

    // print out dual variable solutions

    for (int dual_idx = 0; dual_idx<solver.best_solution->dual.size(); ++dual_idx){
        std::cout << "constraint idx: " << CLC.getOriginalConIdx(dual_idx)
        << " with dual value: " << solver.best_solution->dual[dual_idx] << std::endl;
    }

}