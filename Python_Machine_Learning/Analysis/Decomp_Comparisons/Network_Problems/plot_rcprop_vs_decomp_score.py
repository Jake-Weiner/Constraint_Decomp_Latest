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
model_comparisons_outputs_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"


def plotRCPropVsScore():

    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            df = pd.read_csv(features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv")
            rc_prop_np = df['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats'].to_numpy() * 100
            decomp_score_np = df['Decomp Score'].to_numpy()
            plt.figure()
            plt.ylabel("Decomp Score")
            plt.xlabel("Relaxed Constraint (%)")
            # plot prediction vs actual gap
            plt.scatter(rc_prop_np, decomp_score_np)
            plt.title('Decomp Score vs Relaxed Constraint (%)')
            plt.tight_layout()
            plt.savefig(
                model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" + instance_name.split(".")[0] + "_Score_vs_RCProp")

def plotScoreDistribution():

    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
            df = pd.read_csv(features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv")
            decomp_index_np = df['Decomposition Index'].to_numpy()
            decomp_score_np = df['Decomp Score'].to_numpy()
            plt.figure()
            plt.ylabel("Decomp Score")
            plt.xlabel("Decomp Index")
            # plot prediction vs actual gap
            plt.scatter(decomp_index_np, decomp_score_np)
            plt.title('Decomp Score Distribution ' + instance_name)
            plt.tight_layout()
            plt.savefig(
                model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" + instance_name.split(".")[0] + "_Score_distribution")

def main():
    plotRCPropVsScore()
    plotScoreDistribution()


#store the important features in a list
if __name__ == "__main__":

    main()