import os
import pandas as pd
import numpy as np
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    decomp_processed_results_external_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    #collate the features in a single csv
    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'
    for problem_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # list of dataframes that need to be merged together
            df_list = []
            features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
            decomp_input_features_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Calculated"
            # create output folders if necessary
            Path(features_collated_folder).mkdir(parents=True, exist_ok=True)
            for folder_name in data_folders:
                for filename in os.listdir(decomp_input_features_folder + "/" + folder_name):
                    # remove .csv extension from files
                    filename_trimmed = os.path.splitext(filename)[0]
                    df_temp = pd.read_csv(decomp_input_features_folder + "/" + folder_name + "/" + filename)
                    if instance_name == "splice1k1.mps":
                        df_temp.drop(df_temp[df_temp['Decomposition Index'] == 1318].index, inplace=True)
                    df_temp.fillna(0, inplace=True)
                    if df_temp.isna().sum().sum() > 0:
                        print(instance_name)
                        print(df_temp[df_temp.isin([np.nan, np.inf, -np.inf]).any(1)])
                    # rename columns to be filename which describes what the features are
                    df_temp.columns = df_temp.columns.map(
                        lambda x: x + "_" + folder_name + "_" + filename_trimmed if x != key else x)
                    df_list.append(df_temp)
            # print("Finished reading in csvs for instance " + instance_name)
            # # merge all features into single csv
            df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                            how='outer'), df_list)


            #check for na's
            print(df_merged_features.columns[df_merged_features.isna().any(axis='rows')])
            print(df_merged_features[df_merged_features.isna().any(1)])
            print("Finished merging features for instance " + instance_name)
            #read in LR output file
            decomp_LR_output_file = decomp_processed_results_external_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            decomp_output_df = pd.read_csv(decomp_LR_output_file)
            df_final = pd.merge(df_merged_features, decomp_output_df, on=[key], how='outer')
            df_final["Decomp Score"] = 0.5 * df_final['Normalised Gap (%)'] + 0.5 * df_final['LR Solve Time(s)']
            df_final.to_csv(features_collated_folder + "/collated.csv")
            print(df_final.isna().sum().sum())
            print("Finished " + instance_name)

if __name__ == "__main__":
    main()