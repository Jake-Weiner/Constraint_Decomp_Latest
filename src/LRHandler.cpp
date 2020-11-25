#include "LRHandler.h"
#include "Random.h"




void LRHandler::initLaPSOSolver(int& argc, const char** argv, LaPSO::LaPSORequirements& lr){
    //initialise the solver_ptr 
    solver_ptr->initProblem(lr); // 
    // override default parameter values with command line arguments
    solver_ptr->param.parse(argc, argv);
    bool printing = solver_ptr->param.printLevel > 0;
    LaPSO_initialised = true;
      
}

void LRHandler::initConnector(ConnectorRequirements& CR){
    
    // *CR.MP_ptr, *CR.ps, *CR.con_relax_vector, CR.debug_printing, CR.total_LR_time_lim, CR.ss_ptr
    CLC_ptr = std::make_shared<ConDecomp_LaPSO_Connector>(*CR.MP_ptr, *CR.ps, *CR.con_relax_vector, CR.debug_printing, CR.total_LR_time_lim, CR.ss_ptr);
    Connector_initialised = false;
}


void LRHandler::solve(){

    if (!LaPSO_initialised || !Connector_initialised){
        cout << "LaPSO or Connector has not been initialised " << endl;
        exit(EXIT_FAILURE);
    }

    solver_ptr->solve(*CLC_ptr);
    //if (printing == true) { // always show the final result
    std::cout << "Best Upper Bound solution  " << solver_ptr->best_solution->ub
              << " , Best lower bound  solution " << solver_ptr->best_solution->lb
              << std::endl
              << "CPU time = " << solver_ptr->cpuTime()
              << " elapsed = " << solver_ptr->wallTime() << " sec"
              << " primal cpu time " << solver_ptr->primal_cpu_time
              << " dual cpu time " << solver_ptr->dual_cpu_time
              << std::endl;

    // print out dual variable solutions

    // for (int dual_idx = 0; dual_idx<solver_ptr.best_solution->dual.size(); ++dual_idx){
    //     std::cout << "constraint idx: " << CLC.getOriginalConIdx(dual_idx)
    //     << " with dual value: " << solver_ptr.best_solution->dual[dual_idx] << std::endl;
    // }

}

std::tuple<double,double> LRHandler::getLaPSOOutputs(){

    return std::make_tuple(solver_ptr->best_solution->lb, solver_ptr->cpuTime());
}