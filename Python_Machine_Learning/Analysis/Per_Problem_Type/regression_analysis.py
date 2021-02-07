#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from sklearn.model_selection import cross_validate
from sklearn.linear_model import LinearRegression
from sklearn import svm
from sklearn.linear_model import SGDRegressor
from sklearn.neighbors import KNeighborsRegressor
from sklearn import tree
from sklearn.neural_network import MLPRegressor
import os
import statistics
import pickle

def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    #read in dataset
    # network_design_important_features = {}
    # fixed_cost_network_flow_important_features = {}
    # supply_network_planning_important_features = {}

    #create sets for important features
    important_features_set = [set() for i in range(len(problem_types))]
    # print(important_features_set)

    # problem_type_chosen_idx = 0

    # problem_type = problem_types[problem_type_chosen_idx]

    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    regression_model_names = ["OLM", "SVM", "SGD", "KNN", "DT", "MLP"]

    regression_models_pickle_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"

    write_cfv_results = True
    cfv_results_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_CFV/problem_types"
    cfv_results = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        Path(processed_results_folder + "/Machine_Learning_Outputs" + "/" + "Problem_Types" + "/" + problem_type).mkdir(
            parents=True, exist_ok=True)
        df_list = []
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)
            # remove any rows with nan, inf or -inf vals

        df_combined = pd.concat(df_list, keys=instance_names[problem_type_idx])
        # print(df_combined.columns[df_combined.isnull().any()])

        # print(df_combined.head)
        #check to see if there are any
        #remove any rows in which there are nan, inf or -inf values
        # df_combined = df_combined[~df_combined.isin([np.nan, np.inf, -np.inf]).any(1)]

        # df_combined = df_combined[~df_combined.isin([np.inf, -np.inf]).any(1)]
        # print(df_combined.head)
        # #remove any values where the gap is less and 0%, a result of potential rounding errors from CPLEX
        # df_combined = df_combined[df_combined['Gap (%)'] > 0]
        #reset the indexes
        # df_combined.reset_index(drop=True, inplace=True)
        # print(df_combined.head)
        # print(df_combined)
        # remove default index and Decomp index from dataframe to store features
        X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', 'Gap (%)', 'LR Solve Time(s)'])
        # print(X.columns)
        # capture output columns
        Y = df_combined[['Gap (%)', 'LR Solve Time(s)']]
        # print(Y)

        # print(X.columns)
        # print(X)
        # print(Y)
        # Using Pearson Correlation
        # plt.figure(figsize=(12, 10))
        # cor = df_combined.corr()
        # # Correlation with output variable
        # cor_bound_target = abs(cor['Gap (%)'])
        # # print(cor_bound_target)
        # # Selecting highly correlated features
        # relevant_features_bound = cor_bound_target[cor_bound_target > 0.6]
        #
        # for feature in relevant_features_bound.index:
        #     important_features_set[problem_type_idx].add(feature)

        # convert features to np array
        X_np = X.to_numpy()
        Bound_np = Y['Gap (%)'].to_numpy()

        regression_models = []
        #OLM Regression
        OLM_reg = LinearRegression().fit(X_np, Bound_np)
        # print("OLM Regression model score is " + str(OLM_reg.score(X_np, Bound_np)))
        regression_models.append(OLM_reg)
        # # SVM_reg = svm.SVR().fit(X_np, Bound_np)
        # # # print("SVM Regression model score is " + str(SVM_reg.score(X_np, Bound_np)))
        # # regression_models.append(SVM_reg)
        # SGD_reg = SGDRegressor().fit(X_np, Bound_np)
        # # print("SGD Regression model score is " + str(SGD_reg.score(X_np, Bound_np)))
        # regression_models.append(SGD_reg)
        # KNN_reg = KNeighborsRegressor().fit(X_np, Bound_np)
        # # print("KNN Regression model score is " + str(KNN_reg.score(X_np, Bound_np)))
        # regression_models.append(KNN_reg)
        # DT_reg = tree.DecisionTreeRegressor().fit(X_np, Bound_np)
        # # print("DT Regression model score is " + str(DT_reg.score(X_np, Bound_np)))
        # regression_models.append(DT_reg)
        # # MLP_reg = MLPRegressor().fit(X_np, Bound_np)
        # # # print("MLP Regression model score is " + str(MLP_reg.score(X_np, Bound_np)))
        # # regression_models.append(MLP_reg)

        #store models with pickle
        with open(regression_models_pickle_output_folder + "/" + problem_type + ".pkl" , 'wb') as pickle_output_fs:
            pickle.dump(regression_models, pickle_output_fs)

    #     # use 10 fold cross validated
    #     problem_cfv_results = []
    #     for reg_idx, regression_model in enumerate(regression_models):
    #         cv_results = cross_validate(regression_model, X_np, Bound_np, cv=10, scoring=["max_error", "neg_mean_squared_error"])
    #         sorted(cv_results.keys())
    #         print("Crossfold validation scores for model " + regression_model_names[reg_idx] + " -")
    #         print(cv_results)
    #         # print(cv_results['test_max_error'])
    #         print(cv_results['test_neg_mean_squared_error'])
    #         mse_ave = statistics.mean(cv_results['test_neg_mean_squared_error'])
    #         mse_stddev = statistics.pstdev(cv_results['test_neg_mean_squared_error'])
    #         problem_cfv_results.append(str(mse_ave) + "," + str(mse_stddev) + ",")
    #
    #     cfv_results.append(problem_cfv_results)
    #
    # if write_cfv_results:
    #     if not os.path.exists(cfv_results_path):
    #         with open(cfv_results_path, "w") as output_fs:
    #             ["OLM", "SVM", "SGD", "KNN", "DT", "MLP"]
    #             output_fs.write(", OLM,,SVM,,SGD,,KNN,,DT,,MLP\n")
    #
    #     with open(cfv_results_path, "a+") as output_fs:
    #         for problem_type_idx, problem_type in enumerate(problem_types):
    #             output_fs.write(problem_type + ",")
    #             for val in cfv_results[problem_type_idx]:
    #                 output_fs.write(val)
    #             output_fs.write("\n")

    # A_only = important_features_set[0].difference(important_features_set[1] | important_features_set[2])
    # B_only = important_features_set[1].difference(important_features_set[0] | important_features_set[2])
    # C_only = important_features_set[2].difference(important_features_set[1] | important_features_set[0])
    # A_and_B_only = (important_features_set[0] & important_features_set[1]).difference(important_features_set[2])
    # A_and_C_only = (important_features_set[0] & important_features_set[2]).difference(
    #     important_features_set[1])
    # B_and_C_only = (important_features_set[1] & important_features_set[2]).difference(
    #     important_features_set[0])
    # A_and_B_and_C_only = important_features_set[0] & important_features_set[1] & important_features_set[2]
    #
    # print(A_only)
    # print(B_only)
    # print(C_only)
    # print(A_and_B_only)
    # print(A_and_C_only)
    # print(B_and_C_only)
    # print(A_and_B_and_C_only)

#store the important features in a list
if __name__ == "__main__":

    main()