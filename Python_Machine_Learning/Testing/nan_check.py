import statistics
import csv
import os
from shutil import copyfile
from pathlib import Path
import pandas as pd
import numpy as np
# This script calculates tests all relaxed constraint features
def main():



    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    # processed_results_folders = [external_processed_results_folder, processed_no_con_rel_results_folder]

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            features_calculated_output_path = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/Features_Calculated" + "/" + "Relaxed_Constraint_Statistics"
            for filename in os.listdir(features_calculated_output_path):
                df = pd.read_csv(features_calculated_output_path + "/" + filename)
                print(df[df.isin([np.nan, np.inf, -np.inf]).any(1)])
            #
            # #create output folders if necessary
            # Path(features_calculated_output_path).mkdir(parents=True, exist_ok=True)
            # normalised_data_input_folder =  external_processed_results_folder + "/" + problem_type + "/" + instance_name + "/Normalised_Data" + "/" + "Relaxed_Constraint_Statistics"
            # # for each relaxed constraint file, calculate and output the features
            # for filename in os.listdir(normalised_data_input_folder):
            #     if filename != "single_stats.csv":
            #         writeAllStats(normalised_data_input_folder + "/" + filename, features_calculated_output_path + "/" + filename)
            #
            # # single stats file doesn't require any manipulation, just copy the files across as these are already features
            # copyfile(normalised_data_input_folder + "/" + "single_stats.csv", features_calculated_output_path + "/" + "single_stats.csv")
            # print("Finished " + instance_name)

if __name__ == "__main__":

    main()