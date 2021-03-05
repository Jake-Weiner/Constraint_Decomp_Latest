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
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]


    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            relaxed_constraint_prop_path = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results" + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
            df = pd.read_csv(relaxed_constraint_prop_path)
            # reset the indexes
            df.reset_index(drop=True, inplace=True)
            RBA_scores = df[['Decomposition Index', 'Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats']]
            RBA_values_output_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Heuristic_Outputs" + "/" + problem_type + "/" + instance_name
            RBA_scores.to_csv(RBA_values_output_folder + "/" + "RBA_Scores.csv")

# store the important features in a list
if __name__ == "__main__":
    main()