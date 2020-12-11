import os
import pandas as pd
import functools


def main():

    #collate the features in a single csv
    input_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Calculated"
    output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Collated"
    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'
    df_list = []

    for folder_name in data_folders:
        for filename in os.listdir(input_root_folder + "/" + folder_name):
            #remove .csv extension from files
            filename_trimmed = os.path.splitext(filename)[0]
            df_temp = pd.read_csv(input_root_folder + "/" + folder_name + "/" + filename)
            # rename columns
            df_temp.columns = df_temp.columns.map(lambda x: x + "_" + folder_name + "_" + filename_trimmed  if x != key else x)
            df_list.append(df_temp)

    # merge all features into single csv
    df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                    how='outer'), df_list)
    #read in output file
    LR_output_file = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Normalised_Data/LROutputs/LR_outputs.csv"
    output_df = pd.read_csv(LR_output_file)
    df_final = pd.merge(df_merged_features, output_df, on=[key], how='outer')
    df_final.to_csv(output_folder + "/collated.csv")
    print(df_final)
if __name__ == "__main__":
    main()