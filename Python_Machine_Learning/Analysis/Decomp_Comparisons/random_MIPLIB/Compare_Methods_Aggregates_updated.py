
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
from scipy.stats import friedmanchisquare
#global vars
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]

ML_models = ['OLM', 'SVM', 'SGD', 'KNN', 'RF', 'MLP', 'Stacking', 'Voting']
heuristic_methods = ["GCG1", "Goodness", "MW", "RBA"]


processed_results_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
model_comparisons_outputs_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons"

# p_val_threshold = 0.01


#this function calculates the decomp scores by problem type for either data trained on the same problemy type or all problem types, depending on the same_all_switch
# parameter. This function also presents the best predicted heuristic scores
def calculateSameAllProblemAggregates(same_all_switch):
    #dictionary to store dictionaries of ML comparisons for each problem type

    problem_aggregate_df_list = []
    #best prediction for ranking method across all test instances
    rm_best_all = dict()
    df_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        # best decomp scores for ranking types by problem type
        rm_best_pt = dict()
        ranking_method_test_RMSE_scores_dict = dict()
        ranking_method_train_RMSE_scores_dict = dict()
        ranking_method_p_vals_dict = dict()
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

            ranking_method_list = []
            decomp_scores_list = []
            # need to create a dataframe for each instance with all scores for each instance for each method
            # Ranking Method
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        # look at either same results or all results in batch file results
                        if identifier in ranking_method or ranking_method in heuristic_methods:
                            ranking_method_list.append(ranking_method.split('_')[0])
                            decomp_scores_list.append(float(line_split[1]))
                            original_decomp_score = float(line_split[1])
                            test_rmse_score = float(line_split[2])
                            try:
                                train_rmse_score = float(line_split[3])
                            except:
                                train_rmse_score = line_split[3]
                            p_val = float(line_split[4])
                            # if ranking method has not been seen, create the key and then append the score
                            if not ranking_method in rm_best_pt:
                                rm_best_pt[ranking_method] = []
                                # ranking_method_test_RMSE_scores_dict[ranking_method] = []
                                # ranking_method_train_RMSE_scores_dict[ranking_method] = []
                                ranking_method_p_vals_dict[ranking_method] = []
                            if not ranking_method in rm_best_all:
                                rm_best_all[ranking_method] = []

                            rm_best_all[ranking_method].append(original_decomp_score)
                            rm_best_pt[ranking_method].append(original_decomp_score)
                            # ranking_method_test_RMSE_scores_dict[ranking_method].append(test_rmse_score)
                            # ranking_method_train_RMSE_scores_dict[ranking_method].append(train_rmse_score)
                            # ranking_method_p_vals_dict[ranking_method].append(1 if p_val < p_val_threshold else 0)
            d = {"Ranking Method" : ranking_method_list, instance_name : decomp_scores_list}
            df = pd.DataFrame(data=d)
            # print(df)
            df_list.append(df)

    # print(df_list[0])
    # print(df_list[1])
    # result = pd.merge(df_list[0], df_list[1], on="Ranking Method")
    # print(result)
    df_merged = functools.reduce(lambda left, right: pd.merge(left, right, on="Ranking Method",
                                                                           how='inner'), df_list)
    df_merged.to_csv("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/prediction_results" + "_" + problem_type + ".csv")
    # #
    print(df_merged)
    #merge the dataframes together to get the whole data..
        # get list of ranking methods as keys in dictionaries. Remove _${Training_Type} from ranking method name if it is a ML model
        # ranking_methods = [ranking_method.split('_')[0] for ranking_method in rm_best_pt]
    # ranking_methods_original = [ranking_method for ranking_method in rm_best_all]
    #     #8 methods in total...
    # print(ranking_methods_original)
    # print(rm_best_all[ranking_methods_original[0]])
    # print(rm_best_all[ranking_methods_original[8]])
    # stat, p = friedmanchisquare(rm_best_all[ranking_methods_original[0]], rm_best_all[ranking_methods_original[1]],
    #                             rm_best_all[ranking_methods_original[2]], rm_best_all[ranking_methods_original[3]],
    #                             rm_best_all[ranking_methods_original[4]], rm_best_all[ranking_methods_original[5]],
    #                             rm_best_all[ranking_methods_original[6]], rm_best_all[ranking_methods_original[7]],
    #                             rm_best_all[ranking_methods_original[8]], rm_best_all[ranking_methods_original[9]],
    #                             rm_best_all[ranking_methods_original[10]], rm_best_all[ranking_methods_original[11]])

    # stat, p = friedmanchisquare(rm_best_all[ranking_methods_original[0]], rm_best_all[ranking_methods_original[1]],
    #                             rm_best_all[ranking_methods_original[2]], rm_best_all[ranking_methods_original[3]],
    #                             rm_best_all[ranking_methods_original[4]], rm_best_all[ranking_methods_original[5]],
    #                             rm_best_all[ranking_methods_original[6]], rm_best_all[ranking_methods_original[7]])

    # print(ranking_methods_original[7], ranking_methods_original[8], ranking_methods_original[9], ranking_methods_original[10], ranking_methods_original[11])
    # stat, p = friedmanchisquare(rm_best_all[ranking_methods_original[7]], rm_best_all[ranking_methods_original[8]],
    #                             rm_best_all[ranking_methods_original[9]], rm_best_all[ranking_methods_original[10]],
    #                             rm_best_all[ranking_methods_original[11]])

                                    # , rm_best_all[ranking_methods_original[4]],
                                    # rm_best_all[ranking_methods_original[5]],
                                    # rm_best_all[ranking_methods_original[6]], rm_best_all[ranking_methods_original[7]])
    # print('F=%.3f, p=%.3f' % (stat, p))
    # # interpret
    # alpha = 0.1
    # if p > alpha:
    #     print('Same distributions (fail to reject H0)')
    # else:
    #     print('Different distributions (reject H0)')
    #     mean_decomp_scores = [statistics.mean(rm_best_pt[ranking_method]) for ranking_method in rm_best_pt]
    #     # print("For problem {}".format(problem_type))
    #     # print("Scores are {}".format([rm_best_pt[ranking_method] for ranking_method in rm_best_pt]))
    #     stdev_decomp_scores = [statistics.stdev(rm_best_pt[ranking_method]) for ranking_method in rm_best_pt]
    #     test_rmse_scores = [statistics.mean(ranking_method_test_RMSE_scores_dict[ranking_method]) for ranking_method in ranking_method_test_RMSE_scores_dict]
    #     train_rmse_scores = [statistics.mean(ranking_method_train_RMSE_scores_dict[ranking_method]) if ranking_method not in heuristic_methods else "NA" for ranking_method in
    #                         ranking_method_test_RMSE_scores_dict]
    #     p_vals = [statistics.mean(ranking_method_p_vals_dict[ranking_method]) for ranking_method in
    #                    ranking_method_p_vals_dict]
    #     df = pd.DataFrame(zip(ranking_methods,mean_decomp_scores,stdev_decomp_scores, train_rmse_scores, test_rmse_scores, p_vals), columns=['Ranking Method', '\\bar{Score}', '\\bar{\sigma}','\\bar{Train RMSE}','\\bar{Test RMSE}','\\bar{p vals}'])
    #     problem_aggregate_df_list.append(df)
    # 
    # 
    # df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=['Ranking Method'],
    #                                                                        how='outer'), problem_aggregate_df_list)
    # 
    # # rename GCG1 to GCG OS
    # df_merged_features['Ranking Method'] = df_merged_features['Ranking Method'].replace(['GCG1'], 'GCG OS')
    # # rename SGD to OLS + L2
    # df_merged_features['Ranking Method'] = df_merged_features['Ranking Method'].replace(['SGD'], 'OLS + L2')
    # # rename SVM to SVR
    # df_merged_features['Ranking Method'] = df_merged_features['Ranking Method'].replace(['SVM'], 'SVR')
    # # rename Ranking Method column to Ranking Method
    # df_merged_features.rename(columns={"\\bar{Score}_x" : "\\bar{Score}", "\\bar{Score}_y" : "\\bar{Score}",
    # "\\bar{\sigma}_x" : "\\bar{\sigma}", "\\bar{\sigma}_y" : "\\bar{\sigma}",
    # "\\bar{Train RMSE}_x" : "\\bar{Train RMSE}", "\\bar{Train RMSE}_y" : "\\bar{Train RMSE}",
    # "\\bar{Test RMSE}_x" : "\\bar{Test RMSE}", "\\bar{Test RMSE}_x" : "\\bar{Test RMSE}",
    # "\\bar{p vals}_x" : "\\bar{p vals}", "\\bar{p vals}_y" : "\\bar{p vals}"}, inplace=True)
    # df_merged_features.to_csv(model_comparisons_outputs_root_folder + "/" + same_all_switch + "_problem_aggregates.csv")
    return

def main():

    same_all_switch = "ALL"
    calculateSameAllProblemAggregates(same_all_switch)
    # calculateProblemTypeTrainingAggregates()

#store the important features in a list
if __name__ == "__main__":
    main()

