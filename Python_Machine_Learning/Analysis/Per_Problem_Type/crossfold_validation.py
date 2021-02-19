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
import os
import statistics
import pickle

def main():

    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    #create sets for important features



    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    regression_model_names = ["OLM", "SVM", "SGD", "KNN", "DT", "MLP"]

    regression_models_pickle_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"
    write_cfv_results = True
    cfv_results_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_CFV"
    cfv_results = []

    # prepare models
    models = []
    models.append(('OLM', LinearRegression()))
    models.append(('SVM', svm.SVR()))
    models.append(('SGD', SGDRegressor()))
    models.append(('KNN', KNeighborsRegressor()))
    models.append(('DT', tree.DecisionTreeRegressor()))
    models.append(('MLP', MLPRegressor()))

    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)
            # remove any rows with nan, inf or -inf vals

        df_combined = pd.concat(df_list, keys=instance_names[problem_type_idx])
        #reset the indexes
        df_combined.reset_index(drop=True, inplace=True)

        # remove default index and Decomp index from dataframe to store features
        X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', "Normalised Gap (%)", 'LR Solve Time(s)'])
        # print(X.columns)
        # capture output columns
        Y = df_combined[["Normalised Gap (%)", 'LR Solve Time(s)']]


        # convert features to np array
        X_np = X.to_numpy()
        Bound_np = Y["Normalised Gap (%)"].to_numpy()
        # print(Bound_np)

        seed = 0
        # evaluate each model in turn
        boxplot_results = []
        summary_results = []
        names = []
        scoring_method = 'neg_mean_squared_error'

        for name, model in models:
            kfold = KFold(n_splits=10, random_state=seed)
            cv_results = cross_val_score(model, X_np, Bound_np, cv=kfold, scoring=scoring_method)
            boxplot_results.append(cv_results)
            summary_results.append([name, cv_results.mean(), cv_results.std()])
            names.append(name)
            msg = "%s: %f (%f)" % (name, cv_results.mean(), cv_results.std())
            print(msg)


        with open(cfv_results_path + "/" + problem_type + "_summary", "a+") as results_output_fs:
            for model_idx, model_result in enumerate(summary_results):
                results_output_fs.write(names[model_idx] + "," + str(model_result[1]) + "," + str(model_result[2]) + "\n")

        fig = plt.figure()
        fig.suptitle('Algorithm Comparison')
        ax = fig.add_subplot(111)
        plt.boxplot(boxplot_results)
        ax.set_xticklabels(names)
        plt.show()
        plt.savefig(cfv_results_path + "/" + problem_type + "_box_plot")

        # for problem_type_regression_idx, regression_model_list in enumerate(regression_models_list):
        #     problem_cfv_results = []
        #     for reg_idx, regression_model in enumerate(regression_model_list):
            # print(regression_model.coef_)

                # cv_results = cross_val_score(regression_model, X_np, Bound_np, cv=10, scoring="neg_mean_squared_error")
                # print("for problem type " + problem_type + " and for model trained on " + regression_model_trained_on[problem_type_regression_idx] + " prediction is ")
                # print(regression_model.predict(X_np)[0])
                # print("actual bound is " + str(Bound_np[0]))
                # print(cv_results)
                # sorted(cv_results.keys())

                # print("Crossfold validation scores for model " + regression_model_names[reg_idx] + " -")
                # # print(cv_results)
                # # print(cv_results['test_max_error'])
                # print(cv_results['test_neg_mean_squared_error'])

                # mse_ave = statistics.mean(cv_results['test_neg_mean_squared_error'])
                # print(mse_ave)
                # # mse_stddev = statistics.pstdev(cv_results['test_neg_mean_squared_error'])
                # # problem_cfv_results.append(str(mse_ave) + "," + str(mse_stddev) + ",")
                # # problem_cfv_results.append(str(mse_ave) + ",")

        # cfv_results.append(problem_cfv_results)

    #zip together the results from each of the 4 sets of regression models trained
    # zipped_results = zip(cfv_results[0], cfv_results[1], cfv_results[2], cfv_results[3])

    # # print(zipped_results)
    # if write_cfv_results:
    #     if not os.path.exists(cfv_results_path):
    #         with open(cfv_results_path, "w") as output_fs:
    #             output_fs.write(""", OLM_1,, OLM_2,, OLM_3,, OLM_all,,
    #                              SVM_1,, SVM_2,, SVM_3,, SVM_all,,
    #                              SGD_1,, SGD_2,, SGD_3,, SGD_all,,
    #                              KNN_1,, KNN_2,, KNN_3,, KNN_all,,
    #                              DT_1,, DT_2,, DT_3,, DT_all,,
    #                              MLP_1,, MLP_2,, MLP_3,, MLP_all\n""")
    #
    #     with open(cfv_results_path, "a+") as output_fs:
    #         output_fs.write("all instances" + ",")
    #         for zip_element in zipped_results:
    #             for reg_mse in zip_element:
    #                 output_fs.write(str(reg_mse) + ",")
    #         output_fs.write("\n")
            # # for problem_type_idx, problem_type in enumerate(problem_types):
            # #     output_fs.write(problem_type + ",")
            # #     for val in cfv_results[problem_type_idx]:
            # #         output_fs.write(val)
            #     output_fs.write("\n")


  # regression_models_list = []

        # #read in regression models trained on each problem type individually
        # for problem_type_idx_2, problem_type_2 in enumerate(problem_types):
        #     #read in models with pickle
        #     with open(regression_models_pickle_output_folder + "/" + problem_type_2 + ".pkl" , 'rb') as pickle_input_fs:
        #         regression_models_list.append(pickle.load(pickle_input_fs))
        #
        # #read in regression models trained on all problem types
        # # read in models with pickle
        # with open(regression_models_pickle_output_folder + "/" + "all_problem_types" + ".pkl", 'rb') as pickle_input_fs:
        #     regression_models_list.append(pickle.load(pickle_input_fs))
            # use 10 fold cross validated

        # regression_model_trained_on = ["network_design", "fixed_cost_network_flow", "supply_network_planning",
        #                                "all_problems"]


#store the important features in a list
if __name__ == "__main__":

    main()