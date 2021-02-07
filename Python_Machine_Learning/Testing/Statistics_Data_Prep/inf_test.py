import csv
from typing import NamedTuple
import collections
from pathlib import Path
import os


#this script will check all files to see if there are any inf values contained
def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                        "snp-10-052-052.mps"]]

    raw_data_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            for folder in os.listdir(raw_data_root_folder + "/" + problem_type + "/" + instance_name):
                for filename in os.listdir(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + folder):
                    with open(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + folder + "/" + filename, "r") as input_fs:
                        for line in input_fs:
                            if "inf" in line:
                                print("inf found in " + raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + folder + "/" + filename)
                                break
            print("Finished Processing " + instance_name)

if __name__ == "__main__":

    main()