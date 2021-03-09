import pandas as pd
import csv
from pathlib import Path
# GCG1 scores are heuristic scores given for bordered mmatrices with no linking variables. This score is as described
# in the GCG documentation, which can be simplified as 0.6 * (relaxed_const_prop) + 0.01 + 0.2*(1 – min(density))

def getMinDensity(data_list):

    min_val = 0.0
    data_list_float = [float(i) for i in data_list]
    min_val = min(data_list_float)
    return min_val

def writeGCG1Scores(relaxed_constraint_prop_path, densities_path, GCG1_output_path):

    decomp_col_index = 0
    rc_prop_col_index = 1
    densities_col_index = 1
    with open(GCG1_output_path , "w") as GCG1_Scores_output_fs:
        #loop through each file and multiply Q and P scores
        with open(relaxed_constraint_prop_path, "r") as rc_input_fs, open(densities_path, "r") as densities_input_fs:
            rc_prop_csvreader = csv.reader(rc_input_fs, delimiter=",")
            densities_csvreader = csv.reader(densities_input_fs, delimiter=",")
            for line_number, (rc_prop_line_split, densities_line_split) in enumerate(zip(rc_prop_csvreader, densities_csvreader)):
                # ensure that the decomposition index is the same
                if line_number == 0:
                    GCG1_Scores_output_fs.write("Decomposition Index,GCG1 Scores" + "\n")
                else:
                    rc_prop_decomp_num = int(rc_prop_line_split[decomp_col_index])
                    densities_decom_num = int(densities_line_split[decomp_col_index])
                    if rc_prop_decomp_num != densities_decom_num:
                        print("Incorrect decomposition index analysed in {}".format(relaxed_constraint_prop_path))
                        exit(-1)
                    else:
                        min_density = getMinDensity(densities_line_split[densities_col_index:])
                        rc_prop = float(rc_prop_line_split[rc_prop_col_index])
                        gcg1_score = (0.6 * rc_prop) + (0.2*(1 - min_density))
                        GCG1_Scores_output_fs.write(str(rc_prop_decomp_num) + "," + str(gcg1_score) + "\n")
def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]

    # instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]

    instance_names_testing = [["germany50-UUM.mps"], ["k16x240b.mps"], ["snp-10-052-052.mps"]]
    number_of_batches = 10

    raw_outputs_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs/Ranking"
    processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Ranking"
    features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Features_Calculated"
    
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            for batch_number in range(number_of_batches):
                relaxed_constraint_prop_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
                densities_path = raw_outputs_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Subproblem_Statistics" + "/" + "Densities.csv"
                GCG1_values_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Ranking_Method_Scores"
                Path(GCG1_values_output_folder).mkdir(parents=True, exist_ok=True)
                GCG1_output_path = GCG1_values_output_folder + "/" + "GCG1_Scores.csv"
                writeGCG1Scores(relaxed_constraint_prop_path, densities_path, GCG1_output_path)
                print("Finished {}".format(instance_name))
# store the important features in a list
if __name__ == "__main__":
    main()