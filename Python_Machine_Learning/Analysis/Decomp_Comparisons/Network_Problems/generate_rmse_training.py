# importing libraries
import pandas as pd

import pickle
import math
from sklearn.metrics import mean_squared_error
from pathlib import Path

#global vars
instance_names_training_flat_list =  ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps",
        "g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps",
        "snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]

problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

instance_names = [
        ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
        ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
        ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]]

processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

#regression model pickle path
regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"

model_comparisons_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"

def generateAllDecompDF():
    # generate DF that contains all of the decompositions from every instance
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
    return df_all_problems_combined.copy()

# rmse scores for models trained on all data except for test instance
def generate_training_rmse_scores(models):
    df_all_problems_combined = generateAllDecompDF()
    for problem_type_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            df_training = df_all_problems_combined[df_all_problems_combined['Instance Name'] != instance_name].copy()
            print(instance_name)
            print(df_training.shape)
            features_np = df_training.drop(
                columns=[df_training.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                         'Decomp Score', 'Instance Name'])

            training_rmse_folder = model_comparisons_outputs_root_folder
            rmse_training_output_path = training_rmse_folder + "/" + "RMSE_training_scores_" + "all_problem_types" + ".csv"
            if not Path(rmse_training_output_path).is_file():
                with open(rmse_training_output_path,
                          "w") as RMSE_training_scores_output_fs:
                    RMSE_training_scores_output_fs.write(
                        "Problem Type,Instance Name,Ranking Method,Training RMSE" + "\n")

            with open(rmse_training_output_path,
                      "a+") as RMSE_training_scores_output_fs:
                for model_name in models:
                    saved_model_name = model_name + "_" + "all_problem_types" + "_" + instance_name
                    with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                              'rb') as pickle_input_fs:
                        model = pickle.load(pickle_input_fs)
                        predicted_scores = model.predict(features_np)
                        rmse = math.sqrt(mean_squared_error(df_training["Decomp Score"],
                                                            predicted_scores))

                        RMSE_training_scores_output_fs.write('{},{},{},{}\n'.format(problem_type, instance_name, model_name, rmse))
                    print("Finished model {} for instance {}".format(model_name, instance_name))


def generateAllDataByProblemType(problem_type, problem_type_idx):
    df_problem_type_list = []
    for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
        input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
        df = pd.read_csv(input_data_filepath)
        df['Instance Name'] = instance_name
        df_problem_type_list.append(df)

    df_problem_type_combined = pd.concat(df_problem_type_list, keys=instance_names_training_flat_list)
    # reset the indexes
    df_problem_type_combined.reset_index(drop=True, inplace=True)

    return df_problem_type_combined.copy()

#generate RMSE training scores for models trained on data from all instances from each problem type
def generateRMSEByProblemType(models):
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_training = generateAllDataByProblemType(problem_type, problem_type_idx)
        features_np = df_training.drop(
            columns=[df_training.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                     'Decomp Score', 'Instance Name'])

        training_rmse_folder = model_comparisons_outputs_root_folder
        rmse_training_output_path = training_rmse_folder + "/" + "RMSE_training_scores_" + problem_type + ".csv"
        if not Path(rmse_training_output_path).is_file():
            with open(rmse_training_output_path,
                      "w") as RMSE_training_scores_output_fs:
                RMSE_training_scores_output_fs.write(
                    "Problem Type,Ranking Method,Training RMSE" + "\n")

        with open(rmse_training_output_path,
                  "a+") as RMSE_training_scores_output_fs:
            for model_name in models:
                saved_model_name = model_name + "_" + problem_type
                with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                          'rb') as pickle_input_fs:
                    model = pickle.load(pickle_input_fs)
                    predicted_scores = model.predict(features_np)
                    rmse = math.sqrt(mean_squared_error(df_training["Decomp Score"],
                                                        predicted_scores))

                    RMSE_training_scores_output_fs.write(
                        '{},{},{}\n'.format(problem_type, model_name, rmse))
                print("Finished model {} for problem type {}".format(model_name, problem_type))

#get all data
def generateRMSEByProblemTypeExclTest(models):
#generate RMSE scores for models trained on all instances for each problem type except for the test instance
    # train model on each problem type except for 1 instance left for testing
    for problem_type_idx, problem_type in enumerate(problem_types):
        df_combined = generateAllDataByProblemType(problem_type, problem_type_idx)
        # for each problem type, train each model on all instances except for test instance
        for inner_instance_idx, inner_instance_name in enumerate(instance_names[problem_type_idx]):
            # df_training = df_list[df_list[]]
            df_training = df_combined[df_combined['Instance Name'] != inner_instance_name].copy()
            features_np = df_training.drop(
                columns=[df_training.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)',
                         'Decomp Score', 'Instance Name'])
            training_rmse_folder = model_comparisons_outputs_root_folder
            rmse_training_output_path = training_rmse_folder + "/" + "RMSE_training_scores_" + "problem_types_excl_test" + ".csv"
            if not Path(rmse_training_output_path).is_file():
                with open(rmse_training_output_path,
                          "w") as RMSE_training_scores_output_fs:
                    RMSE_training_scores_output_fs.write(
                        "Problem Type,Instance Name,Ranking Method,Training RMSE" + "\n")

            with open(rmse_training_output_path,
                      "a+") as RMSE_training_scores_output_fs:
                for model_name in models:
                    saved_model_name = model_name + "_" + problem_type + "_" + inner_instance_name
                    with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                              'rb') as pickle_input_fs:
                        model = pickle.load(pickle_input_fs)
                        predicted_scores = model.predict(features_np)
                        rmse = math.sqrt(mean_squared_error(df_training["Decomp Score"],
                                                            predicted_scores))
                        RMSE_training_scores_output_fs.write(
                            '{},{},{},{}\n'.format(problem_type, inner_instance_name, model_name, rmse))
                    print("Finished model {} for instance {}".format(model_name, inner_instance_name))


def main():
    model_names = []
    model_names.append('OLM')
    model_names.append('SVM')
    model_names.append('SGD')
    model_names.append('KNN')
    model_names.append('RF')
    model_names.append('MLP')
    model_names.append('Stacking')
    model_names.append('Voting')
    # generate_training_rmse_scores(model_names)
    # generateRMSEByProblemType(model_names)
    generateRMSEByProblemTypeExclTest(model_names)
# store the important features in a list
if __name__ == "__main__":
    main()