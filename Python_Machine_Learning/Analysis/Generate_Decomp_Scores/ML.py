#importing libraries
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
plt.style.use('ggplot')

#global vars
models = ['OLM', 'SVM', 'SGD', 'KNN', 'RF', 'MLP']
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
# problem_types = ["network_design", "fixed_cost_network_flow"]
instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]
data_trained_on_list = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "all_problem_types"]
features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"


# prepare models
#def scores for same problem type
def generateScoresSameProblem():
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            #Read in statistics
            df = pd.read_csv(input_data_filepath)
            # remove default index and Decomp index from dataframe to store features
            Y_np = df['Decomp Score'].to_numpy()
            X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)', 'Decomp Score'])
            X_np = X.to_numpy()
            Path(
                model_prediction_output_folder + "/" + problem_type).mkdir(
                parents=True, exist_ok=True)

            print("for problem type " + problem_type)
            # data_trained_on_list = [problem_type, "all_problem_types"]
            for model_name in models:
                saved_model_name = model_name + "_" + problem_type + "_" + instance_name
                with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                          'rb') as pickle_input_fs:
                    model = pickle.load(pickle_input_fs)
                    df_ml = pd.DataFrame()
                    df_ml['Decomposition Index'] = df['Decomposition Index']
                    df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                    ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                    ML_output_filename = saved_model_name + ".csv"
                    df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)

                    # #plot ML score against actual
                    # plt.figure()
                    # plt.ylabel("Predicted Score")
                    # plt.xlabel("Decomp Score")
                    # # plot prediction vs actual gap
                    # plt.scatter(Y_np, model.predict(X_np))
                    # plt.title('Predicted Score vs Decomp Score ' + problem_type + ' - ' + model_name)
                    # plt.tight_layout()
                    # plt.savefig(
                    #     ML_output_folder + "/" + saved_model_name + "_prediction")
                print("Finished problem type {}, model {}".format(problem_type,model_name))

    return

def generateScoresAllProblems():
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            #Read in statistics
            df = pd.read_csv(input_data_filepath)
            # remove default index and Decomp index from dataframe to store features
            Y_np = df['Decomp Score'].to_numpy()
            X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)', 'Decomp Score'])
            X_np = X.to_numpy()
            Path(
                model_prediction_output_folder + "/" + problem_type).mkdir(
                parents=True, exist_ok=True)

            print("for problem type " + problem_type)
            # data_trained_on_list = [problem_type, "all_problem_types"]
            for model_name in models:
                saved_model_name = model_name + "_" + "all_problem_types" + "_" + instance_name
                with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                          'rb') as pickle_input_fs:
                    model = pickle.load(pickle_input_fs)
                    df_ml = pd.DataFrame()
                    df_ml['Decomposition Index'] = df['Decomposition Index']
                    df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                    ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                    ML_output_filename = saved_model_name + ".csv"
                    df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)

                    # #plot ML score against actual
                    # plt.figure()
                    # plt.ylabel("Predicted Score")
                    # plt.xlabel("Decomp Score")
                    # # plot prediction vs actual gap
                    # plt.scatter(Y_np, model.predict(X_np))
                    # plt.title('Predicted Score vs Decomp Score ' + problem_type + ' - ' + model_name)
                    # plt.tight_layout()
                    # plt.savefig(
                    #     ML_output_folder + "/" + saved_model_name + "_prediction")
                print("Finished problem type {}, model {}".format(problem_type,model_name))
    return

#write in list with all problem types, go through except for specific problem
def gernerateScoresOtherProblems():
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            # Read in statistics
            df = pd.read_csv(input_data_filepath)
            # remove default index and Decomp index from dataframe to store features
            Y_np = df['Decomp Score'].to_numpy()
            X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                                 'Decomp Score'])
            X_np = X.to_numpy()
            Path(
                model_prediction_output_folder + "/" + problem_type).mkdir(
                parents=True, exist_ok=True)

            print("for problem type " + problem_type)
            # data_trained_on_list = [problem_type, "all_problem_types"]
            for model_name in models:
                # loop through all problem types and generate scores using models trained only on different problem types
                for inner_problem_type in problem_types:
                    if inner_problem_type != problem_type:
                        saved_model_name = model_name + "_" + inner_problem_type
                        with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                                  'rb') as pickle_input_fs:
                            model = pickle.load(pickle_input_fs)
                            df_ml = pd.DataFrame()
                            df_ml['Decomposition Index'] = df['Decomposition Index']
                            df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                            ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                            Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                            ML_output_filename = saved_model_name + ".csv"
                            df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)

                    # #plot ML score against actual
                    # plt.figure()
                    # plt.ylabel("Predicted Score")
                    # plt.xlabel("Decomp Score")
                    # # plot prediction vs actual gap
                    # plt.scatter(Y_np, model.predict(X_np))
                    # plt.title('Predicted Score vs Decomp Score ' + problem_type + ' - ' + model_name)
                    # plt.tight_layout()
                    # plt.savefig(
                    #     ML_output_folder + "/" + saved_model_name + "_prediction")
                print("Finished problem type {}, model {}".format(problem_type, model_name))

    return

#def scores for all instances

#def scores for other problem types

def main():
    generateScoresSameProblem()
    generateScoresAllProblems()
    gernerateScoresOtherProblems()

#store the important features in a list
if __name__ == "__main__":

    main()