#ifndef ConDecomp_LaPSO_Connector_H
#define ConDecomp_LaPSO_Connector_H

#include "Constraint.h"
#include "Hypergraph.h"
#include "LaPSO.hpp"
#include "MIP_Problem.h"
#include "Subproblem_Constraints.h"
#include "Variable.h"
#include <ilcplex/ilocplex.h>
#include <map>
#include <string>
#include <unordered_map>

using namespace LaPSO;
using namespace std;

class IloSmartPointer { 
    IloEnv* ptr; //  
public: 
    // Constructor: Refer https:// www.geeksforgeeks.org/g-fact-93/ 
    // for use of explicit keyword 
    explicit IloSmartPointer(IloEnv* p = NULL) { ptr = p; } 
    
    void setPtr(IloEnv* p){
        ptr = p;
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

private:
};

class ConDecomp_LaPSO_ConnectorParticle : public LaPSO::Particle {
public:
    ConDecomp_LaPSO_ConnectorParticle(int num_var, int num_con)
        : LaPSO::Particle(num_var, num_con)
    {
    }
    // set bounds for different dual variables based on indexes given
};

class ConDecomp_LaPSO_Connector : public LaPSO::UserHooks {

public:
    ConDecomp_LaPSO_Connector(MIP_Problem& original_problem, const vector<Partition_Struct>& partitions, const bool printing, const double sp_solve_time_limit);
    //void solve_ConDecomp_LaPSO_Connector(ConDecomp_LaPSO_ConnectorParticle &p);
    int nsolves; // number of times ConDecomp_LaPSO_Connector was solved
    int maxsolves; // abort after this many
    ~ConDecomp_LaPSO_Connector(){};

    Status reducedCost(const Particle& p, DblVec& redCost);
    Status solveSubproblem(Particle& p);
    //void randomiseMethod(ConDecomp_LaPSO_ConnectorParticle &p);
    Status fixConstraint(const int constraint,
        const Particle& p,
        SparseVec& feas);
    Status heuristics(Particle& p);
    Status updateBest(Particle& p);

    void setPrinting(bool p) { printing = p; }
    void endCplexEnvs();

private:
    void updateParticleLB(ConDecomp_LaPSO_ConnectorParticle& p);
    void updateParticleViol(ConDecomp_LaPSO_ConnectorParticle& p);
    vector<CPLEX_MIP_Subproblem> MS;
    void initSubproblems(const vector<Partition_Struct>&);
    void initOriginalCosts();
    int solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, DblVec& rc, DblVec& x);
    bool printing;
    MIP_Problem OP;
    DblVec original_costs;
    double sp_solve_time_limit;
};

#endif