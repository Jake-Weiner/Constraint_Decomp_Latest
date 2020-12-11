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

    pca = PCA()  # project from 64 to 2 dimensions
    # for component, score in enumerate(pca):

    # print(type(pca))


    projected = pca.fit_transform(X_np)
    print(pca.explained_variance_ratio_)

    component_indices = [i for i in range(1,len(pca.explained_variance_ratio_))]
    pca_explained_variance_ratios = [pca.explained_variance_ratio_[i] for i in range(0,len(pca.explained_variance_ratio_))]
    pca_fig = plt.figure()
    pca_ax = pca_fig.add_axes([0, 0, 1, 1])
    langs = component_indices
    students =pca_explained_variance_ratios
    pca_ax.bar(langs, students)

    pca_fig.savefig(output_folder + "/PCA_Scree")
    print(pca_explained_variance_ratios)
    print(component_indices)
    print(projected.shape)
    # print(pca.explained_variance_)
    # plt.scatter(projected[:, 0], projected[:, 1],
    #             c=Bound_np, edgecolor='none', alpha=0.7
    #             ,cmap=plt.cm.Reds)
    # plt.xlabel('component 1')
    # plt.ylabel('component 2')
    # cbar = plt.colorbar()
    # cbar.set_label('LR Bound', rotation = 270, labelpad = 20)
    # output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Machine_Learning_Outputs"
    # plt.savefig(output_folder + "/PCA_Analysis")

        # training_data = np.reshape(num_cars + num_ops + num_classes + min_usage + ave_usage + max_usage + std_dev_usage \
        #                    + ave_ops + min_pq + ave_pq + max_pq + std_dev_pq + lcm + std_dev_classes, [14, len(mip_ub)]).T
        #
        # gap_data = np.reshape(mip_gap_to_best_lb + lazy_gap_to_best_lb + lcm_gap_to_best_lb + ten_gap_to_best_lb  \
        #                    + lr_aco_gap_to_best_lb, [5, len(mip_gap_to_best_lb)]).T
        #
        # # label = source
        # # label = np.array(label)
        # # # print(label)
        # # source = np.array(source)
        #
        # label = []
        # for instance_class in source:
        #     if instance_class == class_names[0]:
        #         label.append(class_names[0])
        #     elif instance_class == class_names[1]:
        #         label.append(class_names[1])
        #     elif instance_class == class_names[2]:
        #         label.append(class_names[2])
        #     # elif instance_class == class_names[3]:
        #     #     label.append(class_names[3])
        #     # elif instance_class == class_names[4]:
        #     #     label.append(class_names[4])
        #     else:
        #         label.append(class_names[3])
        # label = np.array(label)
        #
        #
        # n_components = 2
        # pca = PCA(n_components=n_components)
        # X_pca = pca.fit_transform(training_data[:, [3,4,6,9,1,2,7,10]])
        # plt.figure(figsize=(8, 8))
        # # colors = "ryb"
        # # colors = ['cyan', 'navy', 'orange', 'maroon', 'red', 'grey']
        # colors = ['cyan', 'navy', 'orange', 'maroon']
        # # colors = ['cyan', 'navy', 'orange', 'maroon', 'grey']
        # for color, i, target_name in zip(colors, class_names, label_names):
        #     plt.scatter(X_pca[label == i, 0], X_pca[label == i, 1], c=color, lw=2, label=target_name, cmap=plt.cm.RdYlBu)
        # plt.legend(loc="upper left", ncol = 2, handletextpad = 0.0, columnspacing= 0.1, shadow=False, scatterpoints=1, fontsize=16)
        # plt.title('instance source', fontsize=20)
        # filename = 'figs/instance_source_distribution.png'
        # # filename = 'figs/instance_source_distribution_highU.png'
        # # filename = 'figs/instance_source_distribution_RandC.png'
        # # filename = 'figs/instance_source_distribution_pq.png'
        # plt.savefig(filename, bbox_inches='tight')
        # # plt.show()
        #


if __name__ == "__main__":

    main()


