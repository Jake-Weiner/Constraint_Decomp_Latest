#This script copies across all files which do not require any normalisation
import os
import sys
from shutil import copyfile
from pathlib import Path
import csv
import linecache

def combine_bin_int_stats(input_root_folder, output_root_folder):

    # open up bin and int csvs to combine
    # store values of bin, then add to these the int values

    # store total number of lines in input_file
    total_lines = 0
    with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
        total_lines = sum(1 for row in bin_prop_input_fs)

    with open(output_root_folder + "/" + "Bin_Int_Combined.csv", "w") as bin_int_combined_output_fs:
        bin_int_csv_writer = csv.writer(bin_int_combined_output_fs)

        # copy the first line across, containing column names
        with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
            for line in bin_prop_input_fs:
                bin_int_combined_output_fs.write(line)
                break

        # lines in the file are indexed from 1. Ignore the first row as these are just column header names
        current_row_number = 2
        while current_row_number < total_lines + 1:
            bin_line = linecache.getline(input_root_folder + "/" + "Bin_props.csv", current_row_number)
            bin_line_split = bin_line.split(",")
            bin_props = [float(element) for element in bin_line_split[1:]]
            int_line = linecache.getline(input_root_folder + "/" + "Int_props.csv", current_row_number)
            int_line_split = int_line.split(",")
            int_props = [float(element) for element in int_line_split[1:]]

            #zip function combines two different list
            int_bin_combined_props = [bin_prop + int_prop for int_prop, bin_prop in zip(bin_props, int_props)]
            int_bin_line = [current_row_number - 2] + int_bin_combined_props
            bin_int_csv_writer.writerow(int_bin_line)
            current_row_number += 1


def main():


    # problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]


    problem_types = ["random_MIPLIB"]
    instance_names = [["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps", "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]


    subproblem_filenames = ["Bin_props.csv", "Const_props.csv", "Cont_props.csv", "Densities.csv", "Equality_props.csv",
                            "Int_props.csv", "Var_props.csv"]
    relaxed_constraint_filenames = ["single_stats.csv", "Bin_props.csv", "Int_props.csv", "Cont_props.csv"]

    raw_data_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"

    statistics_types_folders = ["Relaxed_Constraint_Statistics", "Subproblem_Statistics"]

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            for subproblem_filename in subproblem_filenames:
                # create output folders if they don't already exists
                Path(
                    processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics").mkdir(
                    parents=True, exist_ok=True)
                relative_path = problem_type + "/" + instance_name +  "/Subproblem_Statistics" + "/" + subproblem_filename
                copyfile(raw_data_root_folder + "/" + problem_type + "/" + instance_name +  "/Subproblem_Statistics" + "/" + subproblem_filename,
                         processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics" + "/" + subproblem_filename)

            for relaxed_constraint_filename in relaxed_constraint_filenames:
                # create output folders if they don't already exists
                Path(
                    processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics").mkdir(
                    parents=True, exist_ok=True)

                copyfile(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/Relaxed_Constraint_Statistics" + "/" + relaxed_constraint_filename,
                         processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/Relaxed_Constraint_Statistics" + "/" + relaxed_constraint_filename)

            # instance Statistics
            Path(processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Instance_Statistics").mkdir(
                parents=True, exist_ok=True)

            copyfile(raw_data_root_folder + "/" +  problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv",
                     processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv")

            # for both the relaxed constraint statistics and the subproblem statistics, combine int and bin variables
            for folder in statistics_types_folders:
                combine_bin_int_stats(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + folder, processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + folder)

            print("Finished " + instance_name)
if __name__ == "__main__":

    main()