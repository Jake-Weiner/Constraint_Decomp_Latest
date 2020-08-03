#include "ConDecomp_LaPSO_Connector.h"
#include "Random.h"
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <fstream>
#include <iostream>
#include <list>
#include <map>

#include <type_traits>
#include <typeinfo>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
#include <cstdlib>
#include <memory>
#include <string>

using namespace boost;
using namespace std;

ConDecomp_LaPSO_Connector::ConDecomp_LaPSO_Connector(MIP_Problem& original_problem, const vector<Partition_Struct>& partitions,
    const bool printing, const double sp_solve_time_limit)
{
    this->OP = original_problem;
    this->printing = printing;
    this->sp_solve_time_limit = sp_solve_time_limit;
    initOriginalCosts();
    initSubproblems(partitions);
}

void ConDecomp_LaPSO_Connector::initOriginalCosts()
{
    original_costs.resize(OP.getNumVariables(), 0);
    for (auto& objective_term : OP.objective_fn) {
        int var_idx = objective_term.first;
        double var_coeff = objective_term.second;
        original_costs[var_idx] = var_coeff;
    }
}

void ConDecomp_LaPSO_Connector::initSubproblems(const vector<Partition_Struct>& partitions)
{
    int subproblem_idx = 0;
    for (auto& partition : partitions) {
        if (subproblem_idx % 10000 == 0) {
            cout << "setting up subproblem " << subproblem_idx << endl;
        }
        subproblem_idx++;
        // partition.printNodes();
        // partition.printEdges();

        CPLEX_MIP_Subproblem sp;
        sp.envPtr = new IloEnv;
        // IloEnv *(sp.envPtr);
        IloModel model(*(sp.envPtr));
        IloNumVarArray subproblem_vars_cplex(*(sp.envPtr));
        IloRangeArray subproblem_constraints_cplex(*(sp.envPtr));
        // unordered_map<int, int> subproblemVarIdx_to_originalVarIdx;
        // unordered_map<int, int> originalVarIdx_to_subproblemVarIdx;

        // add all variables in partition to subproblem model
        vector<Variable> var_in_partition;
        for (const auto& var_idx : partition.node_idxs) {
            var_in_partition.push_back(OP.getVariable(var_idx));
        }
        int subproblem_var_idx = 0;
        sp.subproblemVarIdx_to_originalVarIdx.reserve(var_in_partition.size());
        sp.originalVarIdx_to_subproblemVarIdx.reserve(var_in_partition.size());
        for (auto& variable : var_in_partition) {
            int orig_var_idx = variable.getVarIndx();
            double var_lb = variable.getLB();
            double var_ub = variable.getUB();
            sp.originalVarIdx_to_subproblemVarIdx[orig_var_idx] = subproblem_var_idx;
            sp.subproblemVarIdx_to_originalVarIdx[subproblem_var_idx] = orig_var_idx;
            if (variable.getVarType() == Int) {
                IloNumVar x(*(sp.envPtr), var_lb, var_ub, ILOINT);
                subproblem_vars_cplex.add(x);
            } else if (variable.getVarType() == Cont) {
                IloNumVar x(*(sp.envPtr), var_lb, var_ub, ILOFLOAT);
                subproblem_vars_cplex.add(x);
            }

            subproblem_var_idx++;
        }
        if (subproblem_idx % 10000 == 0) {
            cout << "number of variables in subproblem is " << subproblem_var_idx << endl;
        }

        // if only 1 variable is involved, no need to create a mip model
        if (subproblem_var_idx == 1) {
            sp.subproblemVarIdx_to_originalVarIdx;
            sp.originalVarIdx_to_subproblemVarIdx;
            sp.num_subproblem_vars = subproblem_var_idx;
            if (sp.envPtr != nullptr) {
                if ((*(sp.envPtr)).getImpl() != nullptr) {
                    (*(sp.envPtr)).end();
                }
                delete sp.envPtr;
                sp.envPtr = nullptr;
            }

            // (*(sp.envPtr)).end();
            MS.push_back(sp);
            continue;
        }

        //add constraints to subproblem
        vector<Constraint> subproblem_constraints;
        if (!partition.edge_idxs.empty()) {
            for (auto& constraint_idx : partition.edge_idxs) {
                subproblem_constraints.push_back(OP.constraints[constraint_idx]);
            }

            //add constraints to the model
            for (auto& constraint : subproblem_constraints) {
                IloExpr constraint_exp(*(sp.envPtr));

                for (auto& term : constraint.getConTerms()) {
                    int var_idx = term.first;
                    double var_coeff = term.second;
                    int var_idx_in_sp = sp.originalVarIdx_to_subproblemVarIdx[var_idx];
                    constraint_exp += (var_coeff * subproblem_vars_cplex[var_idx_in_sp]);
                }

                if (constraint.getBoundType() == Greater) {
                    IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, INF);
                    subproblem_constraints_cplex.add(r1);
                } else if (constraint.getBoundType() == Equal) {
                    IloRange r1(*(sp.envPtr), constraint.getRHS(), constraint_exp, constraint.getRHS());
                    subproblem_constraints_cplex.add(r1);
                } else if (constraint.getBoundType() == Less) {
                    IloRange r1(*(sp.envPtr), -INF, constraint_exp, constraint.getRHS());
                    subproblem_constraints_cplex.add(r1);
                }
            }
            model.add(subproblem_constraints_cplex);
        }

        // save model and dictionaries to subproblem structure and save these to the MIP_subproblems vector
        // CPLEX_MIP_Subproblem sp((*(sp.envPtr)), model, subproblem_vars_cplex, subproblemVarIdx_to_originalVarIdx,
        // originalVarIdx_to_subproblemVarIdx, subproblem_var_idx);
        // *(sp.envPtr).end();
        sp.model = model;
        sp.variables = subproblem_vars_cplex;
        sp.num_subproblem_vars = subproblem_var_idx;
        MS.push_back(sp);
    }
    cout << "finished initialising subproblems " << endl;
    return;
}

