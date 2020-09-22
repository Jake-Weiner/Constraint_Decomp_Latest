/// Generic solver for combinatorial optimisation problems using a combination
/// of Wedelin's lagrangian relaxation heuristic and Solution swarm
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

Solution::Solution(const Solution& other){
    this->dual = other.dual;
    this->x = other.x;
    this->viol = other.viol;
    this->rc =other.rc;
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

Problem::Problem(int nVar, int nConstr)
    : psize(nVar)
    , dsize(nConstr)
    , dualLB(dsize, -INF)
    , dualUB(dsize, INF)
    , nIter(-1)
{
    _wallTime = omp_get_wtime();
    best->ub = INF;
    best->lb = -INF;
    best->isFeasible = false;
    best->dual.resize(nConstr, 0.0);
}

void Problem::initProblem(const LaPSORequirements& LR)
{
    psize = LR.nVar;
    dsize = LR.nConstr;
    dualLB.resize(dsize, -INF);
    dualUB.resize(dsize, INF);
    nIter = -1;
    _wallTime = omp_get_wtime();
    best->ub = INF;
    best->lb = -INF;
    best->isFeasible = false;
    best->dual.resize(LR.nConstr, 0.0);
    
    // use a predetermined ub?
    if (LR.benchmark_ub_flag){
        this->best_ub = LR.best_ub;
        this->benchmark_ub_flag = LR.benchmark_ub_flag;
    }

    setDualBoundsEqual(LR.cti.equality_idxs);
    setDualBoundsLesser(LR.cti.less_than_idxs);
    setDualBoundsGreater(LR.cti.greater_than_idxs);

}

void Problem::setDualBoundsEqual(const std::vector<int>& idxs)
{
    for (auto& idx : idxs) {
        dualLB[idx] = -INF;
        dualUB[idx] = INF;
    }
}

void Problem::setDualBoundsLesser(const std::vector<int>& idxs)
{
    for (auto& idx : idxs) {
        dualLB[idx] = -INF;
        dualUB[idx] = 0;
    }
}
void Problem::setDualBoundsGreater(const std::vector<int>& idxs)
{
    for (auto& idx : idxs) {
        dualLB[idx] = 0;
        dualUB[idx] = INF;
    }
}

// Main method
void Problem::solve(UserHooks& hooks)
{

    double improv_thresh = 1.01;
    const int lbCheckFreq = 10;
    initialise(hooks); // set up the swarm
    //------------- initial solution for swarm --------------
    if (param.printLevel > 1)
        printf("Initialisation:\n");
    // initial dual values

    // update final dual min and max values

    initial_dual_min = s->dual.min();
    initial_dual_max = s->dual.max();

    if (s->dual.min() < initial_dual_min) {
        initial_dual_min = s->dual.min();
    }
    if (s->dual.max() > initial_dual_max) {
        initial_dual_max = s->dual.max();
    }

    if (param.printLevel > 1) {
        printf("initial dual min is %f:\n", initial_dual_min);
        printf("initial dual max is %f:\n", initial_dual_max);
    }
    double bestLB;

    Uniform rand; //

    if (param.randomSeed != 0)
        rand.seed(param.randomSeed);
    else
        rand.seedTime();

    Status status = OK;

    for (int i = 0; i < dsize; ++i) { // check initial point is valid
        //check the dual values...
        if (s->dual[i] < dualLB[i]) {
            if (param.printLevel)
                printf("ERROR: initial dual[%d] %.2f below LB %.2f\n",
                    i, s->dual[i], dualLB[i]);
            s->dual[i] = dualLB[i];
        }
        if (s->dual[i] > dualUB[i]) {
            if (param.printLevel)
                printf("ERROR: initial dual[%d] %.2f below UB %.2f\n",
                    i, s->dual[i], dualUB[i]);
            s->dual[i] = dualUB[i];
        }
    }

    // update the reduced costs
    if (hooks.reducedCost(*s) == ABORT) {
        status = ABORT;
    }
   

    if (hooks.solveSubproblem(*s) == ABORT) {
        status = ABORT;
    }


    printf("Status is %d", status);
    bestLB = s->lb;
    if (param.printLevel > 1) {
        printf("\t LB=%g UB=%g feas=%d viol=%g -- %g\n",
            s->lb, s->ub, s->isFeasible,
            s->viol.min(), s->viol.max());
        if (param.printLevel > 2) {
            printf("\t\tRedCst %g - %g\n", s->rc.min(), s->rc.max());
            printf("\t\tdual %g - %g\n", s->dual.min(), s->dual.max());
        }
    }

    best->x.resize(psize, 0);
    updateBest(hooks, 0);
    if (status == ABORT)
        return;

    double subgradFactor = param.subgradFactor;

    if (param.printLevel > 1)
        printf("Initial LB=%g UB=%g\n", best->lb, best->ub);
    int noImproveIter = 0, nReset = 0, maxNoImprove = 1000;

    int current_lb_time_limits_idx = 0;
    double last_lb_comparison;

    best_lb_tracking.push_back(best->lb);
    double average_lb = s->lb;
    average_lb_tracking.push_back(average_lb);
    timing_tracking.push_back(cpuTime());
    for (nIter = 1; nIter < param.maxIter && cpuTime() < param.maxCPU && wallTime() < param.maxWallTime && status == OK && (best->lb + param.absGap <= best->ub) && fabs(best->ub - best->lb / best->ub) > param.relGap;
         ++nIter) {

        if (param.printLevel > 1 && nIter % param.printFreq == 0) {
            printf("iter num = %d\n", nIter);
            printf("best lower bound so far is %f \n", best->lb);
        }

        // cpu time is within the limit
        best_lb_tracking.push_back(best->lb);
        double average_lb = s->lb;
        average_lb_tracking.push_back(average_lb);
        timing_tracking.push_back(cpuTime());
        if (param.printLevel > 1 && nIter % param.printFreq == 0)
            printf("Iteration %d --------------------\n", nIter);

        // --------- calculate step size/direction ----------
        double norm = 0;
        for (int i = 0; i < dsize; ++i)
            norm += s->viol[i] * s->viol[i];

        // calculate the step size
        const double stepSize = (best_ub - bestLB) / norm;

        //---------- make a step ------------------------------
        for (int i = 0; i < dsize; ++i) {
            s->dual[i] += stepSize * s->viol[i];
            s->dual[i] = std::max(dualLB[i], std::min(dualUB[i], s->dual[i]));
        }
        //---------- solve subproblems -------------------------
        if (hooks.reducedCost(*s) == ABORT) {
            status = ABORT;
            continue;
        }

        if (hooks.solveSubproblem(*s) == ABORT) {
            status = ABORT;
            continue;
        }

        //
        if (nIter % lbCheckFreq == 0) {
            if (s->lb > bestLB) {
                bestLB = s->lb;
            }
            // slow down
            else {
                subgradFactor = std::max(param.subgradFmin, param.subgradFmult * subgradFactor);
            }
        }
        // no heuristic is run

        // if (param.heurFreq > 0 && nIter % param.heurFreq == 0
        //     && !s->isFeasible) { // only run heuristic if not alreay feas.
        //     if (hooks.heuristics(*s) == ABORT) {
        //         status = ABORT;
        //         continue;
        //     }
        // }
        if (param.printLevel > 1 && nIter % param.printFreq == 0) {
            printf("\t: LB=%g UB=%g feas=%d minViol=%g\n",
                s->lb, s->ub, s->isFeasible,
                s->viol.min());
            if (param.printLevel > 2) {
                printf("\t\tstepSize=%g\n", stepSize);
                printf("\t\tRedCst %g - %g\n", s->rc.min(), s->rc.max());
                printf("\t\tdual %g - %g\n", s->dual.min(), s->dual.max());
            }
        }
    }

    if (updateBest(hooks, nIter)) {
        noImproveIter = 0;
    }
    // go through code reset criteria...
    else {
        if (++noImproveIter > maxNoImprove) { // reset duals
            ++nReset;
            s->dual = 0;
            double minLB = s->lb, maxLB = s->lb;
            hooks.reducedCost(*s);
            double maxdual = std::max(0.0, s->rc.max()) - std::min(0.0, s->rc.min());
            if (maxdual < 1e-9)
                maxdual = 1;

            for (int j = 0; j < dsize; ++j) { // generate pertubation about
                if (s->dual[j] == 0 && rand() < 2.0 / dsize) {
                    s->dual[j] = rand(
                        std::max(dualLB[j], -maxdual),
                        std::min(dualUB[j], maxdual));
                } else
                    s->dual[j] = best->dual[j] * rand(1 - 0.2 / nReset, 1 + 0.2 / nReset);
                s->dual[j] = std::max(
                    dualLB[j], std::min(dualUB[j], s->dual[j]));
            }

            subgradFactor = param.subgradFactor / nReset;

            if (hooks.reducedCost(*s) == ABORT) {
                status = ABORT;
            }
            if (hooks.solveSubproblem(*s) == ABORT) {
                status = ABORT;
            }

            param.heurFreq = std::max(param.heurFreq / 2, 1);
            param.subgradFmin *= 0.5;
            updateBest(hooks, nIter);
            noImproveIter = 0;
            maxNoImprove += 1000;
        }
    }

    if (param.printLevel && nIter % param.printFreq == 0)
        printf("%2d: LB=%.2f UB=%.2f\n", nIter, best->lb, best->ub);

    // update final dual min and max values

    final_dual_min = s->dual.min();
    final_dual_max = s->dual.max();
}

double Problem::wallTime() const
{
    return omp_get_wtime() - _wallTime;
}
// set up an initial swarm
void Problem::initialise(UserHooks& hooks)
{
    nIter = 0;
    x_total.resize(psize, 0);
    omp_set_num_threads(param.nCPU);
    _wallTime = omp_get_wtime();
    timer.reset();

    s = new Solution(psize, dsize);
    // create the best and s Solution objects
    best = new Solution(psize, dsize);
    // figure out what range of duals makes sense
    if (best->dual.size() < (size_t)dsize)
        best->dual.resize(dsize, 0.0);
    if (best->rc.size() < (size_t)psize)
        best->rc.resize(psize, 0.0);
    hooks.reducedCost(*best);


    // initialuse the dual variables if wanted..

    // find biggest absolute value reduced cost
    const double maxCost = std::max(
        *std::max_element(best->rc.begin(), best->rc.end()),
        -*std::min_element(best->rc.begin(), best->rc.end()));

   
    Uniform rand;
    rand.seed(1);

 
    // for (int j = 0; j < dsize; ++j) { // generate values up to maxCost
    //     if (param.zeroInitial == false) {
    //         s->dual[j] = rand(std::max(dualLB[j], -maxCost),
    //             std::min(dualUB[j], maxCost));
    //     } else {
    //         s->dual[j] = 0.0;
    //     }
    // }
}

bool Problem::updateBest(UserHooks& hooks, int nIter)
{
   
    bool improved = false;
    if (s->lb > best->lb) {
        delete best;
        best = new Solution(*s);
        improved = true;
    }

    return improved;

    // for (ParticleIter s(swarm); !s.done(); ++s) {
    //     if (s->isFeasible && s->ub < best->ub) {
    //         best->ub = s->ub;
    //         best->isFeasible = true;
    //         best->x = s->x;
    //         best->best_ub_sol = s->best_ub_sol;
    //         best->perturb = s->perturb; // perturbation gives good feasible
    //         bestP = &(*s);
    //         if (param.nParticles == 1) {
    //             best_particles_primal_time = best_particles;
    //         } else {
    //             swarm_primal_time = swarm;
    //         }
    //         primal_cpu_time = cpuTime();
    //         best_nIter = nIter;
    //         if (s->lb > best->lb) {
    //             lb_primal_cpu_time = s->lb;
    //         } else {
    //             lb_primal_cpu_time = best->lb;
    //         }
    //     }

    //     if (s->lb > best->lb) {
    //         best->lb = s->lb;
    //         best->dual = s->dual;
    //         best->viol = s->viol;
    //         improved = true;
    //         dual_cpu_time = cpuTime();
    //     }
    //     if (s->lb >= best->lb) {
    //         Solution p1 = *(s);
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
