import pandas as pd
import numpy as np
from pathlib import Path

# Bergner scores are Relative Border Area, which is just the proportion of relaxed constraints


def main():

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    features_calucated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    external_process_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            relaxed_constraint_prop_path = external_process_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
            df = pd.read_csv(relaxed_constraint_prop_path)
            # reset the indexes
            df.reset_index(drop=True, inplace=True)
            RBA_scores = df[['Decomposition Index', 'Relaxed Constraint Prop']]
            RBA_scores.rename(columns={"Relaxed Constraint Prop": "RBA Scores"}, inplace=True)
            RBA_values_output_folder = features_calucated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
            Path(RBA_values_output_folder).mkdir(parents=True, exist_ok=True)
            RBA_scores.to_csv(RBA_values_output_folder + "/" + "RBA_Scores.csv")
            print("Finished {}".format(instance_name))
# store the important features in a list
if __name__ == "__main__":
    main()