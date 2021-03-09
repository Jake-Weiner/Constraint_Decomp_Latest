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

    features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Features_Calculated"

    regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
    model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated/Ranking"
    # prepare models
    ML_names = ['OLM','SVM','SGD','KNN','RF','MLP']
    heuristic_names = ["GCG1", "Goodness", "MW", "RBA"]
    number_of_batches = 10
    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Features_Calculated"
    batch_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Ranking/Batch_Results"

    # read in scores file
    data_trained_on_list = ["network_design", "fixed_cost_network_flow",  "supply_network_planning", "all_problem_types"]

    for ML_name in ML_names:
        for problem_type_idx, problem_type in enumerate(problem_types):
            # data_trained_on_list = [problem_type, "all_problem_types"]
            # data_trained_on_list = problem_types + "all_problem_types"
            for data_trained_on in data_trained_on_list:
                for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
                    batch_output_folder = batch_outputs_root_folder + "/" + problem_type + "/" + instance_name
                    Path(batch_output_folder).mkdir(parents=True, exist_ok=True)
                    #test if output file exists
                    my_file = Path(batch_output_folder + "/" + "batch_results.csv")
                    #write headers if file does not exist
                    if not my_file.is_file():
                        with open(batch_output_folder + "/" + "batch_results.csv", "w") as batch_outputs_fs:
                            batch_outputs_fs.write(
                                "Ranking Method,Mean-Mean Decomp Score,Mean-Decomp Score Stddev" + "\n")

                    with open(batch_output_folder + "/" + "batch_results.csv", "a+") as batch_outputs_fs:
                        batch_means = []
                        batch_stddevs = []
                        for batch_number in range(number_of_batches):
                            features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                                batch_number) + "/" + "Features_Collated"
                            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                                batch_number) + "/" + "Features_Collated" + "/" + "collated.csv"
                            # read in collated data, which contains the decomp value
                            df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
                            scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                                batch_number) + "/" + "Ranking_Method_Scores"

                            ml_scores_df = pd.read_csv(scores_folder + "/" + ML_name + "_" + data_trained_on + ".csv")
                            # get the decomp indexes of the best n decompisitions as predicted by the ML model
                            smallest_ml_values_decomp_indexes = ml_scores_df.nsmallest(10, 'ML predicted val')['Decomposition Index']
                            decomp_scores = []
                            #for each decomp index of the best 10 predicted decomps, get the actual decomp scores
                            for decomp_idx in smallest_ml_values_decomp_indexes:
                                decomp_scores.append(
                                    df_collated[df_collated['Decomposition Index'] == decomp_idx]["Decomp Score"].values[0])
                            #calculate both mean and stddev of best predicted decomps
                            batch_means.append(statistics.mean(decomp_scores))
                            batch_stddevs.append(statistics.pstdev(decomp_scores))

                        batch_outputs_fs.write("{}_{},{},{} \n".format(ML_name, data_trained_on, statistics.mean(batch_means),
                                                                              statistics.pstdev(batch_stddevs)))
                    print("Finished {}".format(instance_name))
                print("Finished {}".format(data_trained_on))
            print("Finished {}".format(problem_type))
        print("Finished {}".format(ML_name))

    for heuristic_name in heuristic_names:
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
                batch_output_folder = batch_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(batch_output_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                my_file = Path(batch_output_folder + "/" + "batch_results.csv")
                # write headers if file does not exist
                if not my_file.is_file():
                    with open(batch_output_folder + "/" + "batch_results.csv", "w") as batch_outputs_fs:
                        batch_outputs_fs.write(
                            "Ranking Method,Mean-Mean Decomp Score,Mean-Decomp Score Stddev" + "\n")

                with open(batch_output_folder + "/" + "batch_results.csv", "a+") as batch_outputs_fs:
                    batch_means = []
                    batch_stddevs = []
                    for batch_number in range(number_of_batches):
                        features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                            batch_number) + "/" + "Features_Collated"
                        input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                            batch_number) + "/" + "Features_Collated" + "/" + "collated.csv"
                        # read in collated data, which contains the decomp value
                        df_collated = pd.read_csv(features_collated_folder + "/collated.csv")
                        scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + str(
                            batch_number) + "/" + "Ranking_Method_Scores"

                        heuristic_scores_df = pd.read_csv(
                            scores_folder + "/" + heuristic_name + "_Scores" + ".csv")
                        # get the decomp indexes of the best n decompisitions as predicted by the ML model
                        smallest_heur_values_decomp_indexes = heuristic_scores_df.nsmallest(10, heuristic_name + " Scores")[
                            'Decomposition Index']
                        decomp_scores = []
                        # for each decomp index of the best 10 predicted decomps, get the actual decomp scores
                        for decomp_idx in smallest_heur_values_decomp_indexes:
                            decomp_scores.append(
                                df_collated[df_collated['Decomposition Index'] == decomp_idx][
                                    "Decomp Score"].values[0])
                        # calculate both mean and stddev of best predicted decomps
                        batch_means.append(statistics.mean(decomp_scores))
                        batch_stddevs.append(statistics.pstdev(decomp_scores))

                    batch_outputs_fs.write(
                        "{},{},{} \n".format(heuristic_name, statistics.mean(batch_means),
                                                statistics.pstdev(batch_stddevs)))
                print("Finished {}".format(instance_name))
            print("Finished {}".format(problem_type))
        print("Finished {}".format(ML_name))

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