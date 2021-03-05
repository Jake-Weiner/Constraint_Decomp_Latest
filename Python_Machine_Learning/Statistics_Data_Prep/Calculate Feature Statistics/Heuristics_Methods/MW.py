import pandas as pd
import csv

# Max-White (MW) scores are heuristic scores given for gcg made decompositions fopr constraint classification as described on MIPLIB
# the Max-white score is 1 - (s+t)/(nvars * ncons) where s = sum(nvar * cons) in subproblems and t = nvars * nrelaxed constr)

def getMinDensity(data_list):

    min_val = 0.0
    data_list_float = [float(i) for i in data_list]
    min_val = min(data_list_float)
    return min_val

#return the sum of the number of variables * number of constraints for each subproblem. Data lists are read in as list of strings
def sumNVarNConstrSP(var_prop_data_list, constr_prop_data_list, num_instance_var, num_instance_constr):

    #ensure the sizes of the lists are the same
    if len(var_prop_data_list) != len(constr_prop_data_list):
        print("Error, the number of subproblems in var_data != the number of subproblems in constr_data")
        exit(-1)

    sum = sum((float(num_var) * float(num_constr) * num_instance_var * num_instance_constr) for num_var, num_constr in zip(var_prop_data_list, constr_prop_data_list))
    return sum

def getInstanceNumVarsConstr(instance_characteristics_path):
    num_vars = 0
    num_constr = 0
    num_var_idx = 0
    num_constr_idx = 1
    with open(instance_characteristics_path, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                num_vars = int(line_split[num_var_idx])
                num_constr = int(line_split[num_constr_idx])
    return num_vars, num_constr


def writeMWScores(relaxed_constraint_prop_path, var_props_path, constr_props_path, instance_characteristic_path, MW_output_path):

    decomp_col_index = 0
    rc_prop_col_index = 1
    var_prop_col_index = 1
    constr_prop_col_index = 1
    instance_var, instance_constr = getInstanceNumVarsConstr(instance_characteristic_path)
    with open(MW_output_path , "w") as MW_Scores_output_fs:
        #loop through each file and multiply Q and P scores
        with open(relaxed_constraint_prop_path, "r") as rc_input_fs, open(var_props_path, "r") as var_props_input_fs, \
            open(constr_props_path, "r") as constr_props_input_fs:
            rc_prop_csvreader = csv.reader(rc_input_fs, delimiter=",")
            var_props_csvreader = csv.reader(var_props_input_fs, delimiter=",")
            constr_props_csvreader = csv.reader(constr_props_input_fs, delimiter=",")
            for line_number, (rc_prop_line_split, var_props_line_split, constr_props_line_split) in enumerate(zip(rc_prop_csvreader, var_props_csvreader, constr_props_csvreader)):
                # ensure that the decomposition index is the same
                rc_prop_decomp_num = int(rc_prop_line_split[decomp_col_index])
                var_props_decomp_num = int(var_props_line_split[decomp_col_index])
                constr_props_decomp_num = int(var_props_line_split[decomp_col_index])
                if not( rc_prop_decomp_num == var_props_decomp_num == constr_props_decomp_num):
                    print("Decomp index does not match in {}".format(relaxed_constraint_prop_path))
                    exit(-1)
                else:
                    if line_number == 0:
                        MW_Scores_output_fs.write("Decomposition Index, MW Scores" + "\n")
                    else:
                        S = sumNVarNConstrSP(var_props_line_split[var_prop_col_index:], constr_props_line_split[constr_prop_col_index:],
                                             instance_var, instance_constr)

                        rc_prop = float(rc_prop_line_split[rc_prop_col_index])
                        mw_score = 1 - ((S / (instance_var * instance_constr)) + rc_prop)
                        MW_Scores_output_fs.write(str(rc_prop_decomp_num) + "," + str(mw_score) + "\n")



def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    raw_outputs_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"
    heuristics_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Heuristic_Outputs"
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):

            instance_characteristics_path = raw_decomps_results_folder + "/" + problem_type + "/" + instance_name + "/Instance_Statistics" + "/" + "Instance_Statistics.csv"

            instance_num_var, instance_num_constr = getInstanceNumVarsConstr(instance_characteristics_path)


            # relaxed_constraint_prop_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
            # densities_path =  raw_outputs_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/" + "Densities.csv"
            # GCG1_output_path = heuristics_root_folder + "/" + problem_type + "/" + instance_name + "/" + "GCG1_scores.csv"
            # writeGCG1Scores(relaxed_constraint_prop_path, densities_path, GCG1_output_path)
# store the important features in a list
if __name__ == "__main__":
    main()