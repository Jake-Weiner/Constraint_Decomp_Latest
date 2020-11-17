#ifndef ConDecomp_LaPSO_Connector_H
#define ConDecomp_LaPSO_Connector_H

#include "Constraint.h"
#include "Hypergraph.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"
#include "Subproblem_Constraints.h"
#include "Variable.h"
#include "Util.h"
#include "Decomposition_Statistics.h"
#include <ilcplex/ilocplex.h>
#include <map>
#include <string>
#include <unordered_map>


using namespace LaPSO;
using namespace std;
using Decomposition_Statistics::Subproblems;

class IloSmartPointer { 
    IloEnv* ptr; //  
public: 
    // Constructor: Refer https:// www.geeksforgeeks.org/g-fact-93/ 
    // for use of explicit keyword 
    explicit IloSmartPointer(IloEnv* s = NULL) { ptr = s; } 
    
    void setPtr(IloEnv* s){
        ptr = s;
    }
    // Destructor 
    ~IloSmartPointer() {
        ptr->end();
        // delete ptr;
    }
    // Overloading dereferencing operator 
    IloEnv& operator*() { return *ptr; } 
}; 



class CPLEX_MIP_Subproblem {

public:
        
    CPLEX_MIP_Subproblem(){};
    int num_subproblem_vars;
    unordered_map<int, int> subproblemVarIdx_to_originalVarIdx;
    unordered_map<int, int> originalVarIdx_to_subproblemVarIdx;
    IloEnv* envPtr;
    IloModel model;
    IloNumVarArray variables;

    int getSubproblemIdx() const{
        return subproblem_idx;
    }

    void setSubproblemIdx(int index){
        subproblem_idx = index;
    }

    void setSubproblemVarProp(double prop){
        subproblem_var_prop = prop;
    }

    double getSubproblemVarProp() const{
        return subproblem_var_prop;
    }

private:
    int subproblem_idx;
    double subproblem_var_prop;
};

class ConDecomp_LaPSO_Connector_Solution : public LaPSO::Solution {
public:
    ConDecomp_LaPSO_Connector_Solution(int num_var, int num_con)
        : LaPSO::Solution(num_var, num_con)
    {
    }
    // set bounds for different dual variables based on indexes given
};

// this is the general method class which implements solve, reduced costs, heursistic for this particular
// constraint relaxation problem
class ConDecomp_LaPSO_Connector : public LaPSO::UserHooks {
public:
    ConDecomp_LaPSO_Connector(MIP_Problem& original_problem, const vector<Partition_Struct>& partitions, const vector<int>& con_vec
    ,const bool printing, const double& total_solve_time_lim, 
    std::shared_ptr<Subproblems> subproblem_statistics_ptr);
    //void solve_ConDecomp_LaPSO_Connector(ConDecomp_LaPSO_ConnectorParticle &s);
    int nsolves; // number of times ConDecomp_LaPSO_Connector was solved
    int maxsolves; // abort after this many
    ~ConDecomp_LaPSO_Connector(){};

    Status reducedCost(Solution& s);
    Status solveSubproblem(Solution& s);
    //void randomiseMethod(ConDecomp_LaPSO_ConnectorParticle &s);
    Status fixConstraint(const int constraint,
        const Solution& s,
        SparseVec& feas);
    Status heuristics(Solution& s);
    Status updateBest(Solution& s);
    void setPrinting(bool p) { debug_printing = p; }
    void endCplexEnvs();
    int getOriginalConIdx(int dual_idx);
    LaPSO::constraint_type_indicies convertOriginalConstraintTypeIndicies(const LaPSO::constraint_type_indicies& cti);
    vector<initial_dual_value_pair> convertOriginalConstraintInitialDualIndicies(const std::vector<initial_dual_value_pair>& original_initial_dual_pairs);

private:
    void addConstLagMult(ConDecomp_LaPSO_Connector_Solution& s);
    void updateParticleViol(ConDecomp_LaPSO_Connector_Solution& s);
    vector<CPLEX_MIP_Subproblem> MS;
    void initSubproblems(const vector<Partition_Struct>& ps, const int& num_var);
    void initOriginalCosts();
    int solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, Solution& s);
    bool debug_printing;
    void populateDualIdxToOrigIdxMap(const vector<int>& con_relax_vector);
    MIP_Problem OP;
    DblVec original_costs;
    double total_solve_time_lim;
    unordered_map<int, int> dual_idx_to_orig_constraint_idx_map;
    unordered_map<int, int> orig_constraint_idx_to_dual_idx_map;
    std::shared_ptr<Subproblems> subproblem_statistics_ptr;
};

#endif