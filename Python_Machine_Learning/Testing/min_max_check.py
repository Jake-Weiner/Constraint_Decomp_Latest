#importing libraries
import pandas as pd
import numpy as np

#test the ranges of the collated data to make sure that all data is between 0-1
def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    processed_results_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df.drop(columns=[df.columns[0], 'Decomposition Index'], inplace=True)
            print(df.max(axis=0).max(axis=0))
            print(df.min(axis=0).min(axis=0))


#store the important features in a list
if __name__ == "__main__":

    main()