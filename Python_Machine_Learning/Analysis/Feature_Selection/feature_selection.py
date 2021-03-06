#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
from sklearn.feature_selection import SelectFromModel
import pickle
plt.style.use('ggplot')
from sklearn.inspection import permutation_importance
from collections import defaultdict
import matplotlib.pyplot as plt
import numpy as np
from scipy.stats import spearmanr
from scipy.cluster import hierarchy

#global vars
# models = ['OLM', 'SVM', 'SGD', 'KNN', 'RF', 'MLP']
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]
instance_names_training_flat_list =  ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps",
        "g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps",
        "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]
data_trained_on_list = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "all_problem_types"]
features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

def getCombinedDF():
    df_all_problems_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df['Instance Name'] = instance_name
            df_all_problems_list.append(df)

    df_all_problems_combined = pd.concat(df_all_problems_list, keys=instance_names_training_flat_list)
    # reset the indexes
    df_all_problems_combined.reset_index(drop=True, inplace=True)

    #return a deep copy
    return df_all_problems_combined.copy()

#given the dataframe with all instances, select the features and training data
def getFeaturesTarget(df_all_problems_combined, test_instance_name):
    df_training = df_all_problems_combined[df_all_problems_combined['Instance Name'] != test_instance_name]
    target_np = df_training['Decomp Score'].to_numpy()
    features_np = df_training.drop(
        columns=[df_training.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                 'Decomp Score', 'Instance Name']).to_numpy()

    return features_np, target_np

#write in list with all problem types, go through except for specific problem
def featureSelectionAllProblems():

    df_all_problems_combined = getCombinedDF()
    X = df_all_problems_combined.drop(
        columns=[df_all_problems_combined.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                 'Decomp Score', 'Instance Name'])
    # fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 8))
    # corr = spearmanr(X).correlation
    # corr_linkage = hierarchy.ward(corr)
    # dendro = hierarchy.dendrogram(
    #     corr_linkage, labels=X.columns.tolist(), ax=ax1, leaf_rotation=90
    # )
    # dendro_idx = np.arange(0, len(dendro['ivl']))
    # 
    # ax2.imshow(corr[dendro['leaves'], :][:, dendro['leaves']])
    # ax2.set_xticks(dendro_idx)
    # ax2.set_yticks(dendro_idx)
    # ax2.set_xticklabels(dendro['ivl'], rotation='vertical')
    # ax2.set_yticklabels(dendro['ivl'])
    # fig.tight_layout()
    # plt.savefig("/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/Feature_Analysis/dendro")
    # exit(0)
    # for each instance, the model has been trained on data from all instances except for the target instance
    for test_instance_idx, test_instance_name in enumerate(instance_names_training_flat_list):
        features_np, target_np = getFeaturesTarget(df_all_problems_combined, test_instance_name)
        model_name = 'Voting'
        saved_model_name = model_name + "_" + "all_problem_types" + "_" + test_instance_name
        with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                  'rb') as pickle_input_fs:
            model = pickle.load(pickle_input_fs)

            # print("For problem type {}, model {}".format(problem_type, model_name))
            # selector = SelectFromModel(estimator=model, prefit=True)
            # print(selector.get_support())
            # # define dataset
            #training set
            # perform permutation importance
            results = permutation_importance(model, features_np, target_np, scoring='neg_mean_squared_error')
            # get importance
            importance = results.importances_mean
            # summarize feature importance
            for i, v in enumerate(importance):
                print('Feature: %0d, Score: %.5f' % (i, v))
            # plot feature importance
            # plt.bar([x for x in range(len(importance))], importance)
            # plt.show()
            # selector.estimator_.coef
            # print(selector.estimator_.coef_)
            # print(selector.threshold_)
            # print(selector.get_support())

    return


        # df_training = df_list[df_list[]]


#def scores for all instances

#def scores for other problem types

def main():
    featureSelectionAllProblems()
    
# # permutation feature importance with knn for regression
# from sklearn.datasets import make_regression
# from sklearn.neighbors import KNeighborsRegressor
# from sklearn.inspection import permutation_importance
# from matplotlib import pyplot
# # define dataset
# X, y = make_regression(n_samples=1000, n_features=10, n_informative=5, random_state=1)
# # define the model
# model = KNeighborsRegressor()
# # fit the model
# model.fit(X, y)
# # perform permutation importance
# results = permutation_importance(model, X, y, scoring='neg_mean_squared_error')
# # get importance
# importance = results.importances_mean
# # summarize feature importance
# for i,v in enumerate(importance):
# 	print('Feature: %0d, Score: %.5f' % (i,v))
# # plot feature importance
# pyplot.bar([x for x in range(len(importance))], importance)
# pyplot.show()
#
# # permutation feature importance with knn for regression
# from sklearn.datasets import make_regression
# from sklearn.neighbors import KNeighborsRegressor
# from sklearn.inspection import permutation_importance
# from matplotlib import pyplot
# # define dataset
# X, y = make_regression(n_samples=1000, n_features=10, n_informative=5, random_state=1)
# # define the model
# model = KNeighborsRegressor()
# # fit the model
# model.fit(X, y)
# # perform permutation importance
# results = permutation_importance(model, X, y, scoring='neg_mean_squared_error')
# # get importance
# importance = results.importances_mean
# # summarize feature importance
# for i,v in enumerate(importance):
# 	print('Feature: %0d, Score: %.5f' % (i,v))
# # plot feature importance
# pyplot.bar([x for x in range(len(importance))], importance)
# pyplot.show()

#store the important features in a list
if __name__ == "__main__":

    main()