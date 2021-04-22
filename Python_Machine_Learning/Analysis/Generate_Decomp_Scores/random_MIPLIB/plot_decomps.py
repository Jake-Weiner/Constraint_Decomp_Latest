
#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
import statistics
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

def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names_testing = [["germany50-UUM.mps"], [ "k16x240b.mps"], [ "snp-10-052-052.mps"]]

    features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
    model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    # prepare models
    ML_names = ['OLM','SVM','SGD','KNN','RF','MLP']
    heuristic_names = ["GCG1", "Goodness", "MW", "RBA"]

    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    batch_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Batch_Results"

    # read in scores file
    data_trained_on_list = ["network_design", "fixed_cost_network_flow",  "supply_network_planning", "all_problem_types"]

    # for ML_name in ML_names:
    #     for problem_type_idx, problem_type in enumerate(problem_types):
    #         # data_trained_on_list = [problem_type, "all_problem_types"]
    #         # data_trained_on_list = problem_types + "all_problem_types"
    #         for data_trained_on in data_trained_on_list:
    #             for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
    #                 batch_output_folder = batch_outputs_root_folder + "/" + problem_type + "/" + instance_name
    #                 Path(batch_output_folder).mkdir(parents=True, exist_ok=True)
    #
    #                 features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
    #                 input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
    #                 # read in collated data, which contains the decomp value
    #                 df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
    #                 X_np = df_collated["Decomp Score"]
    #                 scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
    #                 ml_scores_df = pd.read_csv(scores_folder + "/" + ML_name + "_" + data_trained_on + ".csv")
    #                 Y_np = ml_scores_df['ML predicted val']
    #                 # plot ML score against actual
    #                 plt.figure()
    #                 plt.ylabel("Predicted Score")
    #                 plt.xlabel("Decomp Score")
    #                 # plot prediction vs actual gap
    #                 plt.scatter(X_np, Y_np)
    #                 plt.title('Predicted Score vs Decomp Score ' + problem_type + ' - ' + ML_name)
    #                 plt.tight_layout()
    #                 plt.savefig(
    #                     batch_output_folder + "/" + ML_name + "_trained_on_" + data_trained_on + "_prediction")
    #
    #                 print("Finished {}".format(instance_name))
    #             print("Finished {}".format(data_trained_on))
    #         print("Finished {}".format(problem_type))
    #     print("Finished {}".format(ML_name))

    for heuristic_name in heuristic_names:
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
                batch_output_folder = batch_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(batch_output_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                # write headers if file does not exist
                features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
                input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
                # read in collated data, which contains the decomp value
                df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
                X_np = df_collated["Decomp Score"]
                scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                heuristic_scores_df = pd.read_csv(
                    scores_folder + "/" + heuristic_name + "_Scores" + ".csv")
                Y_np = heuristic_scores_df[heuristic_name + " Scores"]
                # get the decomp indexes of the best n decompisitions as predicted by the ML model

                # for ease of chart comparison, RBA should be minimized, the rest should be maximised, so convert scores to 1-score
                if heuristic_name != 'RBA':
                    Y_np = 1 - Y_np

                plt.figure()
                plt.ylabel("Heuristic Score")
                plt.xlabel("Decomp Score")
                # plot prediction vs actual gap
                plt.scatter(X_np, Y_np)
                plt.title('Heuristic Score vs Decomp Score ' + heuristic_name)
                plt.tight_layout()
                plt.savefig(
                    batch_output_folder + "/" + heuristic_name)
                print("Finished {}".format(instance_name))
            print("Finished {}".format(problem_type))



#store the important features in a list
if __name__ == "__main__":

    main()