import os
import pandas as pd
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    problem_types = ["network_design", "fixed_cost_network_flow"]
    # "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]

                      # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                      #  "snp-10-052-052.mps"]]
                      # ", "ta2 - UUE.mps"

    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"
    #collate the features in a single csv

    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'

    for problem_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # list of dataframes that need to be merged together
            df_list = []
            features_collated_folder = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
            input_features_folder = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Calculated"
            # create output folders if necessary
            Path(features_collated_folder).mkdir(parents=True, exist_ok=True)
            for folder_name in data_folders:
                for filename in os.listdir(input_features_folder + "/" + folder_name):
                    # remove .csv extension from files
                    filename_trimmed = os.path.splitext(filename)[0]
                    df_temp = pd.read_csv(input_features_folder + "/" + folder_name + "/" + filename)
                    # rename columns
                    df_temp.columns = df_temp.columns.map(
                        lambda x: x + "_" + folder_name + "_" + filename_trimmed if x != key else x)
                    df_list.append(df_temp)

            # merge all features into single csv
            df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                            how='outer'), df_list)
            #read in output file
            LR_output_file = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs.csv"
            output_df = pd.read_csv(LR_output_file)
            df_final = pd.merge(df_merged_features, output_df, on=[key], how='outer')
            df_final.to_csv(features_collated_folder + "/collated.csv")


if __name__ == "__main__":
    main()