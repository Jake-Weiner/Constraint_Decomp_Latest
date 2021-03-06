import statistics
import csv
import os
from shutil import copyfile
from pathlib import Path
import pandas as pd
import collections
import math

# This script calculates the necessary features for the relaxed constraint statistics
#get instance statistics which are required to normalise the relaxed constraint statistics
def getInstanceNonZeroes(instance_characteristics_path):

    non_zeroes = 0
    non_zeroes_idx = 5
    with open(instance_characteristics_path, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                non_zeroes = int(line_split[non_zeroes_idx])
    return non_zeroes

def getInstanceNumVarsConstr(instance_characteristics_path):
    num_vars = 0
    num_constr = 0
    num_var_idx = 0
    num_constr_idx = 1
    with open(instance_characteristics_path, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                num_vars = int(line_split[num_var_idx])
                num_constr = int(line_split[num_constr_idx])

    return num_vars, num_constr

# calculate the min,max,average and stddev of a list of numbers given
def calculateStats(data_list):
    min_val, max_val, ave, stddev = 0.0, 0.0, 0.0, 0.0
    if data_list:
        data_list_float = [float(i) for i in data_list]
        min_val = min(data_list_float)
        max_val = max(data_list_float)
        ave = statistics.mean(data_list_float)
        stddev = statistics.pstdev(data_list_float)
    return (min_val, max_val, ave, stddev)


# calculate the features for all relaxed constraint statistics
def writeAllStats(input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Min", "Max", "Average", "Stddev"])
                else:
                    stats_row = []
                    # calculate ave,stddev of row
                    stats_tuple = calculateStats(line_split[1:])
                    stats_row.append(line_split[0])
                    # min
                    stats_row.append(stats_tuple[0])
                    # max
                    stats_row.append(stats_tuple[1])
                    # ave
                    stats_row.append(stats_tuple[2])
                    # stddev
                    stats_row.append(stats_tuple[3])
                    writer.writerow(stats_row)

# def getDecompositionNonZeroes(rc_non_zero_counts_path, instance_non_zeroes):

def writeQScores(subproblem_non_zeroes_path, Q_values_output_folder):

    Path(Q_values_output_folder).mkdir(parents=True, exist_ok=True)
    with open(Q_values_output_folder + "/" + "Q_Scores.csv", "w") as Q_scores_outputs_fs:
        with open(subproblem_non_zeroes_path, "r") as non_zeroes_input_fs:
            #calculate the total non_zeroes in the subproblems
            non_zeroes_csv_reader = csv.reader(non_zeroes_input_fs, delimiter=",")
            for line_number, line_split in enumerate(non_zeroes_csv_reader):

                if line_number == 0:
                    Q_scores_outputs_fs.write("Decomposition Index, Q Scores" + "\n")
                else:
                    #calculate total number of non-zeroes in D
                    D_non_zeroes = 0
                    for sp_non_zero_count in line_split[1:]:
                        D_non_zeroes += float(sp_non_zero_count)
                    #calculate Q score
                    Q = 0.0
                    for sp_non_zero_count in line_split[1:]:
                        Q += (float(sp_non_zero_count) / D_non_zeroes) * (1 - (float(sp_non_zero_count) / D_non_zeroes))
                    decomp_index = int(line_split[0])
                    # write out Q score
                    Q_scores_outputs_fs.write(str(decomp_index) + "," + str(Q) + "\n")

        # with open(density_path, "r") as density_input_fs, open(var_prop_path, "r") as var_prop_fs, open(
        #         constr_prop_path, "r") as constr_prop_fs, open(D_non_zero_counts_sum_path, "r") as D_non_zero_sum_fs:
        #     density_csv_reader = csv.reader(density_input_fs, delimiter=",")
        #     var_prop_csv_reader = csv.reader(var_prop_fs, delimiter=",")
        #     constr_prop_csv_reader = csv.reader(constr_prop_fs, delimiter=",")
        #     D_non_zero_csv_reader = csv.reader(D_non_zero_sum_fs, delimiter=",")
        #     for line_number, (
        #     density_line_split, var_prop_line_split, constr_prop_line_split, D_non_zero_line_split) in enumerate(
        #             zip(density_csv_reader, var_prop_csv_reader, constr_prop_csv_reader, D_non_zero_csv_reader)):
        #         if line_number == 0:
        #             Q_scores_outputs_fs.write("Decomposition Index, Q Scores" + "\n")
        #         else:
        #             Q = 0.0
        #             constr_index = 1
        #             D_non_zeroes = int(D_non_zero_line_split[1])
        #             decomp_index = int(D_non_zero_line_split[0])
        #             for index in range(1, len(density_line_split)):
        #                 # print(float(density_line_split[index]) * float(var_prop_line_split[index]) * float(constr_prop_line_split[index]) * float(instance_num_var) * float(instance_num_constr))
        #                 # print(density_line_split[index])
        #                 # keep incrementing constr_index to find next subproblem with at least 1 constraint as some subproblems contain no constraints and therefore no density measurements
        #                 # if float(constr_prop_line_split[constr_index]) == 0:
        #                 #     while float(constr_prop_line_split[constr_index]) == 0:
        #                 #         constr_index += 1
        #                 # if abs((float(density_line_split[index]) * float(var_prop_line_split[index]) * float(constr_prop_line_split[index]) * float(instance_num_var) * float(instance_num_constr)) -
        #                 #        int(float(density_line_split[index]) * float(var_prop_line_split[index]) * float(constr_prop_line_split[index]) * float(instance_num_var) * float(instance_num_constr) + 0.1)) > 0.2:
        #                 #     print(float(density_line_split[index]) * float(var_prop_line_split[index]) * float(
        #                 #         constr_prop_line_split[index]) * float(instance_num_var) * float(
        #                 #         instance_num_constr))
        #                 #     print(float(density_line_split[index]))
        #                 # print(int(float(density_line_split[index]) * float(
        #                 #     var_prop_line_split[constr_index]) * float(
        #                 #     constr_prop_line_split[constr_index]) * float(instance_num_var) * float(
        #                 #     instance_num_constr) + 0.1))
        #                 subproblem_non_zeroes = int(float(density_line_split[index]) * float(
        #                     var_prop_line_split[constr_index]) * float(
        #                     constr_prop_line_split[constr_index]) * float(instance_num_var) * float(
        #                     instance_num_constr) + 0.1)
        #                 # if subproblem_non_zeroes < 0:
        #                 #     print("Density is {}".format(float(density_line_split[index])))
        #                 #     print("Var prop is {}".format(float(
        #                 #     var_prop_line_split[constr_index])))
        #                 #     print("Constr prop is {}".format(float(
        #                 #     constr_prop_line_split[constr_index])))
        #                 #     print("Instance Num Var is {}".format(float(instance_num_var)))
        #                 #     print("Instance Num Constr is {}".format(float(
        #                 #     instance_num_constr)))
        #                 #     print("Number of var * number of constr = {}".format(float(
        #                 #     var_prop_line_split[constr_index]) * float(
        #                 #     constr_prop_line_split[constr_index]) * float(
        #                 #     instance_num_var) * float(
        #                 #     instance_num_constr)))
        #                 #     print("Number of subproblem non zeroes is {}".format(subproblem_non_zeroes))
        #
        #                 Q += (subproblem_non_zeroes / D_non_zeroes) * (1 - (subproblem_non_zeroes / D_non_zeroes))
        #                 # print(float(var_prop_line_split[index]) * instance_num_var)
        #                 # print(float(constr_prop_line_split[index]) * instance_num_constr)
        #                 # print(var_prop_line_split[index])
        #                 # print(constr_prop_line_split[index])
        #                 constr_index += 1
        #                 # if index > 100:
        #                 #     exit(0)
        #             Q_scores_outputs_fs.write(str(decomp_index) + "," + str(Q) + "\n")

# Pvalue is e^(-lambda * (relaxed_const_prop))
def writePScores(relaxed_constraint_prop_path, P_values_output_folder):
    lambda_val = 5
    Path(P_values_output_folder).mkdir(parents=True, exist_ok=True)
    relaxed_constr_prop_col_idx = 1
    with open(P_values_output_folder + "/" + "P_Scores.csv", "w") as P_values_output_fs:
        with open(relaxed_constraint_prop_path, "r") as relaxed_constr_input_fs:
            relaxed_const_csvreader = csv.reader(relaxed_constr_input_fs, delimiter=",")
            for line_number, line_split in enumerate(relaxed_const_csvreader):
                if line_number == 0:
                    P_values_output_fs.write("Decomposition Index, P Value" + "\n")
                else:
                    P_value = math.exp(-1 * lambda_val * float(line_split[relaxed_constr_prop_col_idx]))
                    P_values_output_fs.write(line_split[0] + "," + str(P_value) + "\n")

def writeGoodnessScores(Q_P_scores_input_folder, Goodness_score_output_folder):
    with open(Goodness_score_output_folder + "/" + "Goodness_Scores.csv" , "w") as Goodness_scores_output_fs:
        #loop through each file and multiply Q and P scores
        with open(Q_P_scores_input_folder + "/" + "Q_Scores.csv", "r") as Q_scores_input_fs, open(Q_P_scores_input_folder + "/" + "P_Scores.csv", "r") as P_scores_input_fs:
            Q_Scores_csvreader = csv.reader(Q_scores_input_fs, delimiter=",")
            P_Scores_csvreader = csv.reader(P_scores_input_fs, delimiter=",")
            for line_number, (Q_line_split, P_line_split) in enumerate(zip(Q_Scores_csvreader, P_Scores_csvreader)):
                if line_number == 0:
                    Goodness_scores_output_fs.write("Decomposition Index,Goodness Scores" + "\n")
                else:
                    Goodness_score = float(Q_line_split[1]) * float(P_line_split[1])
                    decomp_index = Q_line_split[0]
                    Goodness_scores_output_fs.write(decomp_index + "," + str(Goodness_score) + "\n")


def main():
    problem_types = ["network_design", "fixed_cost_network_flow",  "supply_network_planning"]

    # instance_names_testing_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]


    instance_names_testing = [
        ["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
        ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
        ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
         "snp-10-052-052.mps"]]
    # Calculate Non zeroes Count in each
    # Sum up all non zero counts
    raw_decomps_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
    external_processed_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_idx]):

            #Ranking output fiolder
            decomp_scores_output_folder = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Decomp_Method_Scores"
            Path(decomp_scores_output_folder).mkdir(parents=True, exist_ok=True)
            # CALCULATE Q SCORES
            subproblem_non_zeroes_path = raw_decomps_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/" + "non_zeroes.csv"
            writeQScores(subproblem_non_zeroes_path, decomp_scores_output_folder)
            # CALCULATE P SCORES
            relaxed_constraint_prop_path = external_processed_results_folder + "/" + problem_type + "/" + instance_name + "/"  + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics" + "/" + "single_stats.csv"
            writePScores(relaxed_constraint_prop_path, decomp_scores_output_folder)


            writeGoodnessScores(decomp_scores_output_folder, decomp_scores_output_folder)

            print("Finished {}".format(instance_name))


if __name__ == "__main__":
    main()