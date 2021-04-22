import os
import pandas as pd
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    # problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    # # problem_types = ["network_design", "fixed_cost_network_flow"]
    # # instance_names = [["germany50-UUM.mps"], ["k16x240b.mps"], ["snp-10-052-052.mps"]]
    #
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]
    # problem_types = ["network_design", "fixed_cost_network_flow"]
    #
    # instance_names = [["germany50-UUM.mps"], ["k16x240b.mps"]]

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    #for splice1k1, the final decomposition ran out of time in collecting feature statistics. Therefore ignore the final
    # decompostion with decomp index of 1318
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
            decomp_LR_output_file = decomp_processed_results_external_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            decomp_output_df = pd.read_csv(decomp_LR_output_file)
            df_final = pd.merge(df_merged_features, decomp_output_df, on=[key], how='outer')
            df_final["Decomp Score"] = 0.5 * df_final['Normalised Gap (%)'] + 0.5 * df_final['LR Solve Time(s)']
            df_final.to_csv(features_collated_folder + "/collated.csv")
            print(df_final.isna().sum().sum())
            print("Finished " + instance_name)

if __name__ == "__main__":
    main()