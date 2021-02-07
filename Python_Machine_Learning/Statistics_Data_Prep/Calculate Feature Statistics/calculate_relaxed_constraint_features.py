import statistics
import csv
import os
from shutil import copyfile
from pathlib import Path



# This script calculates the necessary features for the relaxed constraint statistics

#calculate the min,max,average and stddev of a list of numbers given
def calculateStats(data_list):

    min_val, max_val, ave, stddev = 0.0, 0.0, 0.0, 0.0

    data_list_float = [float(i) for i in data_list]
    min_val = min(data_list_float)
    max_val = max(data_list_float)
    ave = statistics.mean(data_list_float)
    stddev = statistics.pstdev(data_list_float)
    return(min_val,max_val,ave,stddev)

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
                    #min
                    stats_row.append(stats_tuple[0])
                    #max
                    stats_row.append(stats_tuple[1])
                    #ave
                    stats_row.append(stats_tuple[2])
                    #stddev
                    stats_row.append(stats_tuple[3])
                    writer.writerow(stats_row)

def main():


    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"
    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Features_Calculated"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            features_calculated_output_path = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/Features_Calculated" + "/" + "Relaxed_Constraint_Statistics"
            #create output folders if necessary
            Path(features_calculated_output_path).mkdir(parents=True, exist_ok=True)
            normalised_data_input_folder =  processed_results_folder + "/" + problem_type + "/" + instance_name + "/Normalised_Data" + "/" + "Relaxed_Constraint_Statistics"
            # for each relaxed constraint file, calculate and output the features
            for filename in os.listdir(normalised_data_input_folder):
                if filename != "single_stats.csv":
                    writeAllStats(normalised_data_input_folder + "/" + filename, features_calculated_output_path + "/" + filename)

            # single stats file doesn't require any manipulation, just copy the files across as these are already features
            copyfile(normalised_data_input_folder + "/" + "single_stats.csv", features_calculated_output_path + "/" + "single_stats.csv")
            print("Finished " + instance_name)

if __name__ == "__main__":

    main()