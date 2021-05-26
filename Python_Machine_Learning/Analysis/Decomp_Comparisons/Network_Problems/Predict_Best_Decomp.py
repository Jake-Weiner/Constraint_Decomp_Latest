#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt

from sklearn.metrics import mean_squared_error
import scipy
import math
import csv
import textwrap
plt.style.use('ggplot')

#global vars
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]

features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
training_rmse_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/RMSE_training_scores"

# prepare models
ML_names = ['OLM', 'SVM', 'SGD', 'KNN', 'RF', 'MLP', 'Stacking', 'Voting']
heuristic_names = ["GCG1", "Goodness", "MW", "RBA"]

features_calculated_output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
model_comparisons_outputs_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons"

data_trained_on_list = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "all_problem_types"]

ranking_methods = ML_names + heuristic_names

class TrainingTypeEnum:
    SAME = 1
    DIFFERENT = 2
    ALL = 3

#get the best decomp score amongst the top 8 predicted decompositions for all ranking methods
def getBestPredictedDecomps():
    # get the best decomposition scores based on predicted ML outputs
    for ranking_method in ranking_methods:
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
                best_decomp_score_folder = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(best_decomp_score_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                my_file = Path(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv")
                # write headers if file does not exist
                if not my_file.is_file():
                    with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "w") as predicted_decomp_scores_output_fs:
                        predicted_decomp_scores_output_fs.write(
                            "Ranking Method,Best Decomp Score,Test RMSE, Training RMSE,p val" + "\n")

                with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "a+") as predicted_decomp_scores_output_fs:
                    # read in features into dataframe
                    features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
                    # read in collated data, which contains the decomp value
                    true_instance_df = pd.read_csv(features_collated_folder + "/collated.csv")
                    scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    if ranking_method in ML_names:
                        predicted_decomp_score_column_name = 'ML predicted val'
                        #there are 4 different trained algorithms - same problem, all problems, different problem
                        for data_trained_on in data_trained_on_list:
                            # use model trained on same problem data
                            if data_trained_on == problem_type:
                                predicted_decomp_scores_df = pd.read_csv(scores_folder + "/" + ranking_method + "_" + problem_type + "_" + instance_name + ".csv")
                                training_rmse = getTrainingRMSE(data_trained_on,instance_name,ranking_method,TrainingTypeEnum.SAME)
                            # use model trained on all problem data
                            elif data_trained_on == "all_problem_types":
                                predicted_decomp_scores_df = pd.read_csv(
                                    scores_folder + "/" + ranking_method + "_" + "all_problem_types" + "_" + instance_name + ".csv")
                                training_rmse = getTrainingRMSE(data_trained_on, instance_name, ranking_method,
                                                                TrainingTypeEnum.ALL)
                            # use model trained on different problem types
                            else:
                                predicted_decomp_scores_df = pd.read_csv(
                                    scores_folder + "/" + ranking_method + "_" + data_trained_on + ".csv")
                                training_rmse = getTrainingRMSE(data_trained_on, instance_name, ranking_method,
                                                                TrainingTypeEnum.DIFFERENT)
                                # get the decomp indexes of the best n decompositions as predicted by the ML model

                            best_predicted_decomp_indexes = predicted_decomp_scores_df.nsmallest(8, 'ML predicted val')[
                                'Decomposition Index']
                            
                            best_score_scaled, p_val = getResultsFromIndexes(true_instance_df,best_predicted_decomp_indexes)
                            test_rmse = getTestRMSE(true_instance_df["Decomp Score"],
                                                        predicted_decomp_scores_df[predicted_decomp_score_column_name])
                            predicted_decomp_scores_output_fs.write(
                                "{}_{},{},{},{},{}\n".format(ranking_method, data_trained_on, best_score_scaled, test_rmse, training_rmse,
                                                          p_val))

                    #heuristic measures
                    else:
                        predicted_decomp_score_column_name = ranking_method + " Scores"
                        predicted_decomp_scores_df = pd.read_csv(
                            scores_folder + "/" + ranking_method + "_Scores" + ".csv")
                        # get the decomp indexes of the best n decompisitions as predicted by the ML model

                        if ranking_method == 'RBA' or ranking_method == "GCG1":
                            best_predicted_decomp_indexes = \
                            predicted_decomp_scores_df.nsmallest(8, ranking_method + " Scores")[
                                'Decomposition Index']
                        else:
                            best_predicted_decomp_indexes = \
                            predicted_decomp_scores_df.nlargest(8, ranking_method + " Scores")[
                                'Decomposition Index']

                        best_score_scaled, p_val = getResultsFromIndexes(true_instance_df,
                                                                         best_predicted_decomp_indexes)
                        test_rmse = getTestRMSE(true_instance_df["Decomp Score"],
                                                predicted_decomp_scores_df[predicted_decomp_score_column_name])
                        predicted_decomp_scores_output_fs.write(
                            "{},{},{},{},{}\n".format(ranking_method, best_score_scaled, test_rmse, "NA",
                                                      p_val))
                    print("Finished {}".format(instance_name))
                print("Finished {}".format(data_trained_on))
            print("Finished {}".format(problem_type))
        print("Finished {}".format(ranking_method))
    return