Status ConDecomp_LaPSO_Connector::reducedCost(const Particle& p, DblVec& redCost)
{

    for (int i = 0; i < redCost.size(); i++) {
        redCost[i] = original_costs[i];
    }

    // for each constraint, update the red cost by dual[i] * coeff in constraint
    for (auto& constraint : OP.constraints) {
        int constraint_idx = constraint.getConIdx();
        for (auto& con_term : constraint.getConTerms()) {
            int var_idx = con_term.first;
            double var_coeff = con_term.second;
            redCost[var_idx] += p.dual[constraint_idx] * (-1 * var_coeff);
        }
    }
    return OK;
}

int ConDecomp_LaPSO_Connector::solveSubproblemCplex(CPLEX_MIP_Subproblem& sp, DblVec& rc, DblVec& x)
{

    // cout << "number of subproblem variables is " << sp.variables.getSize() << endl;
    // cout << "num variables is " << sp.num_subproblem_vars << endl;

    //if subproblem size is 1 solve without cplex
    if (sp.subproblemVarIdx_to_originalVarIdx.size() == 1) {
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[0];
        double coeff = rc[original_var_idx];
        // if negative coeff, set to max var bound
        Variable var = OP.getVariable(original_var_idx);
        double upper_bound = var.getUB();
        double lower_bound = var.getLB();

        if (coeff < 0) {
            x[original_var_idx] = upper_bound;
        } else if (coeff > 0) {
            x[original_var_idx] = lower_bound;
        } else {
            x[original_var_idx] = 0;
        }
        return 0;
    }
    // add in objective function to the model...
    IloExpr obj_exp(sp.model.getEnv());
    for (int i = 0; i < sp.variables.getSize(); i++) {
        //original index -- get coeff
        int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        double coeff = rc[original_var_idx];
        obj_exp += (coeff * sp.variables[i]);
    }

    IloObjective obj_fn = IloMinimize(sp.model.getEnv(), obj_exp);
    sp.model.add(obj_fn);

    IloCplex cplex(sp.model);
    cplex.setParam(IloCplex::Threads, 1); // solve using 1 thread only
    cplex.setParam(IloCplex::TiLim, sp_solve_time_limit);
    cplex.setOut((*(sp.envPtr)).getNullStream());
    if (!cplex.solve()) {
        // cout << "objective solved is " << obj_exp << endl;
        // cout << "variables involved are: " << endl;
        // for (int i = 0; i < sp.variables.getSize(); i++){
        //     int original_var_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        //     Variable t = OP.getVariable(original_var_idx);
        //     t.printInfo();
        // }
        cout << "Failed to optimize LP" << endl;
        sp.model.remove(obj_fn);
        cplex.end();
        return -1;
    }

    // cout << "Solution status = " << cplex.getStatus() << endl;
    // cout << "Solution value  = " << cplex.getObjValue() << endl;

    for (int i = 0; i < sp.variables.getSize(); i++) {
        int orig_idx = sp.subproblemVarIdx_to_originalVarIdx[i];
        try {
            IloNum val = cplex.getValue(sp.variables[i]);
            Variable_Type vt = OP.variables[orig_idx].getVarType();
            // cout << "index " << i << "has value of " << val << endl;
            if (vt == Int) {
                int x_val = int(val);
                x[orig_idx] = x_val;
            } else if (vt == Cont) {
                x[orig_idx] = val;
            }
        } catch (IloException& e) {
            int x_val = 0;
            x[orig_idx] = x_val;
            // cout << e << endl;
            // cout << "number of variables is " << sp.variables.getSize() << endl;
        } catch (...) {
            cout << "Unknown exception caught" << endl;
        }
    }
    sp.model.remove(obj_fn);
    cplex.end();
    return 0;
}

