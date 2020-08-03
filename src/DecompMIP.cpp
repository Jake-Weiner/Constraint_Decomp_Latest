#include "DecompMIP.h"
#include "MIP_Fileparser.h"
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <limits>


using namespace std;


DecompInfo DecompMIP::solveMIP(vector<HG_Edge> HG_edges, vector<HG_Node> HG_Nodes, int largest_subproblem, 
int num_subproblems, bool print, const char* outfile, const int solver_time,  bool warm_start,  const char* warm_start_filename){

    // based on edges/nodes, need to create the MIP problem.
    //
    // ensure that edges and nodes are arranged in ascending order?
    
    DecompInfo DI;

    IloEnv env;
    IloModel model(env);
    IloNumVarArray constraint_var(env);
    NumVarMatrix node_var(env);
    IloRangeArray constraints(env);
    
    


    int number_constraints = HG_edges.size();
    int number_nodes = HG_Nodes.size();
    // initialise constraint variable array (z[i] variables)
    for (int i = 0; i < number_constraints; i++) {
        constraint_var.add(IloBoolVar(env));
    }

    // initialise node variable matrix (x[k][j] variables)
    for (int k=0; k<num_subproblems; k++){
        IloNumVarArray subproblem_vars(env);
        for (int j=0; j<number_nodes; j++){
            subproblem_vars.add(IloBoolVar(env));
        }
        node_var.add(subproblem_vars);
    }

    vector<double> con_vec;
    if (warm_start == true){
        ifstream input(warm_start_filename);
        if (input.is_open()) {
            while (!input.eof()) {
                string line;
                getline(input, line);
                if (line.empty()) {
                    continue;
                }
                trim(line);
                vector<string> line_split;
                boost::split(line_split, line, boost::is_any_of(","), boost::token_compress_on);
                for (auto& con_val_string: line_split){
         
                    try{
                        con_vec.push_back(stod(con_val_string));
                    }
                    catch(...){

                    }
                }
            }
        }
    }

    IloNumArray con_vars(env,con_vec.size()); //constraint_var concatenated

    for (int i=0; i<con_vec.size(); i++){
        con_vars[i] = con_vec[i];
    }

    
    try {
        // create objective_function
        IloExpr obj_exp(env);

        for (int i=0; i<number_constraints; i++){
            obj_exp += constraint_var[i];
        }

        IloObjective obj_fn = IloMinimize(env, obj_exp);
        model.add(obj_fn);

        // ∑j xjk ≤ S  for all k
        // max subproblem size constraints
        for (int k=0; k<num_subproblems; k++){
            IloExpr max_subproblem_size_con_exp(env);
            for (int j=0; j<number_nodes; j++){
                max_subproblem_size_con_exp += node_var[k][j];
            }
            IloRange max_subproblem_con(env, 0, max_subproblem_size_con_exp, largest_subproblem);
            constraints.add(max_subproblem_con);
        }

        // ∑k xjk  = 1 for all j 
        // each variable must be in a subproblem
        for (int j=0; j<number_nodes; j++){
            IloExpr var_placement_exp(env);
            for (int k=0; k<num_subproblems; k++){
                var_placement_exp += node_var[k][j];
            }
            IloRange var_placement_exp_con(env, 1,var_placement_exp, 1);
            constraints.add(var_placement_exp_con);
        }

        // ∑j in i xjk    ≥ ni xj0,k - ni zi  for i,k
        // if constraint i is not relaxed and first non-zero j is in group k then all n variables of constraint i must be allocated to k
        for (int i=0; i<number_constraints; i++){
            for (int k=0; k<num_subproblems; k++){
                IloExpr LHS_var_nonrelaxedcon_exp(env); //LHS of constraint (3)
                vector<int> node_idxs_con_i = HG_edges[i].getNodeIdxs();
                // nodes contained within constraint i
                for (auto& node_idx : node_idxs_con_i){
                    LHS_var_nonrelaxedcon_exp += node_var[k][node_idx];
                }
                IloExpr RHS_var_nonrelaxedcon_exp(env);  //RHS of constraint (3)
                int ni = node_idxs_con_i.size();
                int first_index = node_idxs_con_i[0]; // j0 in constraint
                RHS_var_nonrelaxedcon_exp = (ni*node_var[k][first_index]) - (ni* constraint_var[i]);
                IloRange same_group_exp_con(env, 0,LHS_var_nonrelaxedcon_exp - RHS_var_nonrelaxedcon_exp, numeric_limits<int>::max());
                constraints.add(same_group_exp_con);
            }
        }

        // ∑j xj,k-1  ≤ ∑j xjk   for all k > 1 
        //symmetry constraints   
        for (int k=1; k<num_subproblems; k++){
            IloExpr LHS_symmetry_con(env);
            IloExpr RHS_symmetry_con(env);
            for (int j=0; j<number_nodes; j++){
                LHS_symmetry_con += node_var[k-1][j];
                RHS_symmetry_con += node_var[k][j];
            }
            IloRange symm_exp_con(env, 0,RHS_symmetry_con - LHS_symmetry_con, numeric_limits<int>::max());
            constraints.add(symm_exp_con);
        }

        model.add(constraints);
        
        IloCplex cplex(model);
        // cplex.exportModel("decomp.lp");
        if (warm_start == true){
            cplex.addMIPStart(constraint_var,con_vars);
        }
        cplex.setParam(IloCplex::Threads, 4); // solve using 1 thread only
        cplex.setParam(IloCplex::TiLim, solver_time);
        cout << "solving MIP..." << endl;

        FILE fp_old = *stdout;  // preserve the original stdout
        
        if (print == true){
            *stdout = *fopen(outfile,"w");  // redirect stdout to null
        }

        // Optimize the problem and obtain solution.
        if (!cplex.solve()) {
            env.error() << "Failed to optimize LP" << endl;
            throw(-1);
        }


        IloNumArray vals(env);
        cplex.getValues(vals, constraint_var);
        //populate y and return it
        cout << "Solution status = " << cplex.getStatus() << endl;
        cout << "Solution value  = " << cplex.getObjValue() << endl;
        cout << "Solution vector = " << endl;
        cout << vals << endl;
        

        *stdout=fp_old;

    } catch (IloException& e) {
        cout << e << endl;
    } catch (...) {
        cout << "Unknown exception caught" << endl;
    }
    env.end();

    return DI;

}