# return the best predicted decomp score (scaled) and the p-val associated with this prediction
def getResultsFromIndexes(true_instance_df,best_predicted_decomp_indexes):

    best_predicted_decomp_scores = []
    true_min_decomp_score = true_instance_df["Decomp Score"].min()
    true_max_decomp_score = true_instance_df["Decomp Score"].max()
    for decomp_idx in best_predicted_decomp_indexes:
        best_predicted_decomp_scores.append(
            true_instance_df[true_instance_df['Decomposition Index'] == decomp_idx]["Decomp Score"].values[0])
    # calculate best decomp score amongst the top 8 predicted decomps
    best_predicted_decomp_score = min(best_predicted_decomp_scores)
    # scale the score using true min and max decomp values
    best_score_scaled = ((best_predicted_decomp_score - true_min_decomp_score) / (
            true_max_decomp_score - true_min_decomp_score))

    # p-values for predicted decomp score. Is this score considered statistically significant
    z_score = ((best_predicted_decomp_score - true_instance_df["Decomp Score"].mean()) / true_instance_df[
        "Decomp Score"].std())
    p_val = scipy.stats.norm.cdf(z_score)

    return best_score_scaled, p_val

# calculate test rmse using predicted decomp scores vs and actual decomp scores
def getTestRMSE(true_decomp_scores, predicted_decomp_scores):
    test_rmse = math.sqrt(mean_squared_error(true_decomp_scores,
                                            predicted_decomp_scores))
    return test_rmse

# get the training RMSE scores from the input path for the given problem type, instance name and model name
def getTrainingRMSE(data_trained_on, instance_name, model_name, training_type):

    if training_type == TrainingTypeEnum.SAME:
        training_rmse_input_path = training_rmse_folder + "/" + "RMSE_training_scores_problem_types_excl_test.csv"
    elif training_type == TrainingTypeEnum.DIFFERENT:
        training_rmse_input_path = training_rmse_folder + "/" + "RMSE_training_scores_" + data_trained_on + ".csv"
    elif training_type == TrainingTypeEnum.ALL:
        training_rmse_input_path = training_rmse_folder + "/" + "RMSE_training_scores_all_problem_types.csv"

    training_rmse_score = -9999999999
    with open(training_rmse_input_path, "r") as training_rmse_score_input_fs:

        csvreader = csv.reader(training_rmse_score_input_fs, delimiter=",")
        for line_number, line_split in enumerate(csvreader):
            if line_number > 0:
                # training scores are in the format of problem_type, instance_name, Ranking Method, Training RMSE score
                if training_type == TrainingTypeEnum.SAME:
                    if line_split[0] == data_trained_on and line_split[1] == instance_name and line_split[2] == model_name:
                        training_rmse_score = float(line_split[3])
                # training scores are in the format of problem_type, Ranking Method, Training RMSE score
                elif training_type == TrainingTypeEnum.DIFFERENT:
                    if line_split[0] == data_trained_on and line_split[1] == model_name:
                        training_rmse_score = float(line_split[2])
                # training scores are in the format of problem_type, Ranking Method, Training RMSE score
                elif training_type == TrainingTypeEnum.ALL:
                    if line_split[1] == instance_name and line_split[2] == model_name:
                        training_rmse_score = float(line_split[3])
    return training_rmse_score
#how many predictions are statistically significant...
def main():
    getBestPredictedDecomps()

if __name__ == "__main__":

    main()