import sys
import numpy as np
import random
import time
import csv
import math
import sys
import pandas as pd
from sklearn import svm
#import identify pareto functionality
from LR_Outputs_pareto import identify_pareto
from sklearn.decomposition import PCA, IncrementalPCA
from sklearn.model_selection import cross_validate

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
    Y_np = Y.to_numpy()
    pca = PCA()  # project from 64 to 2 dimensions
    # for component, score in enumerate(pca):

    # print(type(pca))

    #label the pareto points



    #initialise all points as non-pareto optimal
    pareto_score_idxs = list(identify_pareto(Y_np, 'Max', 'Min'))

    labels = np.zeros(df.shape[0])
    for i in pareto_score_idxs:
        labels[i] = 1

    print(labels)

    pca = PCA(2)  # project from original to 2 dimensions
    X_pca = pca.fit_transform(X_np)

    # all x points are converted to 2 dimensions using first 2 PCA components
    X_pca_two_dims = X_pca[:, [0, 1]]

    print(X_pca_two_dims)
    y = labels
    plt.figure(figsize=(8, 8))

    # Train svm classifier
    c = 1750/50
    clf = svm.SVC(kernel='rbf', C=1,  class_weight = 'balanced').fit(X_pca, y)
    # clf = svm.SVC(kernel='rbf', C=c, class_weight = 'balanced').fit(X_pca, y)

    plot_step = 0.02

    #x and y dimensions for the PCA plot
    x_min, x_max = X_pca_two_dims[:, 0].min() - 0.05, X_pca_two_dims[:, 0].max() + 0.05
    y_min, y_max = X_pca_two_dims[:, 1].min() - 0.05, X_pca_two_dims[:, 1].max() + 0.05
    xx, yy = np.meshgrid(np.arange(x_min, x_max, plot_step),
                         np.arange(y_min, y_max, plot_step))

    print(xx)
    print(yy)
    plt.tight_layout(h_pad=0.5, w_pad=0.5, pad=2.5)
    #

    #predict classification for x and y vals / grid
    Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])
    #reshape Z to plot contour
    Z = Z.reshape(xx.shape)
    #
    # plt.contourf(xx, yy, Z, vmin=0, vmax=2, cmap=plt.cm.RdYlBu)
    plt.contourf(xx, yy, Z, vmin=0, vmax=2, cmap=plt.cm.RdYlBu)
    plt.colorbar()

    #plot non-pareto points

    pareto_idxs = np.where(y==1)
    non_pareto_idxs = np.where(y == 0)

    plt.scatter(X_pca_two_dims[non_pareto_idxs, 0], X_pca_two_dims[non_pareto_idxs, 1], c='r', lw=2, label='Non Pareto')
    plt.scatter(X_pca_two_dims[pareto_idxs, 0], X_pca_two_dims[pareto_idxs, 1], c='y', lw=2, label='Pareto')
    plt.legend(loc='upper left');
    plt.savefig("/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Machine_Learning_Outputs/svm_test")


    # use 10 fold cross validated
    cv_results = cross_validate(clf, X_pca, labels, cv=10)
    sorted(cv_results.keys())

    print("Crossfold validation scores are - ")
    print(cv_results['test_score'])
    #

    # for i, color, target_name in zip([0, 1, 2], plot_colors, target_names):
    # # idx = np.where(y == i)
    # #             plt.scatter(X[idx, 0], X[idx, 1], c=color, lw=2, label=target_name, cmap=plt.cm.RdYlBu, edgecolor='black')

    # plt.scatter(X[idx, 0], X[idx, 1], c=color, lw=2, label=target_name, cmap=plt.cm.RdYlBu, edgecolor='black')

    target_names = ["Non-pareto", "Pareto"]

    #
    #         # plt.xlabel(feature_names[4], fontsize=16)
    #         # plt.ylabel(feature_names[7], fontsize=16)
    #
    #         # Plot the training points
    #         for i, color, target_name in zip([0,1,2], plot_colors, target_names):
    #             idx = np.where(y == i)
    #             plt.scatter(X[idx, 0], X[idx, 1], c=color, lw=2, label=target_name, cmap=plt.cm.RdYlBu, edgecolor='black')
    #
    #         plt.legend(loc="best", shadow=False, scatterpoints=1, fontsize=16)
    #         plt.title(algorithm_pair + ': SVM boundary', fontsize=20)
    #         filename = 'figs/' + file + '_SVMboundary.png'
    #         plt.savefig(filename, bbox_inches='tight')

    # for component, score in enumerate(pca):

    # print(type(pca))

   # (n_components=n_components)
    #
    #
    #         n_classes = 3
    #         plot_colors = "ryb"
    #
    #         plot_step = 0.02
    #         depth = 4
    #         # for pairidx, pair in enumerate([[3, 4], [3, 7], [3, 6], [4, 7], [4, 6], [7, 6]]):
    #
    #         X = X_pca[:, [0,1]]
    #         # X = training_data[:, [4, 7]]
    #         y = label
    #         plt.figure(figsize=(8, 8))
    #
    #         # Train
    #         # clf = tree.DecisionTreeClassifier().fit(X, y)
    #         clf = svm.SVC(kernel='rbf', C=1000).fit(X, y)
    #         # clf = tree.DecisionTreeClassifier(max_depth=depth).fit(X, y)
    #
    #         x_min, x_max = X[:, 0].min() - 0.05, X[:, 0].max() + 0.05
    #         y_min, y_max = X[:, 1].min() - 0.05, X[:, 1].max() + 0.05
    #         xx, yy = np.meshgrid(np.arange(x_min, x_max, plot_step),
    #                              np.arange(y_min, y_max, plot_step))
    #         plt.tight_layout(h_pad=0.5, w_pad=0.5, pad=2.5)
    #
    #


    # project now contains X_np represented as first 2 dimensions in PCA
    # projected = pca.fit_transform(X_np)
    # print(pca.explained_variance_ratio_)
    #
    # # print(pareto_score_idxs)
    # # print(non_pareto_score_idxs)
    # # # print(non_pareto_score_idxs)
    # # # print(df.iloc[pareto_score_idxs,2])
    # # # print(df[pareto_score_idxs,'LR Bound'])
    # # # create scatter plot of bound vs time
    # #
    # X_pareto = df.iloc[pareto_score_idxs, 2]
    # Y_pareto = df.iloc[pareto_score_idxs, 1]
    # #
    #
    # # component_indices = [i for i in range(1,len(pca.explained_variance_ratio_))]
    # # pca_explained_variance_ratios = [pca.explained_variance_ratio_[i] for i in range(0,len(pca.explained_variance_ratio_))]
    # # pca_fig = plt.figure()
    # # pca_ax = pca_fig.add_axes([0, 0, 1, 1])
    # # langs = component_indices
    # # students =pca_explained_variance_ratios
    # # pca_ax.bar(langs, students)
    #
    # pca_fig.savefig(output_folder + "/PCA_Scree")
    # print(pca_explained_variance_ratios)
    # print(component_indices)
    # print(projected.shape)
    # # print(pca.explained_variance_)
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




