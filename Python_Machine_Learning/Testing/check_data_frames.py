#importing libraries
import pandas as pd
import numpy as np


def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    processed_results_folder = processed_results_local_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)


        df_combined = pd.concat(df_list, keys=instance_names[problem_type_idx])

        print("before removing nan, inf or -inf rows shape is (" + str(df_combined.shape[0]) + "," + str(df_combined.shape[1]) + ")")
        #check to see if there are any
        #remove any rows in which there are nan, inf or -inf values
        df_combined_removed_inf = df_combined[~df_combined.isin([np.nan, np.inf, -np.inf]).any(1)]
        print("after removing nan, inf or -inf rows shape is (" + str(df_combined_removed_inf.shape[0]) + "," + str(
            df_combined_removed_inf.shape[1]) + ")")

        print("before removing gap<0%, shape is (" + str(df_combined.shape[0]) + "," + str(
            df_combined.shape[1])+ ")")
        df_combined_gap_0 = df_combined[df_combined['Gap (%)'] > 0]
        print("after removing gap<0%, shape is (" + str(df_combined_gap_0.shape[0]) + "," + str(
            df_combined_gap_0.shape[1]) + ")")

#store the important features in a list
if __name__ == "__main__":

    main()