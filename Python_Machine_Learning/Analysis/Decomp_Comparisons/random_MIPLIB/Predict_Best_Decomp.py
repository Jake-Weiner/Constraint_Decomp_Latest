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

problem_types = ["random_MIPLIB"]
instance_names = [
    ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
     "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
# prepare models
ML_names = ['Voting']
heuristic_names = ["GCG1", "Goodness", "MW", "RBA"]
data_trained_on_list = ["all_network_instances"]

features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
model_comparisons_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"



class Test_Type_Enum:
    SAME_PROBLEM = 1
    DIFFERENT_PROBLEM = 2
    ALL_PROBLEMS = 3


def getBestHeuristicDecomps():
    # get the best decomposition scores based on predicted Heuristic outputs
    for heuristic_name in heuristic_names:
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
                best_decomp_score_folder = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(best_decomp_score_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                my_file = Path(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv")
                # write headers if file does not exist
                if not my_file.is_file():
                    with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "w") as predicted_decomp_scores_output_fs:
                        predicted_decomp_scores_output_fs.write(
                            "Ranking Method,Best Decomp Score,RMSE" + "\n")

                with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "a+") as predicted_decomp_scores_output_fs:
                    features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
                    input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
                    # read in collated data, which contains the decomp value
                    df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
                    true_min_decomp_score = df_collated["Decomp Score"].min()
                    true_max_decomp_score = df_collated["Decomp Score"].max()

                    scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"

                    heuristic_scores_df = pd.read_csv(
                        scores_folder + "/" + heuristic_name + "_Scores" + ".csv")
                    # get the decomp indexes of the best n decompisitions as predicted by the ML model
                    best_heur_values_decomp_indexes = pd.DataFrame()
                    if heuristic_name == 'RBA' or heuristic_name == "GCG1":
                        best_heur_values_decomp_indexes = heuristic_scores_df.nsmallest(8, heuristic_name + " Scores")[
                        'Decomposition Index']
                    else:
                        best_heur_values_decomp_indexes = heuristic_scores_df.nlargest(8, heuristic_name + " Scores")[
                            'Decomposition Index']
                    decomp_scores = []

                    # for each decomp index of the best 8 predicted decomps, get the actual decomp scores
                    for decomp_idx in best_heur_values_decomp_indexes:
                        decomp_scores.append(
                            df_collated[df_collated['Decomposition Index'] == decomp_idx][
                                "Decomp Score"].values[0])
                    # calculate both mean and stddev of best predicted decomps
                    best_score = min(decomp_scores)
                    best_score_scaled = ((best_score - true_min_decomp_score) / (true_max_decomp_score - true_min_decomp_score))

                    rmse = math.sqrt(mean_squared_error(df_collated["Decomp Score"], heuristic_scores_df[heuristic_name + " Scores"]))
                    print("MSE is {}".format(math.sqrt(mse)))
                    predicted_decomp_scores_output_fs.write(
                        "{},{},{} \n".format(heuristic_name, best_score_scaled, rmse))
                print("Finished {}".format(instance_name))
            print("Finished {}".format(problem_type))
        print("Finished {}".format(heuristic_name))

#fix ml training problem
# getBestMLSameProblem
def getBestMLDecomps():
    # get the best decomposition scores based on predicted ML outputs
    for ML_name in ML_names:
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
                best_decomp_score_folder = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(best_decomp_score_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                my_file = Path(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv")
                # write headers if file does not exist
                if not my_file.is_file():
                    with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "w") as predicted_decomp_scores_output_fs:
                        predicted_decomp_scores_output_fs.write(
                            "Ranking Method,Best Decomp Score,RMSE" + "\n")

                with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "a+") as predicted_decomp_scores_output_fs:
                    # read in features into dataframe
                    features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
                    # read in collated data, which contains the decomp value
                    df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
                    true_min_decomp_score = df_collated["Decomp Score"].min()
                    true_max_decomp_score = df_collated["Decomp Score"].max()
                    scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    ml_scores_df = pd.read_csv(scores_folder + "/" + ML_name + "_" + "all_network_instances" + ".csv")

                    # print("MSE is {}".format(math.sqrt(mse)))
                    smallest_ml_values_decomp_indexes = ml_scores_df.nsmallest(8, 'ML predicted val')[
                        'Decomposition Index']
                    decomp_scores = []
                    # for each decomp index of the best 8 predicted decomps, get the actual decomp scores
                    for decomp_idx in smallest_ml_values_decomp_indexes:
                        decomp_scores.append(
                            df_collated[df_collated['Decomposition Index'] == decomp_idx]["Decomp Score"].values[0])
                    # calculate best decomp score amongst the top 8 predicted decomps
                    best_decomp_score = min(decomp_scores)
                    best_score_scaled = (
                                (best_decomp_score - true_min_decomp_score) / (true_max_decomp_score - true_min_decomp_score))
                    rmse = math.sqrt(mean_squared_error(df_collated["Decomp Score"],
                                             ml_scores_df['ML predicted val']))
                    predicted_decomp_scores_output_fs.write(
                        "{},{},{}\n".format(ML_name, best_score_scaled, rmse))
                    print("Finished {}".format(instance_name))
            print("Finished {}".format(problem_type))
        print("Finished {}".format(ML_name))
    return

def main():

    getBestMLDecomps()
    getBestHeuristicDecomps()

#store the important features in a list
if __name__ == "__main__":

    main()