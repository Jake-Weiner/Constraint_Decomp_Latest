import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from sklearn.model_selection import cross_validate
from sklearn.model_selection import cross_val_score
from sklearn.model_selection import KFold
from sklearn.metrics import mean_squared_error
from sklearn.linear_model import LinearRegression
from sklearn import svm
from sklearn.linear_model import SGDRegressor
from sklearn.neighbors import KNeighborsRegressor
from sklearn import tree
from sklearn.neural_network import MLPRegressor
from sklearn.model_selection import train_test_split
import os
import statistics
import pickle
import math
import textwrap

# Bergner scores are Relative Border Area, which is just the proportion of relaxed constraints


def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names_testing = [["germany50-UUM.mps"], [ "k16x240b.mps"], [ "snp-10-052-052.mps"]]


    number_of_batches = 10
    features_calucated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Features_Calculated"
    external_process_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Ranking"
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            for batch_number in range(number_of_batches):
                relaxed_constraint_prop_path = external_process_results_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
                df = pd.read_csv(relaxed_constraint_prop_path)
                # reset the indexes
                df.reset_index(drop=True, inplace=True)
                RBA_scores = df[['Decomposition Index', 'Relaxed Constraint Prop']]
                RBA_scores.rename(columns={"Relaxed Constraint Prop": "RBA Scores"}, inplace=True)
                RBA_values_output_folder = features_calucated_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Ranking_Method_Scores"
                Path(RBA_values_output_folder).mkdir(parents=True, exist_ok=True)
                RBA_scores.to_csv(RBA_values_output_folder + "/" + "RBA_Scores.csv")

# store the important features in a list
if __name__ == "__main__":
    main()