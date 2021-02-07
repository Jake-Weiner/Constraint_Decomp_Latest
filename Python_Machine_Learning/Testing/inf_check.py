#this script will check for inf values contained in any files

#This script tests which instances may have been run, then stopped, then run again appending decompositions
# to the original run. It them removes the first set of decompositions (ASSUMING ONLY 1 additional run)
import pandas as pd
import csv
import os


def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps"
                          , "snp-10-004-052.mps", "snp-10-052-052.mps"]]

    raw_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    folders_to_check = ["LROutputs", "Relaxed_Constraint_Statistics", "Subproblem_Statistics"]

    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            for folder in folders_to_check:
                folder_path = raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + folder
                for filename in os.listdir(folder_path):
                    with open(folder_path + "/" + filename,  "r") as input_fs:
                        inf_count = 0
                        for line in input_fs:
                            if "inf" in line:
                                if "LP_bounds.csv" in filename:
                                    if inf_count == 0:
                                        print("inf found in " + filename + " in " + folder)
                                    inf_count += 1
                                else:
                                    print("inf found in " + filename + " in " + folder)
                                    break
                        if "LP_bounds.csv" in filename and inf_count > 0:
                            print("Inf count in " + filename + " is " + str(inf_count))
            print("Finished " + instance_name)

if __name__ == "__main__":

    main()