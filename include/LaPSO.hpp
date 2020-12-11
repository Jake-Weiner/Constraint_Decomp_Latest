/// Generic solver for combinatorial optimisation problems using a combination
/// of Wedelin's lagrangian relaxation heuristic and particle swarm
/// optimisation. This header file follows the general pattern of the Volume
/// Algorithm implementation in the COIN-OR library to make it easy to swap
/// algorithms and make comparisons between the two methods.
/// We are solving max_dual min_x cx + dual (b - Ax) + perturbation
/// ie dual >= 0 ==> Ax >= b;   dual <= 0 ==> Ax <= b
///
/// The current implementation only allows for binary variables and assumes
/// wer are solving a minimisation problem.
///
/// Author: Andreas Ernst   $ Date: January 2010 $  $ Revision: 0.0 $
#ifndef __LaPSO_H__
#define __LaPSO_H__

#include "CpuTimer.h"
#include <algorithm>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <string>
#include <utility> // for std::pair<>
#include <vector>
#include <memory>


typedef std::pair<int,double> initial_dual_value_pair;


namespace LaPSO {

    // indicies of dual variables with incorrect initial bounds detected
    struct IncorrectInitialDualIndices{
        std::vector<std::pair<int,double>> lower_bound_errors;
        std::vector<std::pair<int,double>> upper_bound_errors;
    };

    const double INF = std::numeric_limits<double>::max();
    /// parameters for use in the algorithm
    struct Param {
        /// set some sensible default values
        Param()
            : randomSeed(331u)
            , // default as in Random.h
            subgradFactor(2.0)
            , subgradFmult(0.6)
            , subgradFmin(0.0001)
            , maxIter(1)
            , maxCPU(600)
            , maxWallTime(INF)
            , printLevel(1)
            , printFreq(1)
            , heurFreq(10)
            , eps(1e-6)
            , absGap(1e-6)
            , relGap(1e-6)
        {
        }
        /// load commandline arguments to set parameter values.
        /// This function will take any argument pair of the form
        /// --<name> <value> (where name is one of the attributes like
        /// maxIter) and set the corresponding parameter to the value
        void parse(int argc, const char** argv);
        /// parsing of filename allows parameter files with
        /// Comment lines starting with # and otherwise lines like
        /// maxIter: 10
        /// (ie <name>: value
        void parse(const char* filename);
        unsigned int randomSeed; /// random number seed
        /** multiplier for the subgradient */
        double subgradFactor;
        /** reduction factor (what to multiply subgradFactor by when no
             *  progress is being made). Should be between 0 and 1 */
        double subgradFmult;
        /** minimum value of subgradFactor (so we continue to make some
             *  progress) */
        double subgradFmin;
        /** maximum number of iterations */
        int maxIter;
        /** maximum CPU time (not wallclock) */
        double maxCPU;
        /** maximum wall-clock (elapsed) time  */
        double maxWallTime;
        /** control the level of output. */
        int printLevel;
        /** after how many iterations to print output */
        int printFreq;
        /** after how many iterations to run the heuristic on each particle */
        int heurFreq;
        /** how accurately to compare whether violations or the like are zero */
        double eps;
        /** terminate when the absolute (not relative) gap between lower and
             * upper bound is less than this. For integer valued objective
             * functions this should normaly be 1 (or 0.99999) */
        double absGap;
        /** terminate when the relative gap between lower and
             * upper bound is less than this. Eg 0.01 is a 1% optimality gap */
        double relGap;
        /** number of parallel processes to use during solve (best performance
             * is number of particles is a multiple of this number) */
        int nCPU;
        bool zeroInitial = true;
        bool printLB;
        const char* LBOutputFilename;
    };

    struct convergence {
        int nIter;
        double euclid_dist;
        double best_lb;
        int best_ub;
    };

    struct constraint_type_indicies{
        std::vector<int> equality_idxs;
        std::vector<int> less_than_idxs;
        std::vector<int> greater_than_idxs;
    };

    struct LaPSORequirements{
        constraint_type_indicies cti;
        int nVar = 0;
        int nConstr = 0;
        double best_ub = 0.0;
        bool benchmark_ub_flag = false;
        double best_lb = -INF;
        std::vector<initial_dual_value_pair> intial_dual_value_pairs;
        bool set_initial_dual_values = false;
        double subproblem_time_lim = 150;
    };



    /** integer vector with  some extra convenience methods */
    class IntVec : public std::vector<int> {
    public:
        IntVec(size_t s = 0, int v = 0)
            : std::vector<int>(s, v)
        {
        }
        /// fill with value v
        int operator=(const int v)
        {
            std::fill(begin(), end(), v);
            return v;
        }
        IntVec& operator=(const std::vector<int>& v)
        {
            *static_cast<std::vector<int>*>(this) = v;
            return *this;
        }
        int min() const
        { ///< minimum value in vector
            return *std::min_element(begin(), end());
        }
        int max() const
        { ///< maximum value in vector
            return *std::max_element(begin(), end());
        }
        int sum() const
        { ///< sum of values in vector
            return std::accumulate(begin(), end(), 0);
        }
    };