void ConDecomp_LaPSO_Connector::updateParticleLB(ConDecomp_LaPSO_ConnectorParticle& p)
{
    //(the reduced costs * x ) +  1b  +  2 e +  3 g
    double lb = 0;
    for (int var_idx = 0; var_idx < p.x.size(); var_idx++) {
        lb += (p.x[var_idx] * p.rc[var_idx]);
    }
    for (int dual_idx = 0; dual_idx < p.dual.size(); dual_idx++) {
        double constraint_bound = OP.constraints[dual_idx].getRHS();
        lb += (p.dual[dual_idx] * constraint_bound);
    }
    p.lb = lb;

    // double lb_test = 0;
    // for (int var_idx = 0; var_idx < p.x.size(); var_idx++){
    //     lb_test += (p.x[var_idx] * original_costs[var_idx]);
    // }

    // for (int dual_idx = 0; dual_idx < p.dual.size(); dual_idx++){
    //     double constraint_bound = OP.constraints[dual_idx].getRHS();
    //     lb_test += (p.dual[dual_idx] * p.viol[dual_idx]);
    // }

    // cout << "reduced costs lb = " << p.lb << " : violation lb = " << lb_test << endl ;
    // check that ~cx + \lambda(bounds) = cx + \lambda(viol)
}

// loop through all constraints, calculate Ax, viol = bound - Ax
void ConDecomp_LaPSO_Connector::updateParticleViol(ConDecomp_LaPSO_ConnectorParticle& p)
{
    p.viol = 0;
    for (int constraint_idx = 0; constraint_idx < OP.getNumConstraints(); constraint_idx++) {
        Constraint con = OP.constraints[constraint_idx];
        double constraint_bound = con.getRHS();
        //calculate constraint value
        double constraint_value = 0;

        for (auto& con_term : con.getConTerms()) {
            int var_idx = con_term.first;
            double var_coeff = con_term.second;
            constraint_value += (var_coeff * p.x[var_idx]);
        }
        // cout << "constraint value is "<< constraint_value << " constraint bound is " << constraint_bound << endl;
        p.viol[constraint_idx] = constraint_bound - constraint_value;
    }
}

Status ConDecomp_LaPSO_Connector::solveSubproblem(Particle& p_)
{

    ++nsolves;
    ConDecomp_LaPSO_ConnectorParticle& p(static_cast<ConDecomp_LaPSO_ConnectorParticle&>(p_));

    //reset previous primal sol
    p.x = 0;

    int subproblem_solve_error = 0;
    // for each subproblem, feed in new objective function, solve and update x
    for (auto& subproblem : MS) {

        int subproblem_status = solveSubproblemCplex(subproblem, p.rc, p.x);
        if (subproblem_status == 1) {
            subproblem_solve_error = 1;
            break;
        }
    }

    updateParticleViol(p);
    updateParticleLB(p);
    //update best particle
    if (p.lb > p_.best_lb) {
        p_.best_lb = p.lb;
    }

    if (printing == true) {
        std::cout << "Subproblem solve " << nsolves << "/" << maxsolves << ": "
                  << " lb=" << p.lb << " ub=" << p.ub
                  << "\trange of dual = " << p.dual.min() << " to " << p.dual.max() << std::endl
                  << "\trange of viol = " << p.viol.min() << " to " << p.viol.max() << std::endl;
        if (!p.perturb.empty())
            std::cout << "\trange of perturb = " << p.perturb.min() << " to " << p.perturb.max() << std::endl;
        std::cout << std::endl;
    }

    LaPSO::Status status;
    if ((nsolves < maxsolves) && (subproblem_solve_error == 0)) {
        status = OK;
    } else {
        status = ABORT;
    }
    return status;
}

Status ConDecomp_LaPSO_Connector::fixConstraint(const int constraint,
    const Particle& p,
    SparseVec& feas) { return OK; }

Status ConDecomp_LaPSO_Connector::heuristics(Particle& p_)
{
    nsolves++;
    return (nsolves < maxsolves) ? OK : ABORT;
}

Status ConDecomp_LaPSO_Connector::updateBest(Particle& p_)
{
    ConDecomp_LaPSO_ConnectorParticle& p(static_cast<ConDecomp_LaPSO_ConnectorParticle&>(p_));

    // if (printing == true)
    //     cout << "################## " << solution_cost << " ############\n";
    return OK;
}

void ConDecomp_LaPSO_Connector::endCplexEnvs()
{

    for (auto& subproblem : MS) {
        if (subproblem.envPtr != nullptr) {
            if ((*(subproblem.envPtr)).getImpl() != nullptr) {
                (*(subproblem.envPtr)).end();
            }
            delete subproblem.envPtr;
            subproblem.envPtr = nullptr;
        }
        subproblem.subproblemVarIdx_to_originalVarIdx.clear();
        subproblem.originalVarIdx_to_subproblemVarIdx.clear();
    }
}
