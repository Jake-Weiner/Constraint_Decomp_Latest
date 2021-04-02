
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

def compareSameProblemMLModels():
    for problem_type_idx, problem_type in enumerate(problem_types):
        model_aggregates_outputs_folder = model_comparisons_outputs_root_folder + "/" + problem_type
        problem_specific_ranking_method_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "batch_results.csv"
            df = pd.read_csv(model_comparisons_path)
            best_decomp_score_min = df["Best Decomp Score"].min()
            best_decomp_score_max = df["Best Decomp Score"].max()
            #open file and normalise each score
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        original_decomp_score = float(line_split[1])
                        new_decomp_score = (original_decomp_score - best_decomp_score_min) / (best_decomp_score_max - best_decomp_score_min)
                        # if ranking method has not been seen, create the key and then append the score
                        if not ranking_method in problem_specific_ranking_method_dict:
                            problem_specific_ranking_method_dict[ranking_method] = []
                        if not ranking_method in all_instances_ranking_method_dict:
                            all_instances_ranking_method_dict[ranking_method] = []
                        problem_specific_ranking_method_dict[ranking_method].append(new_decomp_score)
                        all_instances_ranking_method_dict[ranking_method].append(new_decomp_score)

        with open(model_aggregates_outputs_folder + "/" + "decomp_score_aggregates.csv", "w") as problem_specific_model_aggregates_output_fs:
            problem_specific_model_aggregates_output_fs.write("Ranking Methods,Mean Best Decomp, Stddev Best Decomp \n")
            for ranking_method in problem_specific_ranking_method_dict:
                best_decomp_mean = statistics.mean(problem_specific_ranking_method_dict[ranking_method])
                best_decomp_stddev = statistics.stdev(problem_specific_ranking_method_dict[ranking_method])
                problem_specific_model_aggregates_output_fs.write("{},{},{} \n".format(ranking_method, best_decomp_mean,best_decomp_stddev))

def calculateProblemAggregates():

    all_instances_ranking_method_dict = dict()
    for problem_type_idx, problem_type in enumerate(problem_types):
        model_aggregates_outputs_folder = model_comparisons_outputs_root_folder + "/" + problem_type
        problem_specific_ranking_method_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "batch_results.csv"
            df = pd.read_csv(model_comparisons_path)

            best_decomp_score_min = df["Best Decomp Score"].min()
            best_decomp_score_max = df["Best Decomp Score"].max()
            #open file and normalise each score
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        original_decomp_score = float(line_split[1])
                        new_decomp_score = (original_decomp_score - best_decomp_score_min) / (best_decomp_score_max - best_decomp_score_min)
                        # if ranking method has not been seen, create the key and then append the score
                        if not ranking_method in problem_specific_ranking_method_dict:
                            problem_specific_ranking_method_dict[ranking_method] = []
                        if not ranking_method in all_instances_ranking_method_dict:
                            all_instances_ranking_method_dict[ranking_method] = []
                        problem_specific_ranking_method_dict[ranking_method].append(new_decomp_score)
                        all_instances_ranking_method_dict[ranking_method].append(new_decomp_score)

        with open(model_aggregates_outputs_folder + "/" + "decomp_score_aggregates.csv", "w") as problem_specific_model_aggregates_output_fs:
            problem_specific_model_aggregates_output_fs.write("Ranking Methods,Mean Best Decomp, Stddev Best Decomp \n")
            for ranking_method in problem_specific_ranking_method_dict:
                best_decomp_mean = statistics.mean(problem_specific_ranking_method_dict[ranking_method])
                best_decomp_stddev = statistics.stdev(problem_specific_ranking_method_dict[ranking_method])
                problem_specific_model_aggregates_output_fs.write("{},{},{} \n".format(ranking_method, best_decomp_mean,best_decomp_stddev))

    with open(model_comparisons_outputs_root_folder + "/" + "decomp_score_aggregates.csv",
              "w") as per_problem_type_model_aggregates_output_fs:
        per_problem_type_model_aggregates_output_fs.write("Ranking Methods,Mean Best Decomp, Stddev Best Decomp \n")
        for ranking_method in all_instances_ranking_method_dict:
            best_decomp_mean = statistics.mean(all_instances_ranking_method_dict[ranking_method])
            best_decomp_stddev = statistics.stdev(all_instances_ranking_method_dict[ranking_method])
            per_problem_type_model_aggregates_output_fs.write(
                "{},{},{} \n".format(ranking_method, best_decomp_mean, best_decomp_stddev))
    return

