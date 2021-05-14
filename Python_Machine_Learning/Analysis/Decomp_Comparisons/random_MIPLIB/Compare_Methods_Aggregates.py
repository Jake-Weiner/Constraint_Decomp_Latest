
#for each problem type (granularity)

#for all instances - ave best,

#normalise the best decomps predicted, to then aggregate it fairly across different instances


#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import functools
import csv
import statistics
from sklearn.preprocessing import MinMaxScaler
from sklearn.preprocessing import RobustScaler
import os
#global vars
problem_types = ["random_MIPLIB"]
instance_names = [
    ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
     "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps"]]

ML_models = ['Voting']
heuristic_methods = ["GCG1", "Goodness", "MW", "RBA"]

model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

model_comparisons_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"

def collateAllPredictions():
    for problem_type_idx, problem_type in enumerate(problem_types):
        model_aggregates_outputs_folder = model_comparisons_outputs_root_folder + "/" + problem_type
        problem_specific_ranking_method_dict = dict()
        df_list = []
        key = 'Ranking Method'
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "predicted_decomp_scores.csv"
            df = pd.read_csv(model_comparisons_path)
            # remove .csv extension from files
            filename_trimmed = os.path.splitext(instance_name)[0]
            df.rename(columns={'Best Decomp Score': filename_trimmed}, inplace=True)
            # rename columns to be filename which describes what the features are
            df_list.append(df)
    # print("Finished reading in csvs for instance " + instance_name)
    # # merge all features into single csv
        df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                    how='outer'), df_list)
        print(df_merged_features)
        df_merged_features.to_csv(
            model_comparisons_outputs_root_folder + "/" + problem_type + ".csv")

def collateAllScaledPredictions():
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        key = 'Ranking Method'
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "predicted_decomp_scores.csv"
            df = pd.read_csv(model_comparisons_path)
            best_decomp_scores_np = df["Best Decomp Score"].values.reshape(-1,1)
            # scaler = MinMaxScaler()
            # best_decomp_scores_np_scaled = scaler.fit_transform(best_decomp_scores_np)
            df["Best Decomp Score"] = best_decomp_scores_np
            # remove .csv extension from files
            filename_trimmed = os.path.splitext(instance_name)[0]
            df.rename(columns={'Best Decomp Score': filename_trimmed}, inplace=True)
            # rename columns to be filename which describes what the features are
            df_list.append(df)
    # print("Finished reading in csvs for instance " + instance_name)
    # # merge all features into single csv
        df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=[key],
                                                    how='outer'), df_list)
        df_merged_features.to_csv(
            model_comparisons_outputs_root_folder + "/" + problem_type + "_scaled.csv")

def main():
    # collateAllPredictions()
    collateAllScaledPredictions()

#store the important features in a list
if __name__ == "__main__":
    main()

