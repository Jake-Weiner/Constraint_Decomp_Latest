#importing libraries
from sklearn.datasets import load_boston
import pandas as pd
import numpy as np
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

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    #read in dataset
    input_dataset_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Collated/collated.csv"
    input_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Collated"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        Path(output_root_folder + "/" + problem_type + "/" + action_name).mkdir(
            parents=True, exist_ok=True)

        problem_type_X = pd.DataFrame
        problem_type_Y = pd.DataFrame

        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # create bash scripts


    df = pd.read_csv(input_dataset_filepath)
    # remove any rows with nan, inf or -inf vals
    df = df[~df.isin([np.nan, np.inf, -np.inf]).any(1)]
    # remove default index and Decomp index from dataframe to store features
    X = df.drop(columns = [df.columns[0], 'Decomposition Index', 'LR Bound', 'LR Solve Time(s)'])
    # print(X.columns)
    # capture output columns
    Y = df[['LR Bound', 'LR Solve Time(s)']]
    # print(Y)

    # Using Pearson Correlation
    plt.figure(figsize=(12, 10))
    cor = df.corr()

    # sns.heatmap(cor, annot=True, cmap=plt.cm.Reds)
    # plt.savefig(input_root_folder + "/heatmap")
    # Correlation with output variable
    cor_bound_target = abs(cor["LR Bound"])
    # print(cor_bound_target)
    # Selecting highly correlated features
    relevant_features_bound = cor_bound_target[cor_bound_target > 0.7]
    print(relevant_features_bound)

    # cor_time_target = abs(cor['LR Solve Time(s)'])
    # # print(cor_bound_target)
    # # Selecting highly correlated features
    # relevant_features_time = cor_time_target[cor_time_target > 0.5]
    # print(relevant_features_time)

    # print(relevant_features_bound.columns)

    # compare each highly correlated features with other highly correlated features

    # print(df[[i,j]].corr()) for i in relevant_features_bound.keys() for j in relevant_features_bound.keys() if i != j

    # for i in relevant_features_bound.keys():
    #     for j in relevant_features_bound.keys():
    #         if i != j:
    #             cor_local = df[[i, j]].corr()
    #             cor_bound_target_local = abs(cor_local[j])
    #             if (cor_bound_target_local[0] > 0.5):
    #                 print(i,j)
    #             # if (cor_bound_target_local[cor_bound_target_local < 0.5]):
    #             #
    #             #     print(i,j)



    #convert features to np array
    X_np = X.to_numpy()
    Bound_np = Y['LR Bound'].to_numpy()
    reg = LinearRegression().fit(X_np, Bound_np)

    print("Regression model score is " + str(reg.score(X_np, Bound_np)))

    # use 10 fold cross validated
    cv_results = cross_validate(reg, X_np, Bound_np, cv=10)
    sorted(cv_results.keys())

    print("Crossfold validation scores are - ")
    print(cv_results['test_score'])
# #
# diabetes = datasets.load_diabetes()
# X = diabetes.data[:150]
# y = diabetes.target[:150]
# lasso = linear_model.Lasso()

    #next task is to be able to train a regression model and test it via cross fold validation

    # X = df.drop("MEDV", 1)  # Feature Matrix
    # y = df["MEDV"]  # Target Variable
    # df.head()
    #
    # for filename in subproblem_ave_stddev:
    #     writeAveStddev(input_folder + "/" + filename, output_folder + "/" + filename)
    # #every other file in subproblem statistics folder, we want the min, max, ave and stddev
    # for filename in os.listdir(input_folder):
    #     if filename not in subproblem_ave_stddev:
    #         writeAllStats(input_folder + "/" + filename, output_folder + "/" + filename)



if __name__ == "__main__":

    main()