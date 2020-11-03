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
        
    // CPLEX_MIP_Subproblem(){
    //     this->env.end();
    // };
    CPLEX_MIP_Subproblem(){};

    // CPLEX_MIP_Subproblem(IloModel model,
    //     IloNumVarArray numvararray, int subproblem_var_idx)
    // {   
    //     this->model = model;
    //     this->variables = numvararray;
    //     this->num_subproblem_vars = subproblem_var_idx;
    // }

    // ~CPLEX_MIP_Subproblem(){
    //     this->env.end();
    // }
    // CPLEX_MIP_Subproblem(CPLEX_MIP_Subproblem& other)
    // {
    //     this->env.end();
    //     this->env = IloEnv(other.env.getImpl());
    //     this->model = other.model;
    //     this->variables = other.variables;
    //     this->subproblemVarIdx_to_originalVarIdx = other.subproblemVarIdx_to_originalVarIdx;
    //     this->originalVarIdx_to_subproblemVarIdx = other.originalVarIdx_to_subproblemVarIdx;
    //     this->num_subproblem_vars = other.num_subproblem_vars;
    // }
    //     this->env = IloEnv(other.env.getImpl());

    // }
    // void initialise_environment(){
    //     env_ptr = &IloEnv();

    // }
    int num_subproblem_vars;
    
    unordered_map<int, int> subproblemVarIdx_to_originalVarIdx;
    unordered_map<int, int> originalVarIdx_to_subproblemVarIdx;
    IloEnv* envPtr;
    IloModel model;
    IloNumVarArray variables;

    int getSubproblemIdx(){
        return subproblem_idx;
    }

    void setSubproblemIdx(int index){
        subproblem_idx = index;
    }

private:
    int subproblem_idx;
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
    ConDecomp_LaPSO_Connector(MIP_Problem& original_problem, const vector<Partition_Struct>& partitions, const vector<bool>& con_vec,const bool printing, const double sp_solve_time_limit, 
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
    void updateParticleLB(ConDecomp_LaPSO_Connector_Solution& s);
    void updateParticleViol(ConDecomp_LaPSO_Connector_Solution& s);
    vector<CPLEX_MIP_Subproblem> MS;
    void initSubproblems(const vector<Partition_Struct>& ps);
    void initOriginalCosts();
    int solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, DblVec& rc, DblVec& x);
    bool debug_printing;
    void populateDualIdxToOrigIdxMap(const vector<bool>& con_relax_vector);
    MIP_Problem OP;
    DblVec original_costs;
    double sp_solve_time_limit;
    unordered_map<int, int> dual_idx_to_orig_constraint_idx_map;
    unordered_map<int, int> orig_constraint_idx_to_dual_idx_map;
    std::shared_ptr<Subproblems> subproblem_statistics_ptr;
};

#endif