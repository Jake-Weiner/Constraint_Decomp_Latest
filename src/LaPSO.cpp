/// Generic solver for combinatorial optimisation problems using a combination
/// of Wedelin'current_solution lagrangian relaxation heuristic and Solution swarm
/// optimisation. This header file follows the general pattern of the Volume
/// Algorithm implementation in the COIN-OR library to make it easy to swap
/// algorithms and make comparisons between the two methods.
///
/// The current implementation only allows for binary variables.
///
/// Autor: Andreas Ernst   $ Date: January 2010 $  $ Revision: 0.0 $
#include "LaPSO.hpp"
#include "Random.h"
#include "anyoption.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <time.h>

#ifdef _OPENMP
#include <omp.h>
#else
#include <time.h>
#define omp_get_wtime() (double)time(NULL)
#define omp_set_num_threads(n) // do nothing
#endif

namespace LaPSO {
lagged_fibonacci1279 _randGen;
void Param::parse(int argc, const char** argv)
{
    AnyOption parser(100);
    parser.setFlag("help");
    parser.setFlag("randomSeed");
    parser.setOption("subgradFactor");
    parser.setOption("subgradFmult");
    parser.setOption("subgradFmin");
    parser.setOption("perturbFactor");
    parser.setOption("velocityFactor");
    parser.setOption("globalFactor");
    parser.setOption("maxIter");
    parser.setOption("maxCPU");
    parser.setOption("maxWallTime");
    parser.setOption("nParticles");
    parser.setOption("printLevel");
    parser.setOption("printFreq");
    parser.setOption("heurFreq");
    parser.setOption("eps");
    parser.setOption("absGap");
    parser.setOption("relGap");
    parser.setOption("nCPU");
    parser.setOption("printLB");
    parser.setOption("LBOutputFilename");
    if (argc == -1) // abuse of function
        parser.processFile(*argv);
    else
        parser.processCommandArgs(argc, argv);
    if (parser.getFlag("help"))
        parser.printAutoUsage();
    if (parser.getFlag("randomSeed"))
        randomSeed = 0; //
    if (parser.getValue("subgradFactor"))
        subgradFactor = atof(parser.getValue("subgradFactor"));
    if (parser.getValue("subgradFmult"))
        subgradFmult = atof(parser.getValue("subgradFmult"));
    if (parser.getValue("subgradFmin"))
        subgradFmin = atof(parser.getValue("subgradFmin"));
    if (parser.getValue("maxIter"))
        maxIter = atoi(parser.getValue("maxIter"));
    if (parser.getValue("maxCPU"))
        maxCPU = atof(parser.getValue("maxCPU"));
    if (parser.getValue("maxWallTime"))
        maxWallTime = atof(parser.getValue("maxWallTime"));
    if (parser.getValue("printLevel"))
        printLevel = atoi(parser.getValue("printLevel"));
    if (parser.getValue("printFreq"))
        printFreq = atoi(parser.getValue("printFreq"));
    if (parser.getValue("heurFreq"))
        heurFreq = atoi(parser.getValue("heurFreq"));
    if (parser.getValue("eps"))
        eps = atof(parser.getValue("eps"));
    if (parser.getValue("absGap"))
        absGap = atof(parser.getValue("absGap"));
    if (parser.getValue("relGap"))
        relGap = atof(parser.getValue("relGap"));
    if (parser.getValue("nCPU"))
        nCPU = std::max(1, atoi(parser.getValue("nCPU")));
    if (parser.getValue("printLB"))
        printLB = atoi(parser.getValue("printLB"));
    if (parser.getValue("LBOutputFilename"))
        LBOutputFilename = parser.getValue("LBOutputFilename");
}
void Param::parse(const char* filename) { parse(-1, &filename); }

void Solution::resize(size_t numVar, size_t numConstr)
{
    dual.resize(numConstr, 0.0);
    x.resize(numVar, 0);
    viol.resize(numConstr, 0.0);
    rc.resize(numVar, 0.0);
    isFeasible = false;
    ub = INF;
    lb = -INF;
}

Solution::Solution(const Solution& other)
{
    this->dual = other.dual;
    this->x = other.x;
    this->viol = other.viol;
    this->rc = other.rc;
    this->isFeasible = other.isFeasible;
    this->ub = other.ub;
    this->lb = other.lb;
}

// Solution& Solution::operator=(const Solution& other){

//     // this->dual = other.dual;
//     // this->x = other.x;
//     // this->viol = other.viol;
//     // this->rc =other.rc;
//     // this->isFeasible = other.isFeasible;
//     // this->ub = other.ub;
//     // this->lb = other.lb;
// }

// void Solution::swap(Solution& rhs){

//     std::swap(this->dual, rhs.dual);

//     rhs.dual = this->dual;

// }

//revisit if this is necessary
Problem::Problem(int nVar, int nConstr)
    : psize(nVar)
    , dsize(nConstr)
    , dualLB(dsize, -INF)
    , dualUB(dsize, INF)
    , nIter(-1)
{
}

void Problem::initProblem(const LaPSORequirements& LR)
{
    psize = LR.nVar;
    dsize = LR.nConstr;
    dualLB.resize(dsize, -INF);
    dualUB.resize(dsize, INF);
    // x_total.resize(psize, 0);
    omp_set_num_threads(param.nCPU);

    nIter = -1;
    _wallTime = omp_get_wtime();

    // create the best_solution and current_solution Solution objects

    current_solution = std::make_shared<Solution>(psize, dsize);

    // // current_solution = new Solution(psize, dsize);
    best_solution = std::make_shared<Solution>(psize, dsize);
    

    best_solution->isFeasible = false;
    best_solution->dual.resize(LR.nConstr, 0.0);
    best_solution->x.resize(psize, 0);

    // use a predetermined ub?
    if (LR.benchmark_ub_flag) {
        best_solution->ub = LR.best_ub;
        this->best_ub = LR.best_ub;
        this->benchmark_ub_flag = LR.benchmark_ub_flag;
    }

    best_solution->lb = -INF;

    // set the dual bounds based on constraint types
    setDualBoundsEqual(LR.cti.equality_idxs);
    setDualBoundsLesser(LR.cti.less_than_idxs);
    setDualBoundsGreater(LR.cti.greater_than_idxs);

    if (LR.set_initial_dual_values){
        for (const auto& dual_pair : LR.intial_dual_value_pairs){
            current_solution->dual[dual_pair.first] = dual_pair.second;
        }
    }
}

void Problem::setDualBoundsEqual(const std::vector<int>& idxs)
{

    for (auto& idx : idxs) {
        // printf("constraint idx is %d", idx);
        dualLB[idx] = -INF;
        dualUB[idx] = INF;
    }
}

void Problem::setDualBoundsLesser(const std::vector<int>& idxs)
{
    for (auto& idx : idxs) {
        // printf("constraint idx is %d", idx);
        dualLB[idx] = -INF;
        dualUB[idx] = 0;
    }
}
void Problem::setDualBoundsGreater(const std::vector<int>& idxs)
{
   
    for (auto& idx : idxs) {
        // printf("constraint idx is %d", idx);
        dualLB[idx] = 0;
        dualUB[idx] = INF;
    }
}

// Main method
void Problem::solve(UserHooks& hooks)
{
    _wallTime = omp_get_wtime();
    timer.reset();
    double improv_thresh = 1.01;
    const int lbCheckFreq = 10;

    //------------- initial solution for swarm --------------

    // initial_dual_min = current_solution->dual.min();
    // initial_dual_max = current_solution->dual.max();

    // if (param.printLevel > 1) {
    //     printf("initial dual min is %f:\n", initial_dual_min);
    //     printf("initial dual max is %f:\n", initial_dual_max);
    // }
    double bestLB;
    printf("rc size is %f", current_solution->rc.size());
    Uniform rand; //

    if (param.randomSeed != 0)
        rand.seed(param.randomSeed);
    else
        rand.seedTime();

    Status status = OK;

    for (int i = 0; i < dsize; ++i) { // check initial point is valid
        //check the dual values
        if (current_solution->dual[i] < dualLB[i]) {
            if (param.printLevel)
                printf("ERROR: initial dual[%d] %.2f below LB %.2f\n",
                    i, current_solution->dual[i], dualLB[i]);
            current_solution->dual[i] = dualLB[i];
            IIDI.lower_bound_errors.push_back({i,current_solution->dual[i]});
        }
        if (current_solution->dual[i] > dualUB[i]) {
            if (param.printLevel)
                printf("ERROR: initial dual[%d] %.2f below UB %.2f\n",
                    i, current_solution->dual[i], dualUB[i]);
            current_solution->dual[i] = dualUB[i];
            IIDI.upper_bound_errors.push_back({i,current_solution->dual[i]});
        }
    }

    // update the reduced costs which are used to solve the subproblems
    if (hooks.reducedCost(*current_solution) == ABORT) {
        status = ABORT;
    }

    //solve the subproblems / partitions using the reduced costs
    if (hooks.solveSubproblem(*current_solution) == ABORT) {
        status = ABORT;
    }

    printf("Status is %d", status);
    
    if (param.printLevel > 1) {
        printf("\t LB=%g UB=%g feas=%d viol=%g -- %g\n",
            current_solution->lb, current_solution->ub, current_solution->isFeasible,
            current_solution->viol.min(), current_solution->viol.max());
        if (param.printLevel > 2) {
            printf("\t\tRedCst %g - %g\n", current_solution->rc.min(), current_solution->rc.max());
            printf("\t\tdual %g - %g\n", current_solution->dual.min(), current_solution->dual.max());
        }
    }

    updateBest(hooks, 0);
    if (status == ABORT)
        return;

    double subgradFactor = param.subgradFactor;

    if (param.printLevel > 1)
        printf("Initial LB=%g UB=%g\n", best_solution->lb, best_solution->ub);
    int noImproveIter = 0, nReset = 0, maxNoImprove = 1000;

    // int current_lb_time_limits_idx = 0;
    // double last_lb_comparison;

    // best_lb_tracking.push_back(best_solution->lb);
    // double average_lb = current_solution->lb;
    // average_lb_tracking.push_back(average_lb);
    // timing_tracking.push_back(cpuTime());

    // initial best lower_bound
    bestLB = current_solution->lb;

    for (nIter = 1; nIter < param.maxIter && cpuTime() < param.maxCPU && wallTime() < param.maxWallTime && status == OK && (best_solution->lb + param.absGap <= best_solution->ub) && fabs(best_solution->ub - best_solution->lb / best_solution->ub) > param.relGap;
         ++nIter) {

        if (param.printLevel > 1 && nIter % param.printFreq == 0) {
            printf("iter num = %d\n", nIter);
            printf("best_solution lower bound so far is %f \n", best_solution->lb);
        }

        // for (int dual_idx =0; dual_idx < best_solution->dual.size(); ++dual_idx){
        //     cout << "Dual[" << dual_idx << "] = " <<  best_solution->dual[dual_idx] << std::endl;
        // }

        // cpu time is within the limit
        // best_lb_tracking.push_back(best_solution->lb);
        // double average_lb = current_solution->lb;
        // average_lb_tracking.push_back(average_lb);
        // timing_tracking.push_back(cpuTime());
        if (param.printLevel > 1 && nIter % param.printFreq == 0)
            printf("Iteration %d --------------------\n", nIter);

        // --------- calculate step size/direction ----------
        double norm = 0;
        for (int i = 0; i < dsize; ++i)
            norm += current_solution->viol[i] * current_solution->viol[i];

        // calculate the step size (Z* - Z(u_k) / (||norm||))
        const double stepSize = (subgradFactor * (best_ub - current_solution->lb)) / norm;
        std::cout << "Step Size is " << stepSize << endl;
        //---------- make a step ------------------------------
        for (int i = 0; i < dsize; ++i) {

            current_solution->dual[i] += stepSize * current_solution->viol[i];
            // cout << "viol[" << i << "] is now " << current_solution->viol[i] << endl;
            // cout << "dual[" << i << "] is now " << current_solution->dual[i] << endl;
            current_solution->dual[i] = std::max(dualLB[i], std::min(dualUB[i], current_solution->dual[i]));

            // cout << "dual[" << i << "] is now " << current_solution->dual[i] << endl;
        }
        //---------- update reduced costs -------------------------
        if (hooks.reducedCost(*current_solution) == ABORT) {
            status = ABORT;
            continue;
        }

        // solve subproblems based on updated reduced costs
        if (hooks.solveSubproblem(*current_solution) == ABORT) {
            status = ABORT;
            continue;
        }

        // if the current solution lower bound has not improved over the last lbCheckFreq iterations,
        // reduce the subgradFactor

        if (nIter % lbCheckFreq == 0) {
            if (current_solution->lb > bestLB) {
                bestLB = current_solution->lb;
            }
            // slow down
            else {
                subgradFactor = std::max(param.subgradFmin, param.subgradFmult * subgradFactor);
            }
        }
        // if heuristic is run

        if (param.heurFreq > 0 && nIter % param.heurFreq == 0
            && !current_solution->isFeasible) { // only run heuristic if not alreay feas.
            if (hooks.heuristics(*current_solution) == ABORT) {
                status = ABORT;
                continue;
            }
        }
        // if (param.printLevel > 1 && nIter % param.printFreq == 0) {
        //     printf("\t: LB=%g UB=%g feas=%d minViol=%g\n",
        //         current_solution->lb, current_solution->ub, current_solution->isFeasible,
        //         current_solution->viol.min());
        //     if (param.printLevel > 2) {
        //         printf("\t\tstepSize=%g\n", stepSize);
        //         printf("\t\tRedCst %g - %g\n", current_solution->rc.min(), current_solution->rc.max());
        //         printf("\t\tdual %g - %g\n", current_solution->dual.min(), current_solution->dual.max());
        //     }
        // }

        if (updateBest(hooks, nIter)) {
            noImproveIter = 0;
        }
        // if no improvement has been made for maxNoImprove iterations, reset the dual values
        // go through code reset criteria...
        else {
            if (++noImproveIter > maxNoImprove) { // reset duals
                ++nReset;
                current_solution->dual = 0;
                double minLB = current_solution->lb, maxLB = current_solution->lb;
                hooks.reducedCost(*current_solution);
                double maxdual = std::max(0.0, current_solution->rc.max()) - std::min(0.0, current_solution->rc.min());
                if (maxdual < 1e-9)
                    maxdual = 1;

                for (int j = 0; j < dsize; ++j) { // generate pertubation about
                    if (current_solution->dual[j] == 0 && rand() < 2.0 / dsize) {
                        current_solution->dual[j] = rand(
                            std::max(dualLB[j], -maxdual),
                            std::min(dualUB[j], maxdual));
                    } else
                        current_solution->dual[j] = best_solution->dual[j] * rand(1 - 0.2 / nReset, 1 + 0.2 / nReset);
                    current_solution->dual[j] = std::max(
                        dualLB[j], std::min(dualUB[j], current_solution->dual[j]));
                }

                subgradFactor = param.subgradFactor / nReset;

                if (hooks.reducedCost(*current_solution) == ABORT) {
                    status = ABORT;
                }
                if (hooks.solveSubproblem(*current_solution) == ABORT) {
                    status = ABORT;
                }

                param.heurFreq = std::max(param.heurFreq / 2, 1);
                param.subgradFmin *= 0.5;
                updateBest(hooks, nIter);
                noImproveIter = 0;
                maxNoImprove += 1000;
            }
        }
    }
    if (param.printLevel && nIter % param.printFreq == 0)
        printf("%2d: LB=%.2f UB=%.2f\n", nIter, best_solution->lb, best_solution->ub);

    // update final dual min and max values

    // final_dual_min = current_solution->dual.min();
    // final_dual_max = current_solution->dual.max();
}

double Problem::wallTime() const
{
    return omp_get_wtime() - _wallTime;
}
// set up an initial swarm
void Problem::initialise(UserHooks& hooks)
{

    // for (int j = 0; j < dsize; ++j) { // generate values up to maxCost
    //     if (param.zeroInitial == false) {
    //         current_solution->dual[j] = rand(std::max(dualLB[j], -maxCost),
    //             std::min(dualUB[j], maxCost));
    //     } else {
    //         current_solution->dual[j] = 0.0;
    //     }
    // }
}

//update best_solution only returns true if the lower bound (bound) is improved. It will still update the best_solution solutions
// upper bound and primal solution, however it may not return true if the lower bound has not been improved
bool Problem::updateBest(UserHooks& hooks, int nIter)
{
    bool improved = false;
    if (current_solution->lb > best_solution->lb) {
        //update best_solution's lower bound
        best_solution->lb = current_solution->lb;
        best_solution->dual = current_solution->dual;

        improved = true;
    }

    if (current_solution->ub < best_solution->ub) {
        best_solution->ub = current_solution->ub;
        best_solution->x = current_solution->x;
    }

    return improved;

    // for (ParticleIter current_solution(swarm); !current_solution.done(); ++current_solution) {
    //     if (current_solution->isFeasible && current_solution->ub < best_solution->ub) {
    //         best_solution->ub = current_solution->ub;
    //         best_solution->isFeasible = true;
    //         best_solution->x = current_solution->x;
    //         best_solution->best_ub_sol = current_solution->best_ub_sol;
    //         best_solution->perturb = current_solution->perturb; // perturbation gives good feasible
    //         bestP = &(*current_solution);
    //         if (param.nParticles == 1) {
    //             best_particles_primal_time = best_particles;
    //         } else {
    //             swarm_primal_time = swarm;
    //         }
    //         primal_cpu_time = cpuTime();
    //         best_nIter = nIter;
    //         if (current_solution->lb > best_solution->lb) {
    //             lb_primal_cpu_time = current_solution->lb;
    //         } else {
    //             lb_primal_cpu_time = best_solution->lb;
    //         }
    //     }

    //     if (current_solution->lb > best_solution->lb) {
    //         best_solution->lb = current_solution->lb;
    //         best_solution->dual = current_solution->dual;
    //         best_solution->viol = current_solution->viol;
    //         improved = true;
    //         dual_cpu_time = cpuTime();
    //     }
    //     if (current_solution->lb >= best_solution->lb) {
    //         Solution p1 = *(current_solution);
    //         if (param.nParticles == 1) {
    //             best_particles.push_back(&(p1));
    //         }
    //     }
    // }

    // if (bestP) // only call once if current swarm improved optimum
    //     hooks.updateBest(*bestP);
    // return improved || (bestP != 0);
}
}; // end namespace

/* Stuff for emacs/xemacs to conform to the "Visual Studio formatting standard"
 * Local Variables:
 * tab-width: 4
 * eval: (c-set-style "stroustrup")
 * End:
*/
