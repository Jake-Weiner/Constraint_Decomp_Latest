# importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
from sklearn.linear_model import LinearRegression
from sklearn import svm
from sklearn.linear_model import SGDRegressor
from sklearn.neighbors import KNeighborsRegressor
from sklearn.ensemble import RandomForestRegressor
from sklearn.neural_network import MLPRegressor
from sklearn.ensemble import StackingRegressor
from sklearn.ensemble import VotingRegressor

import pickle

#global vars
instance_names_training_flat_list =  ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps",
        "g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps",
        "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]

problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
# problem_types = ["network_design", "fixed_cost_network_flow"]
instance_names = [
        ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
        ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
        ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]]

processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

# for each instance index in problem type, loop through all others to train on those. Then test instance is the index remaining

regression_models_pickle_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"

def get_stacking():
# define the base models
    level0 = []
    level0.append(('OLM', LinearRegression()))
    level0.append(('SVM', svm.SVR()))
    level0.append(('SGD', SGDRegressor()))
    level0.append(('KNN', KNeighborsRegressor()))
    level0.append(('RF', RandomForestRegressor()))
    level0.append(('MLP', MLPRegressor()))
    # define meta learner model
    level1 = LinearRegression()
    # define the stacking ensemble
    model = StackingRegressor(estimators=level0, final_estimator=level1, cv=5)
    return model

# get a voting ensemble of models
def get_voting():
    # define the base models
    models = []
    models.append(('OLM', LinearRegression()))
    models.append(('SVM', svm.SVR()))
    models.append(('SGD', SGDRegressor()))
    models.append(('KNN', KNeighborsRegressor()))
    models.append(('RF', RandomForestRegressor()))
    models.append(('MLP', MLPRegressor()))
    # define the voting ensemble
    ensemble = VotingRegressor(estimators=models)
    return ensemble

# train on all instances for each problem type
def train_problem_all_instance(models):
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        df_problem_type_list = []
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df['Instance Name'] = instance_name
            df_problem_type_list.append(df)

        df_problem_type_combined = pd.concat(df_problem_type_list, keys=instance_names_training_flat_list)
        # reset the indexes
        df_problem_type_combined.reset_index(drop=True, inplace=True)

        target_np = df_problem_type_combined['Decomp Score'].to_numpy()
        features_np = df_problem_type_combined.drop(
            columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                     'Decomp Score', 'Instance Name']).to_numpy()
        # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size = 0.25, shuffle = True, random_state = 1)
        for model_name, model in models:
            reg_model = model.fit(features_np, target_np)
            # store models with pickle
            with open(
                    regression_models_pickle_output_folder + "/" + model_name + "_" + problem_type + ".pkl",
                    'wb') as pickle_output_fs:
                pickle.dump(reg_model, pickle_output_fs)
            print("Finished model {} for problem type {}".format(model_name, problem_type))

def train_all_excl_test(models):
    # train on all instances in the dataset except for 1 instance to be tested
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

    # for each problem type, train each model on all instances except for test instance
    for inner_instance_idx, inner_instance_name in enumerate(instance_names_training_flat_list):
        # df_training = df_list[df_list[]]
        df_training = df_all_problems_combined[df_all_problems_combined['Instance Name'] != inner_instance_name]
        target_np = df_training['Decomp Score'].to_numpy()
        features_np = df_training.drop(
            columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                     'Decomp Score', 'Instance Name']).to_numpy()
        # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size = 0.25, shuffle = True, random_state = 1)
        for model_name, model in models:
            reg_model = model.fit(features_np, target_np)
            # store models with pickle
            with open(
                    regression_models_pickle_output_folder + "/" + model_name + "_" + "all_problem_types" + "_" + inner_instance_name + ".pkl",
                    'wb') as pickle_output_fs:
                pickle.dump(reg_model, pickle_output_fs)
            print("Finished model {} for problem type {}".format(model_name, "all_problem_types"))


def train_problem_excl_test(models):
    # train DT model on each problem type except for 1 instance left for testing
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_list = []
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df['Instance Name'] = instance_name
            df_list.append(df)
        df_combined = pd.concat(df_list, keys=instance_names[problem_type_idx])
        # for each problem type, train each model on all instances except for test instance
        for inner_instance_idx, inner_instance_name in enumerate(instance_names[problem_type_idx]):
            # df_training = df_list[df_list[]]
            df_training = df_combined[df_combined['Instance Name'] != inner_instance_name]
            target_np = df_training['Decomp Score'].to_numpy()
            features_np = df_training.drop(
                columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                         'Decomp Score', 'Instance Name']).to_numpy()
            # X_train, X_test, Y_train, Y_test = train_test_split(X_np, Bound_np, test_size = 0.25, shuffle = True, random_state = 1)
            for model_name, model in models:
                reg_model = model.fit(features_np, target_np)
                # store models with pickle
                with open(
                        regression_models_pickle_output_folder + "/" + model_name + "_" + problem_type + "_" + inner_instance_name + ".pkl",
                        'wb') as pickle_output_fs:
                    pickle.dump(reg_model, pickle_output_fs)
                print("Finished model {} for problem type {}".format(model_name, problem_type))

def main():
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "ta1-UUM.mps"],
    #                   ["g200x740.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps"
    #                    ]]
    # instance_names_training_flat_list= ["cost266-UUE.mps", "dfn-bwin-DBE.mps",
    #                   "g200x740.mps",
    #                   "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps"]

    # instance_names_training_flat_list = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]



                      #
                      # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                      #  "snp-10-052-052.mps"]]

    # instance_names_flat_list= ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps",
    #                   "g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps",
    #                   "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]
    # create sets for important features




    write_cfv_results = True
    cfv_results_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_CFV"
    cfv_results = []

    models = []
    # models.append(('OLM', LinearRegression()))
    # models.append(('SVM', svm.SVR()))
    # models.append(('SGD', SGDRegressor()))
    # models.append(('KNN', KNeighborsRegressor()))
    # models.append(('RF', RandomForestRegressor()))
    # models.append(('MLP', MLPRegressor()))
    # models.append(('Stacking', get_stacking()))
    models.append(('Voting', get_voting()))
    Path(regression_models_pickle_output_folder).mkdir(parents=True, exist_ok=True)
    train_problem_excl_test(models)
    train_all_excl_test(models)
    train_problem_all_instance(models)


# get a stacking ensemble of models


# get a list of models to evaluate
# def get_models():
# 	models = dict()
# 	models['knn'] = KNeighborsRegressor()
# 	models['cart'] = DecisionTreeRegressor()
# 	models['svm'] = SVR()
# 	models['stacking'] = get_stacking()
# 	return models
# 
# # evaluate a given model using cross-validation
# def evaluate_model(model, X, y):
# 	cv = RepeatedKFold(n_splits=10, n_repeats=3, random_state=1)
# 	scores = cross_val_score(model, X, y, scoring='neg_mean_absolute_error', cv=cv, n_jobs=-1, error_score='raise')
# 	return scores

# define dataset
# X, y = get_dataset()
# # get the models to evaluate
# models = get_models()
# # evaluate the models and store results
# results, names = list(), list()
# for name, model in models.items():
# 	scores = evaluate_model(model, X, y)
# 	results.append(scores)
# 	names.append(name)
# 	print('>%s %.3f (%.3f)' % (name, mean(scores), std(scores)))
# # plot model performance for comparison
# pyplot.boxplot(results, labels=names, showmeans=True)
# pyplot.show()



# store the important features in a list
if __name__ == "__main__":
    main()