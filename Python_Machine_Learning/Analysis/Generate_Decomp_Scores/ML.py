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

def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names_testing = [["germany50-UUM.mps"], [ "k16x240b.mps"], [ "snp-10-052-052.mps"]]
    data_trained_on_list = ["network_design", "fixed_cost_network_flow",  "supply_network_planning", "all_problem_types"]
    features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
    model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    # prepare models
    models = ['OLM','SVM','SGD','KNN','RF','MLP']


    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):

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
                for data_trained_on in data_trained_on_list:
                    with open(regression_models_pickle_input_folder + "/" + model_name + "_" + data_trained_on + ".pkl",
                              'rb') as pickle_input_fs:
                        model = pickle.load(pickle_input_fs)
                        df_ml = pd.DataFrame()
                        df_ml['Decomposition Index'] = df['Decomposition Index']
                        df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                        ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                        Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                        ML_output_filename = model_name + "_" + data_trained_on + ".csv"
                        df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)

                        #plot ML score against actual
                        plt.figure()
                        plt.ylabel("Predicted Score")
                        plt.xlabel("Decomp Score")
                        # plot prediction vs actual gap
                        plt.scatter(Y_np, model.predict(X_np))
                        plt.title('Predicted Score vs Decomp Score ' + problem_type + ' - ' + model_name)
                        plt.tight_layout()
                        plt.savefig(
                            ML_output_folder + "/" + model_name + "_trained_on_" + data_trained_on + "_prediction")
                    print("Finished problem type {}, model {}, trained on {}".format(problem_type,model_name, data_trained_on))

#store the important features in a list
if __name__ == "__main__":

    main()