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
from sklearn.model_selection import train_test_split
import os
import statistics
import pickle
import math
import textwrap
plt.style.use('ggplot')

#global vars
models = ['Voting']
problem_types = ["random_MIPLIB"]
instance_names = [
    ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
     "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]
# egression_models_pickle_output_folder + "/" + model_name + "_" + "all_network_instances" + ".pkl"

data_trained_on_list = ["all_network_instances"]
features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

regression_models_pickle_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/regression_models"
model_prediction_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Machine_Learning_Outputs/DT_evaluation"
processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"




def generateScoresNetworkTrainedModel():
    for problem_type_idx, problem_type in enumerate(problem_types):
        print("for problem type " + problem_type)
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            #Read in statistics
            df = pd.read_csv(input_data_filepath)
            # remove default index and Decomp index from dataframe to store features
            Y_np = df['Decomp Score'].to_numpy()
            X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'Normalised Gap (%)', 'LR Solve Time(s)', 'Decomp Score'])
            #get the index of the decomposition with no relaxed constraints
            no_decomp_row_num = X[X['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats'] == 0].index[0]
            X_np = X.to_numpy()

            Path(
                model_prediction_output_folder + "/" + problem_type).mkdir(
                parents=True, exist_ok=True)

            # data_trained_on_list = [problem_type, "all_problem_types"]
            for model_name in models:
                saved_model_name = model_name + "_" + "all_network_instances"
                with open(regression_models_pickle_input_folder + "/" + saved_model_name + ".pkl",
                          'rb') as pickle_input_fs:
                    model = pickle.load(pickle_input_fs)
                    # print(model.get_params())
                    df_ml = pd.DataFrame()
                    df_ml['Decomposition Index'] = df['Decomposition Index']
                    df_ml['ML predicted val'] = pd.Series(model.predict(X_np))
                    if df_ml['ML predicted val'].min() == df_ml['ML predicted val'][no_decomp_row_num]:
                        print(df_ml['ML predicted val'])
                        print("Best ML Prediction is for no constraints relaxed. Instance Name - {}, ML model - {}".format(instance_name,model_name))
                    ML_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
                    Path(ML_output_folder).mkdir(parents=True, exist_ok=True)
                    ML_output_filename = saved_model_name + ".csv"
                    df_ml.to_csv(ML_output_folder + "/" + ML_output_filename)
            print("Finished instance name {}, model {}".format(instance_name,model_name))
    return


#def scores for all instances

#def scores for other problem types

def main():

    generateScoresNetworkTrainedModel()


#store the important features in a list
if __name__ == "__main__":

    main()