
#for each problem type (granularity)

#for all instances - ave best,

#normalise the best decomps predicted, to then aggregate it fairly across different instances
#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import functools
import csv
import statistics
from sklearn.preprocessing import MinMaxScaler
#global vars
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
# problem_types = ["network_design", "fixed_cost_network_flow"]

instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]

ML_models = ['OLM', 'SVM', 'SGD', 'KNN', 'RF', 'MLP', 'Stacking', 'Voting']
heuristic_methods = ["GCG1", "Goodness", "MW", "RBA"]

model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

model_comparisons_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"

#for each problem, output the aggregates for each ML model for SAME and DIFFERENT training
def calculateProblemTypeTrainingAggregates():
    df_problem_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        same_data_type_trained_on_dict = dict()
        different_data_type_trained_on_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "predicted_decomp_scores.csv"
            df = pd.read_csv(model_comparisons_path)
            # filter out heuristic methods scores

            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        original_decomp_score = float(line_split[1])
                        ML_model_name = ranking_method.split("_")[0]
                        # key for dicts used is ML model name + data type trained on (SAME / DIFFERENT)
                        key = ""
                        #model is trained on same problem data
                        if problem_type in ranking_method:
                            key = ML_model_name + "_" + problem_type
                            if not key in same_data_type_trained_on_dict:
                                same_data_type_trained_on_dict[key] = []
                            same_data_type_trained_on_dict[key].append(original_decomp_score)
                        #model is trianed on different problem types and is not a heuristic measure
                        elif ranking_method not in heuristic_methods and "all_problem_types" not in ranking_method and problem_type not in ranking_method:
                            #get the name of the different problem type
                            different_problem_type = "_".join(ranking_method.split("_")[1:])
                            key = ML_model_name + "_" + different_problem_type
                            if not key in different_data_type_trained_on_dict:
                                different_data_type_trained_on_dict[key] = []
                            different_data_type_trained_on_dict[key].append(original_decomp_score)

        # get means and stddevs for best decomp scores as predicted by ML models trained on same problem type
        same_problem_type_means = [statistics.mean(same_data_type_trained_on_dict[ML_model_name + "_" + problem_type]) for ML_model_name in ML_models]
        #get different problem types
        different_problem_types = problem_types.copy()
        #remove the current problem type, leaving the remaining DIFFERENT problem typs
        different_problem_types.remove(problem_type)
        #get means for best decomp scores as predicted by ML models trained on different problem types
        different_problem_type_means_one = [statistics.mean(different_data_type_trained_on_dict[ML_model_name + "_" + different_problem_types[0]]) for ML_model_name in ML_models]
        different_problem_type_means_two = [
            statistics.mean(different_data_type_trained_on_dict[ML_model_name + "_" + different_problem_types[1]]) for
            ML_model_name in ML_models]
        data = {"ML Model" : ML_models, "SAME" + " " + problem_type : same_problem_type_means, "DIFFERENT" + " " + different_problem_types[0]: different_problem_type_means_one
                , "DIFFERENT" + " " + different_problem_types[1]: different_problem_type_means_two}
        # df_problem_type = pd.DataFrame(same_problem_type_means,different_problem_type_means, columns=["SAME", "DIFFERENT"])
        df_problem_type = pd.DataFrame(data)
        df_problem_type.name = problem_type
        print(df_problem_type)
        df_problem_list.append(df_problem_type)

    #merge the dataframes
    #reduce applies the lambda function,. which is pd.merge, to all of the list elements - df_problem_list
    df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=['ML Model'],
                                                                       how='outer'), df_problem_list)
    df_merged_features.to_csv(model_comparisons_outputs_root_folder + "/" + "different_training_types.csv")
    print(df_merged_features)


#this function calculates the decomp scores by problem type for either data trained on the same problemy type or all problem types, depending on the same_all_switch
# parameter. This function also presents the best predicted heuristic scores
def calculateSameAllProblemAggregates(same_all_switch):
    #dictionary to store dictionaries of ML comparisons for each problem type

    problem_aggregate_df_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):

        ranking_method_best_decomp_scores_dict = dict()
        ranking_method_RMSE_scores_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            # if "g200x740" not in instance_name and "h806320d.mps" not in instance_name and "ta1" not in instance_name and "ta2" not in instance_name:
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "predicted_decomp_scores.csv"
            df = pd.read_csv(model_comparisons_path)
            #identifier is used to get scores for ML models which are trained on either ALL problem types or the SAME problem type
            identifier = ""
            if same_all_switch == "ALL":
                identifier = "all_problem_types"
            elif same_all_switch == "SAME":
                identifier = problem_type
            #open file and normalise each score
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        # look at either same results or all results in batch file results
                        if identifier in ranking_method or ranking_method in heuristic_methods:
                            original_decomp_score = float(line_split[1])
                            rmse_score = float(line_split[2])
                            # if ranking method has not been seen, create the key and then append the score
                            if not ranking_method in ranking_method_best_decomp_scores_dict:
                                ranking_method_best_decomp_scores_dict[ranking_method] = []
                                ranking_method_RMSE_scores_dict[ranking_method] = []
                            ranking_method_best_decomp_scores_dict[ranking_method].append(original_decomp_score)
                            ranking_method_RMSE_scores_dict[ranking_method].append(rmse_score)
        ranking_methods = [ranking_method.split('_')[0] for ranking_method in ranking_method_best_decomp_scores_dict]
        mean_decomp_scores = [statistics.mean(ranking_method_best_decomp_scores_dict[ranking_method]) for ranking_method in ranking_method_best_decomp_scores_dict]
        stdev_decomp_scores = [statistics.stdev(ranking_method_best_decomp_scores_dict[ranking_method]) for ranking_method in ranking_method_best_decomp_scores_dict]
        rmse_scores = [statistics.mean(ranking_method_RMSE_scores_dict[ranking_method]) for ranking_method in ranking_method_RMSE_scores_dict]
        df = pd.DataFrame(zip(ranking_methods,mean_decomp_scores,stdev_decomp_scores, rmse_scores), columns=['ML Method', 'Mean Score', 'Score Std','Mean RMSE'])
        problem_aggregate_df_list.append(df)
        print(df.iloc[:,0:2])

    df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=['ML Method'],
                                                                           how='outer'), problem_aggregate_df_list)

    # rename GCG1 to GCG OS
    df_merged_features['ML Method'] = df_merged_features['ML Method'].replace(['GCG1'], 'GCG OS')
    # rename SGD to OLS + L2
    df_merged_features['ML Method'] = df_merged_features['ML Method'].replace(['SGD'], 'OLS + L2')
    # rename SVM to SVR
    df_merged_features['ML Method'] = df_merged_features['ML Method'].replace(['SVM'], 'SVR')
    # rename ML Method column to Ranking Method
    df_merged_features.rename(columns={"ML Method" : "Ranking Method", "Mean Score_x" : "Mean Score",'Score Std_x' : "Score Std",
                                       "Mean RMSE_x" : "Mean RMSE", "Mean Score_y" : "Mean Score","Score Std_y" : "Score Std",
                                       "Mean RMSE_y" : "Mean RMSE"}, inplace=True)



    df_merged_features.to_csv(model_comparisons_outputs_root_folder + "/" + same_all_switch + "_problem_aggregates.csv")
    return

def main():
    same_all_switch = "ALL"
    calculateSameAllProblemAggregates(same_all_switch)
    # calculateProblemTypeTrainingAggregates()

#store the important features in a list
if __name__ == "__main__":
    main()