#for each problem, output the aggregates for each ML model for SAME training, DIFFERENT training and "ALL" Training
def calculateProblemTypeTrainingAggregates():
    problem_aggregate_df_list = []
    # dictionary has   ML_SAME : (scores) ML_DIFFERENT: (scores), ML_ALL (scores)
    data_trained_on_categories = ["SAME", "DIFFERENT"]
    # problem_ranking dict stores all scores for every instance across all problem types
    problem_ranking_method_dict = dict()
    df_problem_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        model_aggregates_outputs_folder = model_comparisons_outputs_root_folder + "/" + problem_type
        # problem_specific dict stores all scores each problem type and then writes the results to file
        problem_specific_ranking_method_dict = dict()
        same_data_type_trained_on_dict = dict()
        different_data_type_trained_on_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "batch_results.csv"
            df = pd.read_csv(model_comparisons_path)
            # filter out heuristic methods scores
            #if normalisation is required
            df_filtered = df.copy()
            # filter df only on ML scores, not heuristic scores
            df_filtered = df_filtered[~df_filtered["Ranking Method"].isin(heuristic_methods)]
            best_decomp_score_min = df_filtered["Best Decomp Score"].min()
            best_decomp_score_max = df_filtered["Best Decomp Score"].max()
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        original_decomp_score = float(line_split[1])
                        # new_decomp_score = (original_decomp_score - best_decomp_score_min) / (
                        #         best_decomp_score_max - best_decomp_score_min)
                        ML_model_name = ranking_method.split("_")[0]
                        # key for dicts used is ML model name + data type trained on (SAME / DIFFERENT)
                        key = ""
                        #model is trained on same problem data
                        if problem_type in ranking_method:
                            key = ML_model_name + "_" + "SAME"
                            if not ML_model_name in same_data_type_trained_on_dict:
                                same_data_type_trained_on_dict[ML_model_name] = []
                            same_data_type_trained_on_dict[ML_model_name].append(original_decomp_score)
                        #model is trained on all problem types
                        # elif "all_problem_types" in ranking_method:
                        #     key = ML_model_name + "_" + "ALL"
                        #model is trianed on different problem types and is not a heuristic measure
                        elif ranking_method not in heuristic_methods and "all_problem_types" not in ranking_method:
                            key = ML_model_name + "_" + "DIFFERENT"
                            if not ML_model_name in different_data_type_trained_on_dict:
                                different_data_type_trained_on_dict[ML_model_name] = []
                            different_data_type_trained_on_dict[ML_model_name].append(original_decomp_score)
                            # if ranking method has not been seen, create the key and then append the score
                        # if not key in problem_ranking_method_dict:
                        #     problem_ranking_method_dict[key] = []
                        # if not key in problem_specific_ranking_method_dict:
                        #     problem_specific_ranking_method_dict[key] = []
                        #
                        # problem_ranking_method_dict[key].append(original_decomp_score)
                        # problem_specific_ranking_method_dict[key].append(original_decomp_score)
        same_problem_type_means = [statistics.mean(same_data_type_trained_on_dict[ML_model_name]) for ML_model_name in ML_models]
        different_problem_type_means = [statistics.mean(different_data_type_trained_on_dict[ML_model_name]) for
                                   ML_model_name in ML_models]

        data = {"ML Model" : ML_models, "SAME" + " " + problem_type : same_problem_type_means, "DIFFERENT" + " " + problem_type: different_problem_type_means}
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
    # #remove left_right tages from mergre
    # for column in df_merged_features.columns:
    #     # df_merged_features.rename(column, column.split("_")[0])
    #     df_merged_features.rename({column : column.split("_")[0]}, axis = 1, inplace=True)

    # mux = pd.MultiIndex.from_product([problem_types, ['SAME', 'DIFFERENT']])
    # new_columns = ['ML Model'].append(mux)
    # df_merged_features.columns = new_columns
    print(df_merged_features)
        # print(same_problem_type_means)
        # print(different_problem_type_means)
        # instance_counts = [
        #     len(problem_specific_ranking_method_dict[ML_model_name + "_" + data_trained_on_category]) for
        #     data_trained_on_category in
        #     data_trained_on_categories for ML_model_name in ML_models]
        # mean_decomp_scores = [
        #     statistics.mean(problem_specific_ranking_method_dict[ML_model_name + "_" + data_trained_on_category]) for
        #     data_trained_on_category in
        #     data_trained_on_categories for ML_model_name in ML_models]
        #
        # stddev_decomp_scores = [
        #     statistics.stdev(problem_specific_ranking_method_dict[ML_model_name + "_" + data_trained_on_category])
        #     for data_trained_on_category in
        #     data_trained_on_categories for ML_model_name in ML_models]
        # if problem_type == 'fixed_cost_network_flow':
        #     print(instance_counts)
        #     print(mean_decomp_scores)
        #     print(stddev_decomp_scores)


    # mean_decomp_scores = [[statistics.mean(problem_ranking_method_dict[ML_model_name + "_" + data_trained_on_category]), ML_model_name, data_trained_on_category] for ML_model_name in ML_models for data_trained_on_category in
    #                       data_trained_on_categories]
    #
    # # print(mean_decomp_scores)
    #
    # stddev_decomp_scores = [statistics.stdev(problem_ranking_method_dict[ML_model_name + "_" + data_trained_on_category])
    #                                          for ML_model_name in ML_models for data_trained_on_category in
    #                       data_trained_on_categories]
    #
    #
    # same_mean_best_decomp_scores = [pair[1] for pair in enumerate(mean_decomp_scores) if pair[0] % 2 == 0]
    # different_mean_best_decomp_scores = [pair[1] for pair in enumerate(mean_decomp_scores) if pair[0] % 2 == 1]
    # # all_mean_best_decomp_scores = [pair[1] for pair in enumerate(mean_decomp_scores) if pair[0] % 3 == 2]
    #
    #
    # print(same_mean_best_decomp_scores)
    # print(different_mean_best_decomp_scores)
    # # print(all_mean_best_decomp_scores)
    # df = pd.DataFrame(mean_decomp_scores, columns=['Mean Best Decomp Score', 'ML Model', 'Training Type'])
    #
    # # df = pd.DataFrame(zip(ML_models,same_mean_best_decomp_scores,different_mean_best_decomp_scores, all_mean_best_decomp_scores), columns=['ML Method', 'Mean Best Decomp (SAME)', 'Mean Best Decomp (DIFFERENT)', 'Mean Best Decomp (ALL)'])
    # df.to_csv(model_comparisons_outputs_root_folder + "/" + "different_training_types.csv")
    # print(mean_decomp_scores)
    # print(stddev_decomp_scores)

    # stdev_decomp_scores = [statistics.stdev(problem_specific_ranking_method_dict[ranking_method]) for ranking_method in
    #                        problem_specific_ranking_method_dict]
    # df = pd.DataFrame(zip(ranking_methods, mean_decomp_scores, stdev_decomp_scores),
    #                   columns=['ML Method', 'Mean Best Decomp(' + problem_type + ")",
    #                            'Stddev Best Decomp(' + problem_type + ")"])

