import sys
#import path for my_plot_functions
sys.path.append("/home/jake/PycharmProjects/Decomposition_Machine_Learning/Analysis/plotting")
from my_plot import set_size
import pandas as pd
from sklearn.decomposition import PCA, KernelPCA
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler

# # Using seaborn's style
plt.style.use('seaborn')
plt.style.use('tex')
width = 528.93675 #width of EJOR paper

#
def main():
    # read in dataset

    #what is the type of
    PCA_plot_data = "all"

    processed_results_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/PCA_Analysis"
    output_path = output_folder + "/" + "Instance_Features" + "_" + PCA_plot_data + ".csv"
    df = pd.read_csv(output_path)
    # remove any rows with nan, inf or -inf vals
    # remove default index and Decomp index from dataframe to store features
    X = df.drop(columns=[df.columns[0], 'instance_name', 'problem_type'])
    # print(X.columns)
    # capture output columns
    Y = df['problem_type']

    scaler = MinMaxScaler()
    X[X.columns] = scaler.fit_transform(X[X.columns])
    # colors = {'network_design': 'red', 'fixed_cost_network_flow': 'green', 'supply_network_planning': 'blue'}

    # ax.scatter(df['population'], df['Area'], c=df['continent'].map(colors))
    # print(Y)
    # convert features to np array
    X_np = X.to_numpy()

    # linear PCA
    pca = PCA(2)  # project from 64 to 2 dimensions
    projected = pca.fit_transform(X_np)

    df_new = pd.DataFrame({"PCA 1": projected[:, 0], "PCA 2": projected[:, 1], "Problem Type": Y})
    print(df_new)
    # print(df_new)
    groups = df_new.groupby("Problem Type")
    # Plot
    fig, ax = plt.subplots(1, 1, figsize=set_size(width))
    # ax.set_xlabel("Principal Component 1 - Explained Variance Ratio = {:.2f}".format(pca.explained_variance_ratio_[0]))
    # ax.set_ylabel("Principal Component 2 - Explained Variance Ratio = {:.2f}".format(pca.explained_variance_ratio_[1]))
    ax.set_xlabel("Principal Component 1")
    ax.set_ylabel("Principal Component 2")
    ax.margins(0.05)  # Optional, just adds 5% padding to the autoscaling
    for name, group in groups:
        ax.plot(group["PCA 1"], group["PCA 2"], marker='o', linestyle='', ms=10, label=name)
    ax.legend()
    # Save and remove excess whitespace
    plt.savefig(output_folder + "/PCA_Analysis_linear" + "_" + PCA_plot_data + ".pdf", format='pdf', bbox_inches='tight')

    # #non-linear kernels
    # kernels = ["poly", "rbf", "sigmoid", "cosine"]
    # for selected_kernel in kernels:
    #     kpca = KernelPCA(n_components = 2, kernel=selected_kernel)
    #
    #     kpcs_projected = kpca.fit_transform(X_np)
    #     df_new = pd.DataFrame({"PCA 1": kpcs_projected[:, 0], "PCA 2": kpcs_projected[:, 1], "Problem Type": Y})
    #     print(df_new)
    #     # print(df_new)
    #     groups = df_new.groupby("Problem Type")
    #     # Plot
    #     fig, ax = plt.subplots()
    #     ax.set_xlabel("Principal Component 1")
    #     ax.set_ylabel("Principal Component 2")
    #     ax.margins(0.05)  # Optional, just adds 5% padding to the autoscaling
    #     for name, group in groups:
    #         ax.plot(group["PCA 1"], group["PCA 2"], marker='o', linestyle='', ms=10, label=name)
    #     ax.legend()
    #     # plt.xlim([-2, 2])
    #     # plt.savefig(output_folder + "/PCA_Analysis.pdf")
    #
    #     # Save and remove excess whitespace
    #     plt.savefig(output_folder + "/PCA_Analysis_" + selected_kernel + ".pdf", format='pdf', bbox_inches='tight')

    #project now contains X_np represented as first 2 dimensions in PCA
    # projected = pca.fit_transform(X_np)
    # print(pca.explained_variance_ratio_)
    # df_new = pd.DataFrame({"PCA 1" : projected[:, 0], "PCA 2" : projected[:,1], "Problem Type": Y})
    # print(np.square(pca.components_)[1])
    #
    # print(df_new)
    # # print(df_new)
    # groups = df_new.groupby("Problem Type")
    #
    # # Plot
    # fig, ax = plt.subplots()
    # ax.set_xlabel("Principal Component 1 - Explained Variance Ratio = {:.2f}".format(pca.explained_variance_ratio_[0]))
    # ax.set_ylabel("Principal Component 2 - Explained Variance Ratio = {:.2f}".format(pca.explained_variance_ratio_[1]))
    # ax.margins(0.05)  # Optional, just adds 5% padding to the autoscaling
    # for name, group in groups:
    #     ax.plot(group["PCA 1"], group["PCA 2"], marker='o', linestyle='', ms=6, label=name)
    # ax.legend()
    # # plt.xlim([-2, 2])
    # # plt.savefig(output_folder + "/PCA_Analysis.pdf")
    #
    # # Save and remove excess whitespace
    # plt.savefig(output_folder + "/PCA_Analysis.pdf", format='pdf', bbox_inches='tight')


if __name__ == "__main__":

    main()


