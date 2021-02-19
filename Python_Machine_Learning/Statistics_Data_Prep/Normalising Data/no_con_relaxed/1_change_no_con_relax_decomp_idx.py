

#for each file, need to append data (2nd row ) to raw data and change decomp indx


#This script copies across all files which do not require any normalisation
import os
import sys
from shutil import copyfile
from pathlib import Path
import csv
import os
import linecache

def append_no_con_rel(no_con_file, decomp_file):

    # open up bin and int csvs to combine
    # store values of bin, then add to these the int values

    # count rows
    row_count = 0
    with open(decomp_file, "r") as decomp_input_fs:
        for line in decomp_input_fs:
            row_count += 1


    largest_decomp_idx = row_count - 2

    new_no_con_decomp_idx = largest_decomp_idx + 1
    separator = ","
    with open(no_con_file,"w") as no_con_input_fs:
        csvreader = csv.reader(no_con_input_fs, delimiter=separator)
        for line_number, split_line in enumerate(csvreader):
            if line_number == 1:
                print(split_line)
                print(separator.join(split_line))

    with open(no_con_file,"r") as no_con_input_fs:
        for line_number, split_line in enumerate(no_con_input_fs):
            if line_number == 1:
                print(split_line)


    print(new_no_con_decomp_idx)

    # # store total number of lines in input_file
    # total_lines = 0
    # with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
    #     total_lines = sum(1 for row in bin_prop_input_fs)
    #
    # with open(output_root_folder + "/" + "Bin_Int_Combined.csv", "w") as bin_int_combined_output_fs:
    #     bin_int_csv_writer = csv.writer(bin_int_combined_output_fs)
    #
    #     # copy the first line across, containing column names
    #     with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
    #         for line in bin_prop_input_fs:
    #             bin_int_combined_output_fs.write(line)
    #             break
    #
    #     # lines in the file are indexed from 1. Ignore the first row as these are just column header names
    #     current_row_number = 2
    #     while current_row_number < total_lines + 1:
    #         bin_line = linecache.getline(input_root_folder + "/" + "Bin_props.csv", current_row_number)
    #         bin_line_split = bin_line.split(",")
    #         bin_props = [float(element) for element in bin_line_split[1:]]
    #         int_line = linecache.getline(input_root_folder + "/" + "Int_props.csv", current_row_number)
    #         int_line_split = int_line.split(",")
    #         int_props = [float(element) for element in int_line_split[1:]]
    #
    #         #zip function combines two different list
    #         int_bin_combined_props = [bin_prop + int_prop for int_prop, bin_prop in zip(bin_props, int_props)]
    #         int_bin_line = [current_row_number - 2] + int_bin_combined_props
    #         bin_int_csv_writer.writerow(int_bin_line)
    #         current_row_number += 1


#get the largest decomp index of the supplied file. Decomp index is 2 less than the number of rows (1st row are column headers)
def getLargestDecompIndex(input_file):
    # count rows
    row_count = 0
    with open(input_file, "r") as decomp_input_fs:
        for line in decomp_input_fs:
            row_count += 1

    largest_decomp_idx = row_count - 2
    return largest_decomp_idx

def main():


    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]


    raw_decomp_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    raw_no_con_relax_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed"
    no_con_relax_index_updated_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Index_Updated"
    separator = ","

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            #for the given problem type and instance name, get the number of decompositions
            raw_LR_decomp_path = raw_decomp_results_folder + "/" + problem_type + "/" + instance_name + "/" + "LROutputs/LR_outputs.csv"
            largest_decomp_idx = getLargestDecompIndex(raw_LR_decomp_path)
            new_no_con_relax_decomp_idx = largest_decomp_idx + 1
            current_instance_raw_path = raw_no_con_relax_root_folder + "/" + problem_type + "/" + instance_name
            for folder in os.listdir(current_instance_raw_path):
                #create output path
                new_output_folder_path = no_con_relax_index_updated_root_folder + "/" + problem_type + "/" + instance_name + "/" + folder
                Path(new_output_folder_path).mkdir(parents=True, exist_ok=True)
                for filename in os.listdir(current_instance_raw_path + "/" + folder):
                    with open(new_output_folder_path + "/" + filename, "w") as output_fs:
                        with open(current_instance_raw_path + "/" + folder + "/" + filename, "r") as current_raw_no_con_relax_input_fs:
                            csvreader = csv.reader(current_raw_no_con_relax_input_fs, delimiter=separator)
                            for line_number, split_line in enumerate(csvreader):
                                if line_number == 1:
                                    split_line[0] = str(new_no_con_relax_decomp_idx)
                                output_fs.write(separator.join(split_line) + "\n")
            print("Finished " + instance_name)
if __name__ == "__main__":

    main()