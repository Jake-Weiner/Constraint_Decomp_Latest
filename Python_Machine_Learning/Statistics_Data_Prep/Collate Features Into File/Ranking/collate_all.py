import os
import pandas as pd
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names_testing = [["germany50-UUM.mps"], ["k16x240b.mps"], ["snp-10-052-052.mps"]]

    # problem_types = ["network_design", "fixed_cost_network_flow"]
    #
    # instance_names_testing = [["germany50-UUM.mps"], ["k16x240b.mps"]]

    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Features_Calculated"
    decomp_processed_results_external_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Ranking"
    #collate the features in a single csv
    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'
    number_of_batches = 10

    for problem_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_idx]):
            for batch_number in range(number_of_batches):
                # list of dataframes that need to be merged together
                df_list = []
                features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Features_Collated"
                decomp_input_features_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Features_Calculated"
                # create output folders if necessary
                Path(features_collated_folder).mkdir(parents=True, exist_ok=True)
                for folder_name in data_folders:
                    for filename in os.listdir(decomp_input_features_folder + "/" + folder_name):
                        # remove .csv extension from files
                        filename_trimmed = os.path.splitext(filename)[0]
                        df_temp = pd.read_csv(decomp_input_features_folder + "/" + folder_name + "/" + filename)
                        df_temp.fillna(0, inplace=True)
                        # rename columns to be filename which describes what the features are
                        df_temp.columns = df_temp.columns.map(
                            lambda x: x + "_" + folder_name + "_" + filename_trimmed if x != key else x)
                        df_list.append(df_temp)
                print("Finished reading in csvs for instance " + instance_name)
                # merge all features into single csv
                df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                                how='outer'), df_list)

                #check for na's
                print(df_merged_features.columns[df_merged_features.isna().any(axis='rows')])
                print("Finished merging features for instance " + instance_name)
                #read in LR output file
                decomp_LR_output_file = decomp_processed_results_external_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
                decomp_output_df = pd.read_csv(decomp_LR_output_file)
                df_final = pd.merge(df_merged_features, decomp_output_df, on=[key], how='outer')
                df_final["Decomp Score"] =  0.5 * df_final['Normalised Gap (%)'] + 0.5 * df_final['LR Solve Time(s)']
                df_final.to_csv(features_collated_folder + "/collated.csv")
                print(df_final.isna().sum().sum())
                print("Finished " + instance_name)


if __name__ == "__main__":
    main()