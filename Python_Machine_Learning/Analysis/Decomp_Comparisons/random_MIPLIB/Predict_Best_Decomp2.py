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

problem_types = ["random_MIPLIB"]
instance_names = [
    ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
     "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

#need to fix last row error
# , "splice1k1.mps"]]
features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Machine_Learning/Processed_Results_Old/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
training_rmse_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/RMSE_training_scores"

# prepare models
ML_names = ['Voting']
heuristic_names = ["GCG1", "Goodness", "MW", "RBA"]
data_trained_on_list = ["all_network_instances"]

features_calculated_output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
model_comparisons_outputs_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons"



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
            for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
                best_decomp_score_folder = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name
                Path(best_decomp_score_folder).mkdir(parents=True, exist_ok=True)
                # test if output file exists
                my_file = Path(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv")
                # write headers if file does not exist
                if not my_file.is_file():
                    with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "w") as predicted_decomp_scores_output_fs:
                        predicted_decomp_scores_output_fs.write(
                            "Ranking Method,Best Decomp Score,Test RMSE,p val" + "\n")

                with open(best_decomp_score_folder + "/" + "predicted_decomp_scores.csv", "a+") as predicted_decomp_scores_output_fs:
                    # read in features into dataframe
                    features_collated_folder = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated"
                    # read in collated data, which contains the decomp scores
                    true_instance_df = pd.read_csv(features_collated_folder + "/collated.csv")
                    scores_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    #ML measures
                    if ranking_method in ML_names:
                        predicted_decomp_score_column_name = 'ML predicted val'
                        #there are 4 different trained algorithms - same problem, all problems, different problem
                        for data_trained_on in data_trained_on_list:
                            # use model trained on same problem data
                            predicted_decomp_scores_df = pd.read_csv(scores_folder + "/" + ranking_method + "_" + data_trained_on + ".csv")
                            # get the decomp indexes of the best n decompositions as predicted by the ML model
                            best_predicted_decomp_indexes = predicted_decomp_scores_df.nsmallest(8, 'ML predicted val')[
                                'Decomposition Index']
                            
                            best_score_scaled, p_val = getResultsFromIndexes(true_instance_df,best_predicted_decomp_indexes)
                            test_rmse = getTestRMSE(true_instance_df["Decomp Score"],
                                                        predicted_decomp_scores_df[predicted_decomp_score_column_name])
                            predicted_decomp_scores_output_fs.write(
                                "{}_{},{},{},{}\n".format(ranking_method, data_trained_on, best_score_scaled, test_rmse,
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
                            "{},{},{},{}\n".format(ranking_method, best_score_scaled, test_rmse,
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

#
#
# # alpha to critical
# alpha = 0.05
# n_sided = 2 # 2-sided test
# z_crit = stats.norm.ppf(1-alpha/n_sided)
# print(z_crit) # 1.959963984540054
#
# # critical to alpha
# alpha = stats.norm.sf(z_crit) * n_sided
# print(alpha) # 0.05

# calculate test rmse using predicted decomp scores vs and actual decomp scores
def getTestRMSE(true_decomp_scores, predicted_decomp_scores):
    test_rmse = math.sqrt(mean_squared_error(true_decomp_scores,
                                            predicted_decomp_scores))
    return test_rmse


#how many predictions are statistically significant...
def main():
    getBestPredictedDecomps()

if __name__ == "__main__":

    main()