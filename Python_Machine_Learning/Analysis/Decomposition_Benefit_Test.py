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

#global vars
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
# problem_types = ["network_design", "fixed_cost_network_flow"]

instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]

features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"



#fix ml training problem
# getBestMLSameProblem
def testDecompBenefit():

    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            df = pd.read_csv(features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv")
            get_min_decomp_score = df['Decomp Score'].min()
            if abs(df['Decomp Score'][0] - get_min_decomp_score) < 0.05:
                print("{} requires no relaxation for the best decomp score".format(instance_name))


def main():
    #first test, same problemTest_Type_Enum

    testDecompBenefit()
    #
    # # read in scores file
    # data_trained_on_list = ["network_design", "fixed_cost_network_flow",  "supply_network_planning", "all_problem_types"]





                    # for heuristics_name in heuristic_names:
                    #     heuristic_scores_df = pd.read_csv(
                    #         scores_folder + "/" + ML_name + "_" + data_trained_on + ".csv")
                    #     # get the decomp indexes of the best n decompisitions as predicted by the ML model
                    #     smallest_ml_values_decomp_indexes = ml_scores_df.nsmallest(10, 'ML predicted val')[
                    #         'Decomposition Index']
                    #     decomp_scores = []
                    #     for decomp_idx in smallest_ml_values_decomp_indexes:
                    #         decomp_scores.append(df_collated[df_collated['Decomposition Index'] == decomp_idx][
                    #                                  "Decomp Score"].values[0])
                        #read in scores for ML model predictions
                # read in collated
                # remove default index and Decomp index from dataframe to store features
                # X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)'])
                # X_np = X.to_numpy()
                # Path(
                #     model_prediction_output_folder + "/" + problem_type).mkdir(
                #     parents=True, exist_ok=True)
                # print("for problem type " + problem_type)
                #
                # for model_name in models:
                #     for data_trained_on in data_trained_on_list:
                #         with open(regression_models_pickle_input_folder + "/" + model_name + "_" + data_trained_on + ".pkl",
                #                   'rb') as pickle_input_fs:
                #             model = pickle.load(pickle_input_fs)
                #             print(type(model.predict(X_np)))
                #             df_ml = pd.DataFrame()
                #             df_ml['Decomposition Index'] = df['Decomposition Index']
                #             df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                #             ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Ranking_Method_Scores"
                #             Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                #             ML_output_filename = model_name + "_" + data_trained_on + ".csv"
                #             df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)
                #         print("Finished problem type {}, model {}, trained on {}".format(problem_type,model_name, data_trained_on))

#store the important features in a list
if __name__ == "__main__":

    main()