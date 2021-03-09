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
from sklearn.ensemble import RandomForestRegressor
from sklearn.neural_network import MLPRegressor
from sklearn.model_selection import train_test_split
import os
import statistics
import pickle

def main():
    instance_names_training = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "ta1-UUM.mps"],
                      ["g200x740.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps"
                       ]]
    instance_names_training_flat_list= ["cost266-UUE.mps", "dfn-bwin-DBE.mps",
                      "g200x740.mps",
                      "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps"]
    
    instance_names_testing = []
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]
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


    models = []
    models.append(('OLM', LinearRegression()))
    models.append(('SVM', svm.SVR()))
    models.append(('SGD', SGDRegressor()))
    models.append(('KNN', KNeighborsRegressor()))
    models.append(('RF', RandomForestRegressor()))
    models.append(('MLP', MLPRegressor()))

    #train DT model on each problem type
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_training[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)
            # remove any rows with nan, inf or -inf vals

        df_combined = pd.concat(df_list, keys=instance_names_training[problem_type_idx])
        # reset the indexes
        df_combined.reset_index(drop=True, inplace=True)

        # remove default index and Decomp index from dataframe to store features
        X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)'])
        # print(X.columns)
        # capture output columns
        Y = 0.5 * df_combined['Normalised Gap (%)'] + 0.5 * df_combined['LR Solve Time(s)']

        # convert features to np array
        X_np = X.to_numpy()
        Y_np = Y.to_numpy()

        # scoring_method = 'neg_mean_squared_error'
        #train each model on 75% of data
        # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size = 0.25, shuffle = True, random_state = 1)
        for model_name, model in models:
            reg_model = model.fit(X_np, Y_np)

            # store models with pickle
            with open(regression_models_pickle_output_folder + "/" + model_name + "_" + problem_type + ".pkl",
                      'wb') as pickle_output_fs:
                pickle.dump(reg_model, pickle_output_fs)
            print("Finished model {} for problem type {}".format(model_name, problem_type))

    # train DT model on all_instances
    df_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_training[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)


    df_combined = pd.concat(df_list, keys=instance_names_training_flat_list)
    # reset the indexes
    df_combined.reset_index(drop=True, inplace=True)

    # remove default index and Decomp index from dataframe to store features
    X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', "Normalised Gap (%)", 'LR Solve Time(s)'])
    # print(X.columns)
    # capture output columns
    # Y = df_combined[['Normalised Gap (%)', 'LR Solve Time(s)']]
    Y = 0.5 * df_combined['Normalised Gap (%)'] + 0.5 * df_combined['LR Solve Time(s)']

    # convert features to np array
    X_np = X.to_numpy()
    Y_np = Y.to_numpy()

    # scoring_method = 'neg_mean_squared_error'
    # train each model on 75% of data
    # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size=0.25, shuffle=True,
    #                                                     random_state=1)
    for model_name, model in models:
        reg_model = model.fit(X_np, Y_np)

        # store models with pickle
        with open(regression_models_pickle_output_folder + "/" + model_name + "_" + "all_problem_types" + ".pkl",
                  'wb') as pickle_output_fs:
            pickle.dump(reg_model, pickle_output_fs)
        print("Finished model {} for all problem types".format(model_name))

# store the important features in a list
if __name__ == "__main__":
    main()