    /** Floating point vector with some extra convenience methods */
    class DblVec : public std::vector<double> {
    public:
        DblVec(const std::vector<double>& v)
            : std::vector<double>(v)
        {
        }

        DblVec(size_t s = 0, double v = 0)
            : std::vector<double>(s, v)
        {
        }
        /// assign every value to v
        double operator=(const double v)
        {
            std::fill(begin(), end(), v);
            return v;
        }
        /// allow assignment of straight vectors
        DblVec& operator=(const std::vector<double>& v)
        {
            *static_cast<std::vector<double>*>(this) = v;
            return *this;
        }
        DblVec& operator=(const std::vector<int>& v)
        {
            this->resize(v.size());
            std::vector<int>::const_iterator vi = v.begin();
            for (iterator ti = begin(); ti != end() && vi != v.end(); ++ti, ++vi)
                *ti = *vi;
            return *this;
        }
        /// vector addition
        DblVec& operator+=(const DblVec v)
        {
            const_iterator vi = v.begin();
            for (iterator ti = begin(); ti != end() && vi != v.end(); ++ti, ++vi)
                *ti += *vi;
            return *this;
        }
        /// vector scalar multiplication
        DblVec& operator*=(double v)
        {
            for (iterator ti = begin(); ti != end(); ++ti)
                *ti *= v;
            return *this;
        }


        void negate()
        {
            for (iterator ti = begin(); ti != end(); ++ti)
                *ti = -(*ti);
        }
        double min() const
        { ///< minimum value in vector
            return *std::min_element(begin(), end());
        }
        double max() const
        { ///< maximum value in vector
            return *std::max_element(begin(), end());
        }
        double sum() const
        { ///< sum of values in vector
            return std::accumulate(begin(), end(), 0.0);
        }
    };

    typedef std::pair<int, int> IdxVal;
    /** SparseVec defines a sparse integer vector. Defined in terms of indices
            and the value to be taken at that index
        */
    class SparseVec : public std::vector<IdxVal> {
    public:
        void add(int index, int value) { push_back(IdxVal(index, value)); }
    };

    /** A particle defines a current solution in both primal and dual space.
         *  This can be subclassed if a particular implementation wants to store
         *  additional information with each solution.
         *  Note that each particle may be processed in parallel so any solver
         *  required during reducedCost() or solveSubproblem() should be stored
         *  in this class
         */
    class Solution {
    public:
        /// standard constructor with given size of problem
        Solution(size_t numVar, size_t numConstr) { resize(numVar, numConstr); }
        Solution() { resize(0, 0); }
        Solution(const Solution& other);
        // void swap(Solution& rhs);
        // Solution& operator=(const Solution& other);
        virtual ~Solution() {
        }
        /// resize particle to problem size
        void resize(size_t numVar, size_t numConstr);
        DblVec dual; ///< lagrange multiplier for each relaxed constraint
        DblVec x; ///< primal solution for dual
        /// The viol vector contains the violation from the primal solution
        /// calculated as b-Ax for the relaxed constraints
        DblVec viol;
        /// Reduced cost vector (for current dual)
        DblVec rc;
        bool isFeasible; ///< is primal solution feasible?
        double ub; ///< primal cost (upper bound if feasible)
        double lb; ///< lower bound

    private:
        
    };

    /// return codes for user functions
    enum Status { OK,
        NONE,
        ABORT };
    /** This user hooks define the problem specific methods for solving the
            lagrangian subproblems an other related information. This class must
            be subclassed in order to create a LaPSO implementation.
            For all of the methods a return code is used 
        */
    class UserHooks {
    public:
        virtual ~UserHooks() {}
        /** compute the reduced costs
                @param p (IN) the particle with duals vector
                @param redCost (OUT) the reduced costs, vector is pre-allocated to
                the length number of primal variables.
                redCost = c - dual * A 
            */
        
        virtual Status reducedCost(Solution& s) = 0;
        /** Solve the subproblem for the subgradient step.
                @param redCost (IN) the reduced cost with respect to the dual values
                @param p (OUT) the particle. The p.dual & p.perturb contain the
                current duals and perturbation. The only things
                that should be set by the method are:
                @param p.x (OUT) the optimal solution to the subproblem
                @param p.viol (OUT) b-Ax (the violation of the relaxed constraints)
                @param p.isFeasible (OUT) whether x is primal feasible
                @param p.ub (OUT) primal solution (cost * x)
                @param p.lb (OUT) lagrangian bound (reduced cost * x + lagranian
                constant + perturbation correction)
                The perturbation correction needs to calculate the maximum value
                by which the cost might have increased due to perturbation for any
                primal feasible solution. That is how much lower might the true
                cost be without perturbation.
            */
        virtual Status solveSubproblem(Solution& s) = 0;

        /** Search for a feasible solution near the current point. If a
                solution is found update p.x, p.isFeasible, and p.ub then return OK
                @param p (IN/OUT) the current dual vector and subproblem solution.
            */
        virtual Status heuristics(Solution& s) { return NONE; }

        virtual void localSearch(Solution& s) {}