#this function calculates the decomp scores by problem type for either data trained on the same problemy type or all problem types, depending on the same_all_switch
# parameter. This function also presents the best predicted heuristic scores
def calculateSameAllProblemAggregates(same_all_switch):
    #dictionary to store dictionaries of ML comparisons for each problem type

    problem_aggregate_df_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):

        model_aggregates_outputs_folder = model_comparisons_outputs_root_folder + "/" + problem_type
        problem_specific_ranking_method_dict = dict()
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            # if "g200x740" not in instance_name and "h806320d.mps" not in instance_name and "ta1" not in instance_name and "ta2" not in instance_name:
            model_comparisons_path = model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" "batch_results.csv"
            df = pd.read_csv(model_comparisons_path)
            identifier = ""
            if same_all_switch == "ALL":
                identifier = "all_problem_types"
            elif same_all_switch == "SAME":
                identifier = problem_type
            best_decomp_score_min = df["Best Decomp Score"].min()
            best_decomp_score_max = df["Best Decomp Score"].max()
            #open file and normalise each score
            with open(model_comparisons_path, "r") as model_comparison_input_fs:
                model_comparisons_csv_reader = csv.reader(model_comparison_input_fs, delimiter=",")
                for line_number, line_split in enumerate(model_comparisons_csv_reader):
                    if line_number != 0:
                        ranking_method = line_split[0]
                        # look at either same results or all results in batch file results
                        if identifier in ranking_method or ranking_method in heuristic_methods:
                            # scaled_score = scaler.transform(float(line_split[1]))
                            original_decomp_score = float(line_split[1])
                            scaled_score = (original_decomp_score - best_decomp_score_min) / (best_decomp_score_max - best_decomp_score_min)
                            # if ranking method has not been seen, create the key and then append the score
                            if not ranking_method in problem_specific_ranking_method_dict:
                                problem_specific_ranking_method_dict[ranking_method] = []
                            problem_specific_ranking_method_dict[ranking_method].append(scaled_score)
        ranking_methods = [ranking_method.split('_')[0] for ranking_method in problem_specific_ranking_method_dict]
        mean_decomp_scores = [statistics.mean(problem_specific_ranking_method_dict[ranking_method]) for ranking_method in problem_specific_ranking_method_dict]
        stdev_decomp_scores = [statistics.stdev(problem_specific_ranking_method_dict[ranking_method]) for ranking_method in problem_specific_ranking_method_dict]
        df = pd.DataFrame(zip(ranking_methods,mean_decomp_scores,stdev_decomp_scores), columns=['ML Method', 'Mean Best Decomp(' + problem_type  +")", 'Stddev Best Decomp(' + problem_type +")"])
        problem_aggregate_df_list.append(df)
        print(df.iloc[:,0:2])

    df_merged_features = functools.reduce(lambda left, right: pd.merge(left, right, on=['ML Method'],
                                                                           how='outer'), problem_aggregate_df_list)

    df_merged_features.to_csv(model_comparisons_outputs_root_folder + "/" + same_all_switch + "_problem_aggregates.csv")

    return

def calculateAllInstanceAggregates():
    return

def main():
    # same_all_switch = "ALL"
    # calculateSameAllProblemAggregates(same_all_switch)
    calculateProblemTypeTrainingAggregates()

#store the important features in a list
if __name__ == "__main__":
    main()

