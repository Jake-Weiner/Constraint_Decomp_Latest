import sys
import numpy as np
import random
import time
import csv
import math
import sys
import pandas as pd
import os,errno
import scipy.io
from numpy import genfromtxt
#from sklearn.model_selection import train_test_split
from sklearn import datasets
from sklearn import svm
from sklearn.model_selection import ShuffleSplit
from sklearn.model_selection import cross_val_score
from sklearn import neighbors
from skfeature.function.information_theoretical_based.MIM import mim
from skfeature.function.information_theoretical_based.MRMR import mrmr
from skfeature.function.information_theoretical_based.JMI import jmi
from skfeature.function.information_theoretical_based.CMIM import cmim
from skfeature.function.information_theoretical_based.RelaxMRMR import relaxmrmr
from skfeature.utility.data_discretization import data_discretization
from sklearn.decomposition import PCA, IncrementalPCA
import matplotlib.pyplot as plt
from sklearn.datasets import load_iris

from sklearn import tree


def fileSaving(filename, data, writing_model):
    with open(filename, writing_model) as f:
        f_csv=csv.writer(f, quoting=csv.QUOTE_NONE)
        f_csv.writerow(data)

if __name__ == '__main__':

    total = len(sys.argv)
    cmdargs = str(sys.argv)

    print ("The total numbers of args passed to the script: %d " % total)
    print ("Args list: %s " % cmdargs)


    # Classifers = ['SVM','KNN','DT']
    Classifers = ['DT']

    FeatSelMethods = ['MIM','MRMR','JMI','CMIM','ALL']
    # FeatSelMethods = ['ALL']

    algorithm_pairs = ['MIP vs LNS', 'Plain MIP vs Lazy MIP', 'LCM-LNS vs 10-LNS', 'Plain MIP vs LCM-LNS', \
                       'Plain MIP vs 10-LNS', 'Lazy MIP vs LCM-LNS', 'Lazy MIP vs 10-LNS']

    maxRun = 100
    n_neighbors = 3

    instance_name = []
    num_cars = []
    num_ops = []
    num_classes = []
    min_usage = []
    ave_usage = []
    max_usage = []
    std_dev_usage = []
    ave_ops = []
    min_pq = []
    ave_pq = []
    max_pq = []
    std_dev_pq = []
    lcm = []
    std_dev_classes = []

    lr_aco_ub = []
    mip_ub = []
    lr_lcm_window_ub = []
    lr_10_window_ub = []
    lazy_ub = []
    mip_gap = []
    lr_aco_gap = []
    lazy_gap = []
    lr_aco_gap_to_best_lb = []
    mip_gap_to_best_lb = []
    lazy_gap_to_best_lb = []
    lcm_gap_to_best_lb = []
    ten_gap_to_best_lb = []

    label = []

    feature_names = ['num-cars', 'num-ops', 'num-classes', 'min-usage', 'ave-usage', 'max-usage', 'std-dev-usage', \
                     'ave-ops', 'min-pq', 'ave-pq', 'max-pq', 'std-dev-pq', 'lcm', 'std-dev-classes']


    filename = 'training_data.csv'
    with open(filename, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            # print(row['instance'], row['num-cars'])
            instance_name.append(row['instance'])
            num_cars.append(float(row['num-cars']))
            num_ops.append(float(row['num-ops']))
            num_classes.append(float(row['num-classes']))
            min_usage.append(float(row['min-usage']))
            ave_usage.append(float(row['ave-usage']))
            max_usage.append(float(row['max-usage']))
            std_dev_usage.append(float(row['std-dev-usage']))
            ave_ops.append(float(row['ave-ops']))
            min_pq.append(float(row['min-pq']))
            ave_pq.append(float(row['ave-pq']))
            max_pq.append(float(row['max-pq']))
            std_dev_pq.append(float(row['std-dev-pq']))
            lcm.append(float(row['lcm']))
            std_dev_classes.append(float(row['std-dev-classes']))

            lr_aco_ub.append(float(row['lr-aco-ub']))
            mip_ub.append(float(row['mip-ub']))
            lr_lcm_window_ub.append(float(row['5lr-lcm-window-ub']))
            lr_10_window_ub.append(float(row['5lr-10-window-ub']))
            lazy_ub.append(float(row['lazy-ub']))
            mip_gap.append(float(row['mip-gap']))
            lr_aco_gap.append(float(row['lr-aco-gap']))
            lazy_gap.append(float(row['lazy-gap']))
            lr_aco_gap_to_best_lb.append(float(row['lr-aco-gap-to-best-lb']))
            mip_gap_to_best_lb.append(float(row['mip-gap-to-best-lb']))
            lazy_gap_to_best_lb.append(float(row['lazy-gap-to-best-lb']))
            lcm_gap_to_best_lb.append(float(row['lcm-gap-to-best-lb']))
            ten_gap_to_best_lb.append(float(row['10-gap-to-best-lb']))

    # normalize the feature values
    max_num_cars = max(num_cars)
    num_cars = [value / max_num_cars for value in num_cars]
    # print(num_cars)
    max_num_ops = max(num_ops)
    num_ops = [value / max_num_ops for value in num_ops]
    # print(num_ops)
    max_num_classes = max(num_classes)
    num_classes = [value / max_num_classes for value in num_classes]
    # print(num_classes)
    max_lcm = max(lcm)
    lcm = [value / max_lcm for value in lcm]
    # print(lcm)

    training_data = np.reshape(num_cars + num_ops + num_classes + min_usage + ave_usage + max_usage + std_dev_usage \
                               + ave_ops + min_pq + ave_pq + max_pq + std_dev_pq + lcm + std_dev_classes, [14, len(mip_ub)]).T



    X_dis = data_discretization(training_data, 5)
    maxNumSelFeatures = 8

    for classifer in Classifers:
        if classifer == 'KNN':
            clf = neighbors.KNeighborsClassifier(n_neighbors)
        elif classifer == 'SVM':
            # clf = svm.SVC(kernel='rbf', C=1, class_weight='balanced')
            clf = svm.SVC(kernel='rbf', C=1000)
        elif classifer == 'DT':
            clf = tree.DecisionTreeClassifier(max_depth=4)
        else:
            assert (False)


        filename = "plot_data/%s_feature_selection.csv" % (classifer)
        results = []
        results.append('Algorithm Pairs')
        for fsmethod in FeatSelMethods:
            results.append(fsmethod)
        fileSaving(filename, results, 'w')

        for algorithm_pair in algorithm_pairs:
            gap1 = []
            gap2 = []
            if algorithm_pair == 'MIP vs LNS':
                for i in range(0, len(mip_gap_to_best_lb)):
                    gap1.append(min(mip_gap_to_best_lb[i], lazy_gap_to_best_lb[i]))
                    gap2.append(min(ten_gap_to_best_lb[i], lcm_gap_to_best_lb[i]))
            elif algorithm_pair == 'Plain MIP vs Lazy MIP':
                gap1 = mip_gap_to_best_lb
                gap2 = lazy_gap_to_best_lb
            elif algorithm_pair == 'LCM-LNS vs 10-LNS':
                gap1 = lcm_gap_to_best_lb
                gap2 = ten_gap_to_best_lb
            elif algorithm_pair == 'Plain MIP vs LCM-LNS':
                gap1 = mip_gap_to_best_lb
                gap2 = lcm_gap_to_best_lb
            elif algorithm_pair == 'Plain MIP vs 10-LNS':
                gap1 = mip_gap_to_best_lb
                gap2 = ten_gap_to_best_lb
            elif algorithm_pair == 'Lazy MIP vs LCM-LNS':
                gap1 = lazy_gap_to_best_lb
                gap2 = lcm_gap_to_best_lb
            elif algorithm_pair == 'Lazy MIP vs 10-LNS':
                gap1 = lazy_gap_to_best_lb
                gap2 = ten_gap_to_best_lb

            label = []
            # three classes
            for i in range(0, len(gap1)):
                if gap1[i] - gap2[i] > 0.01:
                    label.append(1)
                elif gap1[i] - gap2[i] < - 0.01:
                    label.append(-1)
                else:
                    label.append(0)
            # two classes
            # for i in range(0, len(gap1)):
            #     if gap1[i] - gap2[i] > 0:
            #         label.append(1)
            #     else:
            #         label.append(0)
            # label = np.array(label)

            results = []
            results.append(algorithm_pair)
            for fsMethod in FeatSelMethods:
                featSelected = []
                if fsMethod == 'MIM':
                    featSelected = mim(X_dis, label, n_selected_features=maxNumSelFeatures)
                elif fsMethod == 'MRMR':
                    featSelected = mrmr(X_dis, label, n_selected_features=maxNumSelFeatures)
                elif fsMethod == 'JMI':
                    featSelected = jmi(X_dis, label, n_selected_features=maxNumSelFeatures)
                elif fsMethod == 'CMIM':
                    featSelected = cmim(X_dis, label, n_selected_features=maxNumSelFeatures)
                elif fsMethod == 'ALL':
                    featSelected = list(range(0,14))
                print(featSelected)

                aveScoresPerRun = []
                for run in range(maxRun):
                    k = random.randint(0,10000)
                    cv = ShuffleSplit(n_splits=10, test_size = 0.1, random_state = k)
                    scores = cross_val_score(clf, training_data[:, featSelected], label, cv=cv)
                    aveScoresPerRun.append(scores.mean())
                print(aveScoresPerRun)
                print(sum(aveScoresPerRun)/len(aveScoresPerRun))
                results.append(sum(aveScoresPerRun)/len(aveScoresPerRun))
            print(results)
            fileSaving(filename, results, 'a')