        /** Allow the user to store information about the best solution or
             *  carry out other actions whenever we find a better heuristic solution.
             */
        virtual Status updateBest(Solution& s) { return OK; }
    };

    /** The problem class holds all of the data for the LaPSO algorithm and is
            used to do the actual solving
        */
    class Problem {
        Problem(const Problem&) {} // don't allow copies
    public:
        /// Default constructor
        Problem()
            : nIter(-1)
        {
        }
        /// Constructor with size (number of variables and constraints)
        Problem(int nVar, int nConstr);
        ~Problem()
        { 
            std::cout << "LaPSO Problem Destructor Called " << std::endl;
        }

        
        void initProblem(const LaPSORequirements& LR);
        /// Problem definition:
        int psize; ///< primal size, number of variables
        int dsize; ///< dual size, number of constraints that are relaxed
        /// The dual lower bound (0 for >= constraints otherwise -INF)
        DblVec dualLB;
        /// The dual upper bound (0 for <= constraints otherwise +INF)
        DblVec dualUB;


        // IntVec x_total;
        // DblVec dual;

        // if a benchmark upperbound (primal solution is known)
        double best_ub;
        bool benchmark_ub_flag;
        double best_lb;
        /// Solutions: swarm contains all particles
        /// swarm is stored as pointers to allow it to contain subclasses of
        /// Particle. The particles are deleted by the Problem destructor.
        /// If the swarm is initialised by the user it should have each
        /// particle of the correct size and with dual vectors forming a
        /// suitably diverse initial distribution of starting points.
        // Solution* current_solution;
      

        std::shared_ptr<Solution> current_solution;
        std::shared_ptr<Solution> best_solution;   ///< best solution found so far

        // std::vector<Solution*> swarm_primal_time;
        // std::vector<Solution*> best_particles; // this vector stores best lower bound solutions if only 1 particle is used. Replacement for swarm
        // std::vector<Solution*> best_particles_primal_time;
        std::vector<double> average_lb_tracking;
        std::vector<double> average_ub_tracking;
        std::vector<double> average_viol_tracking;
        std::vector<double> average_path_saved_tracking;
        std::vector<double> best_lb_tracking;
        std::vector<double> lb_comparisons;
        std::vector<double> best_ub_tracking;
        std::vector<double> dual_0_tracking;
        std::vector<double> perturb_euclid;
        std::vector<double> timing_tracking;
        double final_dual_min;
        double final_dual_max;
        double initial_dual_min;
        double initial_dual_max;


        
        /// ParticleIter is a convenience class to avoid double dereferencing

        double primal_cpu_time;
        double best_nIter;
        double dual_cpu_time;
        double lb_primal_cpu_time;

        double prev_best;
        int non_improving = 2;
        double improv_amount = 1.01;
        Param param; ///< parameters

        /** The solve method is the main function that carries out the
             * particle swarm optimisation. Any information required in solving
             * the subproblems must be part of the UserHooks subclass that is
             * passed to solve. If swarm is not empty it is assumed to contain the
             * initial position of particles in the swarm. Hence solve can be
             * called multiple times with relatively small iteration limits and it
             * will each time continue where it left off.
             *
             * The results are stored in best. So if best.isFeasible is false no
             * feasible solution was found, best.ub and best.lb are the best lower
             * and upper bound, best.x the best feasible, best.dual & best.perturb
             * the solution which give the best lower bound.
             */
        void solve(UserHooks& hooks);

        /**@name Methods returning final status information */
        //@{
        int iter() const { return nIter; } ///< Number of iterations
        /** The swarmRadius can be used to measure convergence. It's the
             * maximum distanced between the best solution and any other particle */
        /** CPU time used by last call to solve in seconds */
        double cpuTime() const { return timer.elapsedSeconds(); }
        /** wall clock time in seconds
                used by last call to solve (if using openmp) */
        double wallTime() const;

        double swarmRadius() const;

        IncorrectInitialDualIndices getIncorrectInitialDualIndices() const{
            return IIDI;
        }
       
    private:

        double ub; ///< primal cost (upper bound if feasible)
        double lb; ///< lower bound
        // viol vector
        DblVec viol;
        /// Reduced cost vector (for current dual)
        DblVec rc;
        int nIter; ///< iteration number
        CpuTimer timer;
        double _wallTime;
        IncorrectInitialDualIndices IIDI;
        void setDualBoundsLesser(const std::vector<int>& idxs);
        void setDualBoundsGreater(const std::vector<int>& idxs);
        void setDualBoundsEqual(const std::vector<int>& idxs);

        void initialise(UserHooks& hooks); ///< set up initial swarm
        /// update the best particle information
        /// returns true if lower or upper bound have improved
        bool updateBest(UserHooks& hooks, int nIter);
        /** Calculate a direction for the perturbation that is likely
                to lead to more feasible solutions (using hooks.fixConstraint) */
    };
};

#endif
/* Stuff for emacs/xemacs to conform to the "Visual Studio formatting standard"
 * Local Variables:
 * tab-width: 4
 * eval: (c-set-style "stroustrup")
 * End:
*/
