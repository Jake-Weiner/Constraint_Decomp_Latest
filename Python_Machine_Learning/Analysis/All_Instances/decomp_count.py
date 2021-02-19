# importing libraries
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


def main():
    # instance_names_training = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
    #                            ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps"],
    #                            ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps"
    #                             ]]
    # instance_names_training_flat_list = ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps",
    #                                      "g200x740.mps", "h50x2450.mps", "h80x6320d.mps",
    #                                      "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps",
    #                                      "snp-10-004-052.mps"]

    instance_names_testing = []
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]
    # instance_names_flat_list= ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps",
    #                   "g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps",
    #                   "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]
    # create sets for important features

    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    regression_models_pickle_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"
    write_cfv_results = True
    cfv_results_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_CFV"
    cfv_results = []

    # models.append(('DT', tree.DecisionTreeRegressor()))

    # train DT model on each problem type
    for problem_type_idx, problem_type in enumerate(problem_types):

        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            print(str(df.shape[0]) +" decomps for " + instance_name)
            # remove any rows with nan, inf or -inf vals


# store the important features in a list
if __name__ == "__main__":
    main()