import csv
import os
import pandas as pd
import numpy as np


import matplotlib.pyplot as plt

# returns the indexes of individuals on the pareto front
def identify_pareto(scores, first_obj_sense, second_obj_sense):
    # Count number of items
    print(scores[0])
    population_size = scores.shape[0]
    # Create a NumPy index for scores on the pareto front (zero indexed)
    population_ids = np.arange(population_size)
    print(population_ids)
    # Create a starting list of items on the Pareto front
    # All items start off as being labelled as on the Pareto front
    pareto_front = np.ones(population_size, dtype=bool)
    print(pareto_front)
    # Loop through each item. This will then be compared with all other items
    for i in range(population_size):
        # Loop through all other items
        for j in range(population_size):
            if (i != j):
                # Check if our 'i' pint is dominated by out 'j' point
                if (first_obj_sense == 'Max' and second_obj_sense == 'Min'):
                    # i is dominated by another solution if the other solution is at least as good in all objectives
                    # and better in at least 1 objective
                    # is other at least as good as i
                    if scores[j][0] >= scores[i][0] and scores[j][1] <= scores[i][1]:
                        # is other better than i in at least 1 objective
                        if scores[j][0] > scores[i][0] or scores[j][1] < scores[i][1]:
                            pareto_front[i] = 0
                            break


    # Return idxs of individuals on pareto front
    return population_ids[pareto_front]

def main():

    # #output location
    LR_file_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Normalised_Data/LROutputs/LR_outputs.csv"
    output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Machine_Learning_Outputs"

    df = pd.read_csv(LR_file_path)
    # this removes all negative inf, positive inf LR bounds
    print(df.columns)
    print(df.tail)
    df.sort_values(by=['LR Solve Time(s)'], inplace=True)
    df = df[~df.isin([np.nan, np.inf, -np.inf]).any(1)]
    df.reset_index(drop=True, inplace=True)

    Y = df[['LR Bound', 'LR Solve Time(s)']]
    # print(Y[187])
    # print(Y)
    # convert features to np array
    Y_np = Y.to_numpy()

    # print(Y_np[1878])
    # print(Y_np.shape)


    pareto_score_idxs = list(identify_pareto(Y_np, 'Max', 'Min'))
    non_pareto_score_idxs = list(df.index.difference(pareto_score_idxs))

    print(pareto_score_idxs)
    print(non_pareto_score_idxs)
    # print(non_pareto_score_idxs)
    # print(df.iloc[pareto_score_idxs,2])
    # print(df[pareto_score_idxs,'LR Bound'])
    #create scatter plot of bound vs time



    X_pareto = df.iloc[pareto_score_idxs,2]
    Y_pareto = df.iloc[pareto_score_idxs,1]
    print(X_pareto)
    print(Y_pareto)
    plt.plot(df.iloc[pareto_score_idxs,2],df.iloc[pareto_score_idxs,1], ls = "--", marker = ".", color = 'red', label = 'Pareto')
    # plt.scatter(df.iloc[pareto_score_idxs, 2], df.iloc[pareto_score_idxs, 1], s=20, marker="s", c='r', label='Pareto')
    plt.scatter(df.iloc[non_pareto_score_idxs, 2], df.iloc[non_pareto_score_idxs, 1], s=8, marker="o", c='b', label='Non Pareto')


    plt.legend(loc='lower right');
    plt.xlabel(df.columns[2])
    plt.ylabel(df.columns[1])
    plt.savefig(output_folder + "/Bounds_vs_time_plot_pareto")



if __name__ == "__main__":

    main()
