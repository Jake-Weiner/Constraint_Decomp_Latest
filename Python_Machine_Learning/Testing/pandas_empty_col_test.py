import os
import pandas as pd
import functools
from pathlib import Path

#this script collates all features into the same row to be used for processing
def main():

    problem_type = "network_design"

    instance_name = "cost266-UUE.mps"
    processed_results_external_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed/"
    #collate the features in a single csv

    data_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    #data set key which is used for dataframe merging
    key = 'Decomposition Index'

    df_temp = pd.read_csv(processed_results_external_folder + "/" + problem_type + "/" + instance_name + "/" + data_folders[0] + "/" + "Bin_props.csv")
    print(df_temp)
    df_temp.fillna(0, inplace=True)
    print(df_temp)

if __name__ == "__main__":
    main()