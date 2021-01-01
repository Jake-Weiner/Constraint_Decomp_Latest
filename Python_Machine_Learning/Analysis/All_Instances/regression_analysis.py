#importing libraries
from sklearn.datasets import load_boston
import pandas as pd
import numpy as np
from pathlib import Path
import matplotlib
import matplotlib.pyplot as plt
import seaborn as sns
# import statsmodels.api as sm

from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.feature_selection import RFE
from sklearn.linear_model import RidgeCV, LassoCV, Ridge, Lasso

from sklearn import datasets, linear_model
from sklearn.model_selection import cross_validate
from sklearn.metrics import make_scorer
from sklearn.metrics import confusion_matrix
from sklearn.svm import LinearSVC
from sklearn.linear_model import LinearRegression


def main():

    problem_types = ["network_design", "fixed_cost_network_flow"]
    # , "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]
                      #
                      # "ta2-UUE.mps"
                      # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                      #  "snp-10-052-052.mps"]]
    #read in dataset



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
    #remove any rows in which there are nan, inf or -inf values
    df_combined = df_combined[~df_combined.isin([np.nan, np.inf, -np.inf]).any(1)]
    #remove any values where the gap is less and 0%, a result of potential rounding errors from CPLEX
    df_combined = df_combined[df_combined['Gap (%)'] > 0]
    #reset the indexes
    df_combined.reset_index(drop=True, inplace=True)
    # print(df_combined)
    # remove default index and Decomp index from dataframe to store features
    X = df_combined.drop(columns=[df.columns[0], 'Decomposition Index', 'Gap (%)', 'LR Solve Time(s)'])
    # print(X.columns)
    # capture output columns
    Y = df_combined[['Gap (%)', 'LR Solve Time(s)']]
    # print(Y)

    # Using Pearson Correlation
    plt.figure(figsize=(12, 10))
    cor = df_combined.corr()
    # Correlation with output variable
    cor_bound_target = abs(cor['Gap (%)'])
    # print(cor_bound_target)
    # Selecting highly correlated features
    relevant_features_bound = cor_bound_target[cor_bound_target > 0.5]
    print(relevant_features_bound)
    # convert features to np array
    X_np = X.to_numpy()
    Bound_np = Y['Gap (%)'].to_numpy()
    reg = LinearRegression().fit(X_np, Bound_np)
    print("Regression model score is " + str(reg.score(X_np, Bound_np)))
    # use 10 fold cross validated
    cv_results = cross_validate(reg, X_np, Bound_np, cv=10)
    sorted(cv_results.keys())
    print("Crossfold validation scores are - ")
    print(cv_results['test_score'])

if __name__ == "__main__":

    main()