import sys
import numpy as np
import random
import time
import csv
import math
import sys
import pandas as pd
from pathlib import Path
from sklearn.decomposition import PCA, IncrementalPCA


import matplotlib.pyplot as plt

def main():
    problem_types = ["network_design", "fixed_cost_network_flow"]
    # , "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]
    #
    #
    # "ta2-UUE.mps"
    # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #  "snp-10-052-052.mps"]]
    # read in dataset




    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"

    # create output folders if they don't already exist
    Path(processed_results_folder + "/" + "Machine_Learning_Outputs" + "/" + "All_Problem_Types").mkdir(
        parents=True, exist_ok=True)

    df_list = []
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
            df = pd.read_csv(input_data_filepath)
            df_list.append(df)
            # remove any rows with nan, inf or -inf vals

    df_combined = pd.concat(df_list, keys=instance_names[problem_type_idx])
    # remove any rows in which there are nan, inf or -inf values
    df_combined = df_combined[~df_combined.isin([np.nan, np.inf, -np.inf]).any(1)]
    # remove any values where the gap is less and 0%, a result of potential rounding errors from CPLEX
    df_combined = df_combined[df_combined['Gap (%)'] > 0]
    print(df_combined)
    # reset the indexes
    df_combined.reset_index(drop=True, inplace=True)
    print(df_combined.columns)
    # remove default index and Decomp index from dataframe to store features
    X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', 'Gap (%)', 'LR Solve Time(s)'])
    # print(X.columns)
    # capture output columns
    Y = df_combined[['Gap (%)', 'LR Solve Time(s)']]
    # print(Y)

    X_np = X.to_numpy()
    Bound_np = Y['Gap (%)'].to_numpy()

    pca = PCA(2)  # project from 64 to 2 dimensions
    # for component, score in enumerate(pca):

    # print(type(pca))

    #project now contains X_np represented as first 2 dimensions in PCA
    projected = pca.fit_transform(X_np)
    print(pca.explained_variance_ratio_)

    plt.scatter(projected[:, 0], projected[:, 1],
                c=Bound_np, edgecolor='none', alpha=0.7
                ,cmap=plt.cm.hot)
    plt.xlabel('component 1')
    plt.ylabel('component 2')
    cbar = plt.colorbar()
    cbar.set_label('Gap (%)', rotation = 270, labelpad = 20)
    output_folder = processed_results_folder + "/" + "Machine_Learning_Outputs" + "/" + "All_Problem_Types"
    plt.savefig(output_folder + "/PCA_Analysis")




if __name__ == "__main__":

    main()


