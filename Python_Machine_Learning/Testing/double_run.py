#This script tests which instances may have been run, then stopped, then run again appending decompositions
# to the original run. It them removes the first set of decompositions (ASSUMING ONLY 1 additional run)
import pandas as pd
import csv
import os

def re_write_file(input_root_path, filename):
    with open(input_root_path + "/tmp.csv", "w") as output_fs, open(input_root_path + "/" + filename, "r") as input_fs:
        # csvreader = csv.reader(input_fs, delimiter = ",")
        zero_decomp_idxs_found = 0
        for line_number, line in enumerate(input_fs):
            line_split = line.split(",")
            #copy across header
            if line_number == 0:
                # print(line_split)
                output_fs.write(line)
            else:
                if (line_split[0] == "0"):
                    zero_decomp_idxs_found += 1
                if (zero_decomp_idxs_found) > 1:
                    output_fs.write(line)

    os.remove(input_root_path + "/" + filename)
    print(filename + " deleted")
    os.rename(input_root_path + "/tmp.csv", input_root_path + "/" + filename)
    print("tmp.csv renamed to " + filename)


#this will re-write all statistics files to remove that initial run data. Delete everything up until the 2nd 0 decomp
#idx
def delete_initial_run(input_root_folder):

    folders_to_delete_initial_run = ["LROutputs", "Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    for folder in folders_to_delete_initial_run:
        for filename in os.listdir(input_root_folder + "/" + folder):
            if ".tmp" not in filename:
                re_write_file(input_root_folder + "/" + folder, filename)

def main():

    #Re-write output files to remove additional run
    re_write_output = False

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps"
                          , "snp-10-004-052.mps", "snp-10-052-052.mps"]]

    # problem_types = ["supply_network_planning"]
    # instance_names = ["snp-10-052-052.mps"]

    raw_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    folders_to_delete_initial_run = ["LROutputs", "Relaxed_Constraint_Statistics", "Subproblem_Statistics"]
    ## check LR output folder
    ## check relaxed constraints folder
    ## check subproblem _statistics folder
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            for folder in folders_to_delete_initial_run:
                folder_path = raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + folder
                for filename in os.listdir(folder_path):
                    if "tmp.csv" not in filename:
                        with open(folder_path + "/" + filename,  "r") as input_fs:
                            decomp_idx_list = []
                            input_csvreader = csv.reader(input_fs, delimiter=",")
                            for line_num, line_split in enumerate(input_csvreader):
                                if line_num > 0:
                                    decomp_idx_list.append(line_split[0])
                            if len(decomp_idx_list) != len(set(decomp_idx_list)):
                                print("reruns found in instance " + instance_name)
                                print("difference in length is " + str(len(decomp_idx_list)) + " and " + str(len(set(decomp_idx_list))))
                                if write_output_flag:
                                    re_write_file(folder_path, filename)
            print("Finished " + instance_name)

if __name__ == "__main__":


    main()