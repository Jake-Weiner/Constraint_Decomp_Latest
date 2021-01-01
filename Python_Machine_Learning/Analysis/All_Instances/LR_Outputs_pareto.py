import csv
import os
import pandas as pd
import numpy as np
from pathlib import Path

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
                if (first_obj_sense == 'Min' and second_obj_sense == 'Min'):
                    # i is dominated by another solution if the other solution is at least as good in all objectives
                    # and better in at least 1 objective
                    # is other at least as good as i
                    if scores[j][0] <= scores[i][0] and scores[j][1] <= scores[i][1]:
                        # is other better than i in at least 1 objective
                        if scores[j][0] < scores[i][0] or scores[j][1] < scores[i][1]:
                            pareto_front[i] = 0
                            break


    # Return idxs of individuals on pareto front
    return population_ids[pareto_front]

def main():

    # #output location

    problem_types = ["network_design", "fixed_cost_network_flow"]
    # , "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]
    #
    #, "ta2-UUE.mps"
    # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #  "snp-10-052-052.mps"]]
    # read in dataset

    problem_type_chosen_idx = 1
    problem_type = problem_types[problem_type_chosen_idx]

    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"

    # create output folders if they don't already exist
    Path(
        processed_results_folder + "/" + "Machine_Learning_Outputs" + "/" + "Problem_Types" + "/" + problem_type).mkdir(
        parents=True, exist_ok=True)

    df_list = []
    for instance_idx, instance_name in enumerate(instance_names[problem_type_chosen_idx]):
        input_data_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs.csv"
        df = pd.read_csv(input_data_filepath)
        df_list.append(df)
        # remove any rows with nan, inf or -inf vals

    output_folder = processed_results_folder + "/" + problem_type

    df_combined = pd.concat(df_list, keys=instance_names[problem_type_chosen_idx])
    # remove any rows in which there are nan, inf or -inf values
    df_combined = df_combined[~df_combined.isin([np.nan, np.inf, -np.inf]).any(1)]
    # remove any values where the gap is less and 0%, a result of potential rounding errors from CPLEX
    df_combined = df_combined[df_combined['Gap (%)'] > 0]
    # reset the indexes
    df_combined.reset_index(drop=True, inplace=True)
    # this removes all negative inf, positive inf LR bounds
    print(df_combined.columns)
    print(df_combined.tail)

    df_combined.sort_values(by=['LR Solve Time(s)'], inplace=True)
    Y = df_combined[['Gap (%)', 'LR Solve Time(s)']]
    # convert features to np array
    Y_np = Y.to_numpy()
    pareto_score_idxs = list(identify_pareto(Y_np, 'Min', 'Min'))
    non_pareto_score_idxs = list(df_combined.index.difference(pareto_score_idxs))
    print(pareto_score_idxs)
    print(non_pareto_score_idxs)
    #create scatter plot of bound vs time
    X_pareto = df_combined.iloc[pareto_score_idxs,2]
    Y_pareto = df_combined.iloc[pareto_score_idxs,1]
    print(X_pareto)
    print(Y_pareto)
    plt.plot(df_combined.iloc[pareto_score_idxs,2],df_combined.iloc[pareto_score_idxs,1], ls = "--", marker = ".", color = 'red', label = 'Pareto')
    # plt.scatter(df.iloc[pareto_score_idxs, 2], df.iloc[pareto_score_idxs, 1], s=20, marker="s", c='r', label='Pareto')
    plt.scatter(df_combined.iloc[non_pareto_score_idxs, 2], df_combined.iloc[non_pareto_score_idxs, 1], s=8, marker="o", c='b', label='Non Pareto')
    plt.legend(loc='lower right');
    plt.xlabel(df_combined.columns[2])
    plt.ylabel(df_combined.columns[1])
    plt.savefig(output_folder + "/Bounds_vs_time_plot_pareto")


if __name__ == "__main__":

    main()
