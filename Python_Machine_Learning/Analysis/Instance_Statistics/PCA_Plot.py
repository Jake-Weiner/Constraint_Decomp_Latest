import sys
import numpy as np
import random
import time
import csv
import math
import sys
import pandas as pd
from sklearn.decomposition import PCA, IncrementalPCA


import matplotlib.pyplot as plt

def main():
    # read in dataset
    processed_results_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"
    output_folder = processed_results_root_folder + "/" + "Instance_Features"
    output_path = output_folder + "/" + "Instance_Features.csv"
    df = pd.read_csv(output_path)
    # remove any rows with nan, inf or -inf vals
    # remove default index and Decomp index from dataframe to store features
    X = df.drop(columns=[df.columns[0], 'instance_name', 'problem_type'])
    # print(X.columns)
    # capture output columns
    Y = df['problem_type']




    # colors = {'network_design': 'red', 'fixed_cost_network_flow': 'green', 'supply_network_planning': 'blue'}

    # ax.scatter(df['population'], df['Area'], c=df['continent'].map(colors))
    # print(Y)
    # convert features to np array
    X_np = X.to_numpy()



    pca = PCA(2)  # project from 64 to 2 dimensions
    # for component, score in enumerate(pca):

    # print(type(pca))
    # data = pd.DataFrame({"X Value": x, "Y Value": y, "Category": labels})

    #project now contains X_np represented as first 2 dimensions in PCA
    projected = pca.fit_transform(X_np)
    print(pca.explained_variance_ratio_)
    df_new = pd.DataFrame({"PCA 1" : projected[:, 0], "PCA 2" : projected[:,1], "Problem Type": Y})
    print(df_new)
    groups = df_new.groupby("Problem Type")

    # Plot
    fig, ax = plt.subplots()
    ax.margins(0.05)  # Optional, just adds 5% padding to the autoscaling
    for name, group in groups:
        ax.plot(group["PCA 1"], group["PCA 2"], marker='o', linestyle='', ms=6, label=name)
    ax.legend()
    plt.xlim([-2, 2])
    # plt.scatter(projected[:, 0], projected[:, 1],
    #             c=Y.map(colors), label=Y.map(colors), edgecolor='none', alpha=0.7)
    # plt.xlabel('component 1')
    # plt.ylabel('component 2')
    # plt.legend()
    plt.savefig(output_folder + "/PCA_Analysis")




if __name__ == "__main__":

    main()


