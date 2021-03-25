#importing libraries
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib.pyplot as plt
import statistics
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
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
# problem_types = ["network_design", "fixed_cost_network_flow"]

instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]

features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

#print the best decomp scores possible for each problem type
def getBestDecompScores():

    # get the best decomp scores possible across each problem type
    for problem_type_idx, problem_type in enumerate(problem_types):
        best_decomp_scores = []
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df_collated = pd.read_csv(input_data_filepath)
            best_decomp_scores.append(df_collated["Decomp Score"].min())
        print("For {}, Best Decomp Scores = [{}]".format(problem_type,best_decomp_scores))


def main():
    #first test, same problemTest_Type_Enum

    getBestDecompScores()


#store the important features in a list
if __name__ == "__main__":

    main()