import os
import pandas as pd
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    processed_results_local_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    decomp_processed_results_external_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"
    no_con_rel_processed_results_external_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Processed_Results"
    #collate the features in a single csv

    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'

    for problem_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # list of dataframes that need to be merged together
            df_list = []
            features_collated_folder = processed_results_local_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
            decomp_input_features_folder = processed_results_local_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Calculated"
            no_con_rel_input_features_folder = processed_results_local_folder + "/" "No_Con_Rel" + "/" + problem_type + "/" + instance_name + "/" + "Features_Calculated"
            # create output folders if necessary
            Path(features_collated_folder).mkdir(parents=True, exist_ok=True)
            for folder_name in data_folders:
                for filename in os.listdir(decomp_input_features_folder + "/" + folder_name):
                    # remove .csv extension from files
                    filename_trimmed = os.path.splitext(filename)[0]
                    df_temp = pd.read_csv(decomp_input_features_folder + "/" + folder_name + "/" + filename)
                    df_no_con_rel = pd.read_csv(no_con_rel_input_features_folder + "/" + folder_name + "/" + filename)
                    df_no_con_rel.fillna(0,inplace=True)
                    df_temp = df_temp.append(df_no_con_rel,ignore_index=True)
                    #append the no_con_rel decomp to the results read in
                    # rename columns
                    df_temp.columns = df_temp.columns.map(
                        lambda x: x + "_" + folder_name + "_" + filename_trimmed if x != key else x)
                    df_list.append(df_temp)
            print("Finished reading in csvs for instance " + instance_name)
            # merge all features into single csv
            df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                            how='outer'), df_list)

            print(df_merged_features.columns[df_merged_features.isna().any(axis='rows')])
            print("Finished merging features for instance " + instance_name)
            #read in LR output file
            decomp_LR_output_file = decomp_processed_results_external_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            decomp_output_df = pd.read_csv(decomp_LR_output_file)

            no_con_rel_LR_output_file = no_con_rel_processed_results_external_folder +  "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            # print(decomp_output_df)
            no_con_rel_output_df = pd.read_csv(no_con_rel_LR_output_file)
            # print(no_con_rel_output_df[no_con_rel_output_df.isna().any(axis=0)])

            # no_con_rel_output_df.fillna(0, inplace=True)
            # print(no_con_rel_output_df.isna() == True)
            decomp_output_df = decomp_output_df.append(no_con_rel_output_df, ignore_index=True)

            df_final = pd.merge(df_merged_features, decomp_output_df, on=[key], how='outer')
            df_final.to_csv(features_collated_folder + "/collated.csv")
            print(df_final.isna().sum().sum())
            # df1 = df_final[df_final.isna().any(axis=1)]
            # print(df1[df1.isna()])
            # df_final.fillna(0,inplace=True)
            # print(df_final.isna().sum().sum())
            print("Finished " + instance_name)


if __name__ == "__main__":
    main()