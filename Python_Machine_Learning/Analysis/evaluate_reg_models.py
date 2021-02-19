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
    instance_names_testing = [["ta2-UUE.mps"], [ "k16x240b.mps"], [ "snp-10-052-052.mps"]]
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]


    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"


    regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"

    model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"


    # prepare models
    models = ['OLM','SVM','SGD','KNN','DT','MLP']
    # models.append(('OLM', LinearRegression()))
    # models.append(('SVM', svm.SVR()))
    # models.append(('SGD', SGDRegressor()))
    # models.append(('KNN', KNeighborsRegressor()))
    # models.append(('DT', tree.DecisionTreeRegressor()))
    # models.append(('MLP', MLPRegressor()))
    regression_model_names = ["DT_network_design", "DT_fixed_cost_network_flow", "DT_supply_network_planning",
                              "DT_all_problem_types"]

    data_trained_on_list = ["network_design", "fixed_cost_network_flow",  "supply_network_planning", "all_problem_types"]


    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)
            # remove any rows with nan, inf or -inf vals

        df_combined = pd.concat(df_list, keys=instance_names_testing[problem_type_idx])
        #reset the indexes
        df_combined.reset_index(drop=True, inplace=True)

        # remove default index and Decomp index from dataframe to store features
        X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)'])
        # print(X.columns)
        # capture output columns
        Y = df_combined[['Normalised Gap (%)', 'LR Solve Time(s)']]

        X_relaxed_con_prop = df_combined[['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats']]
        X_relaxed_con_prop_np = X_relaxed_con_prop.to_numpy()
        # convert features to np array
        X_np = X.to_numpy()
        Bound_np = Y['Normalised Gap (%)'].to_numpy()
        # print(Bound_np)
        # scoring_method = 'neg_mean_squared_error'
        # train each model on 75% of data
        # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size=0.25, shuffle=True,
        #                                                  random_state=1)

        rmse_scores = []
        Path(
            model_prediction_output_folder + "/" + problem_type).mkdir(
            parents=True, exist_ok=True)
        print("for problem type " + problem_type)
        for model_name in models:
            for data_trained_on in data_trained_on_list:
                with open(regression_models_pickle_input_folder + "/" + model_name + "_" + data_trained_on + ".pkl",
                          'rb') as pickle_input_fs:
                    model = pickle.load(pickle_input_fs)
                    mse = mean_squared_error(Bound_np, model.predict(X_np))
                    rmse_scores.append((math.sqrt(mse), model_name + "_trained_on_" + data_trained_on))
                    # print("model %s has rmse - %f" % (model_name, math.sqrt(mse)))
                    plt.figure()
                    plt.ylabel("Prediction Gap")
                    plt.xlabel("Actual Gap")
                    #plot prediction vs actual gap
                    plt.scatter(Bound_np, model.predict(X_np))
                    plt.title('Prediction vs Actual for ' + problem_type + ' - ' + model_name)
                    plt.tight_layout()
                    plt.savefig(model_prediction_output_folder + "/" + problem_type + "/" + model_name + "_trained_on_" + data_trained_on + "_prediction")
                print("Finished problem type {}, model {}, trained on {}".format(problem_type,model_name, data_trained_on))

        plt.figure()
        plt.ylabel("Con Relaxed Prop")
        plt.xlabel("Actual Gap")
        # plot prediction vs actual gap
        plt.scatter(Bound_np, X_relaxed_con_prop_np)
        plt.title('Con Relax Prop vs Gap ' + problem_type + ' - ' + model_name)
        plt.tight_layout()
        plt.savefig(
            model_prediction_output_folder + "/" + problem_type + "/" + "con_relax_vs_gap")

        #NEED to create a new figure each time in order to plot the new data

        rmse_scores.sort(key=lambda method: method[0])
        print(rmse_scores)
        plt.figure()
        plt.ylim(0, 1)
        x_pos = np.arange(len(rmse_scores))
        plt.bar(x_pos, [rmse_score_tuple[0] for rmse_score_tuple in rmse_scores] , color='blue')
        plt.xticks(x_pos, [textwrap.fill(rmse_score_tuple[1], 20) for rmse_score_tuple in rmse_scores],
                   rotation=90, fontsize=8, horizontalalignment="center")
        plt.ylabel('RMSE')
        plt.title('RMSE scores for ' + problem_type)
        plt.tight_layout()

        plt.savefig(model_prediction_output_folder + "/" + problem_type + "/" + "rmse_scores")


#store the important features in a list
if __name__ == "__main__":

    main()