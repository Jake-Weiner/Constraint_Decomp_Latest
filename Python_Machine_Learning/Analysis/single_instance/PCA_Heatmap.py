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
    input_dataset_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Collated/collated.csv"
    input_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Collated"
    df = pd.read_csv(input_dataset_filepath)
    # remove any rows with nan, inf or -inf vals
    df = df[~df.isin([np.nan, np.inf, -np.inf]).any(1)]
    # remove default index and Decomp index from dataframe to store features
    X = df.drop(columns=[df.columns[0], 'Decomposition Index', 'LR Bound', 'LR Solve Time(s)'])
    # print(X.columns)
    # capture output columns
    Y = df[['LR Bound', 'LR Solve Time(s)']]
    # print(Y)
    # convert features to np array
    X_np = X.to_numpy()
    Bound_np = Y['LR Bound'].to_numpy()

    pca = PCA(2)  # project from 64 to 2 dimensions
    # for component, score in enumerate(pca):

    # print(type(pca))


    #project now contains X_np represented as first 2 dimensions in PCA
    projected = pca.fit_transform(X_np)
    print(pca.explained_variance_ratio_)

    plt.scatter(projected[:, 0], projected[:, 1],
                c=Bound_np, edgecolor='none', alpha=0.7
                ,cmap=plt.cm.Reds)
    plt.xlabel('component 1')
    plt.ylabel('component 2')
    cbar = plt.colorbar()
    cbar.set_label('LR Bound', rotation = 270, labelpad = 20)
    output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Machine_Learning_Outputs"
    plt.savefig(output_folder + "/PCA_Analysis")




if __name__ == "__main__":

    main()


