import statistics
import csv
import os

from pathlib import Path


def calculateStats(data_list):
    min_val, max_val, ave, stddev = 0.0, 0.0, 0.0, 0.0
    data_list_float = [float(i) for i in data_list]
    min_val = min(data_list_float)
    max_val = max(data_list_float)
    ave = statistics.mean(data_list_float)
    stddev = statistics.pstdev(data_list_float)

    return(min_val,max_val,ave,stddev)

def writeAveStddev(input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Average", "Stddev"])
                else:
                    stats_row = []
                    # calculate ave,stddev of row
                    ave_stddev_tuple = calculateStats(line_split[1:])
                    stats_row.append(line_split[0])
                    stats_row.append(ave_stddev_tuple[2])
                    stats_row.append(ave_stddev_tuple[3])
                    writer.writerow(stats_row)

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

    # list of features which we want to get only average and stddev. Every other
    # we also want to get min and max
    subproblem_ave_stddev = ["Sum_obj.csv", "Sum_abs_obj.csv", "Obj_range.csv", "average_RHS.csv",
                             "average_abs_RHS.csv", "Largest_RHSLHS.csv", "RHS_range.csv", "Shapes.csv"]

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    external_processed_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            features_calculated_output_path = features_calculated_output_folder + "/" + problem_type + "/" + instance_name + "/Features_Calculated" + "/" + "Subproblem_Statistics"
            # create output folders if necessary
            Path(features_calculated_output_path).mkdir(parents=True, exist_ok=True)
            normalised_data_input_folder = external_processed_results_folder + "/" + problem_type + "/" + instance_name + "/Normalised_Data" + "/" + "Subproblem_Statistics"
            for filename in subproblem_ave_stddev:
                writeAveStddev(normalised_data_input_folder + "/" + filename, features_calculated_output_path + "/" + filename)
                print("Finished " + filename)
            for filename in os.listdir(normalised_data_input_folder):
                if filename not in subproblem_ave_stddev:
                    writeAllStats(normalised_data_input_folder + "/" + filename, features_calculated_output_path + "/" + filename)
                    print("Finished " + filename)
            print("Finished " + instance_name)

if __name__ == "__main__":

    main()