# for i in range(0, len(gap1)):
#             if  gap1[i] - gap2[i] > 0.005:
#                 label.append(2)
#             elif gap1[i] - gap2[i] < - 0.005:
#                 label.append(0)
#             else:
#                 label.append(1)
#         label = np.array(label)

#         n_components = 2
#         pca = PCA(n_components=n_components)
#         X_pca = pca.fit_transform(training_data[:, [3, 4, 6, 9, 1, 2, 7, 10]])
#
#         n_classes = 3
#         plot_colors = "ryb"
#
#         plot_step = 0.02
#         depth = 4
#         # for pairidx, pair in enumerate([[3, 4], [3, 7], [3, 6], [4, 7], [4, 6], [7, 6]]):
#
#         X = X_pca[:, [0,1]]
#         # X = training_data[:, [4, 7]]
#         y = label
#         plt.figure(figsize=(8, 8))
#
#         # Train
#         # clf = tree.DecisionTreeClassifier().fit(X, y)
#         clf = svm.SVC(kernel='rbf', C=1000).fit(X, y)
#         # clf = tree.DecisionTreeClassifier(max_depth=depth).fit(X, y)
#
#         x_min, x_max = X[:, 0].min() - 0.05, X[:, 0].max() + 0.05
#         y_min, y_max = X[:, 1].min() - 0.05, X[:, 1].max() + 0.05
#         xx, yy = np.meshgrid(np.arange(x_min, x_max, plot_step),
#                              np.arange(y_min, y_max, plot_step))
#         plt.tight_layout(h_pad=0.5, w_pad=0.5, pad=2.5)
#
#         Z = clf.predict(np.c_[xx.ravel(), yy.ravel()])
#         Z = Z.reshape(xx.shape)
#         # cs = plt.contourf(xx, yy, Z, colors=plot_colors)
#         cs = plt.contourf(xx, yy, Z, vmin=0, vmax=2, cmap=plt.cm.RdYlBu)
#         # cs = plt.contourf(xx, yy, Z, levels=[-1,0,1,2], colors=plot_colors)
#
#         # plt.xlabel(feature_names[4], fontsize=16)
#         # plt.ylabel(feature_names[7], fontsize=16)
#
#         # Plot the training points
#         for i, color, target_name in zip([0,1,2], plot_colors, target_names):
#             idx = np.where(y == i)
#             plt.scatter(X[idx, 0], X[idx, 1], c=color, lw=2, label=target_name, cmap=plt.cm.RdYlBu, edgecolor='black')
#
#         plt.legend(loc="best", shadow=False, scatterpoints=1, fontsize=16)
#         plt.title(algorithm_pair + ': SVM boundary', fontsize=20)
#         filename = 'figs/' + file + '_SVMboundary.png'
#         plt.savefig(filename, bbox_inches='tight')
#
#
#
