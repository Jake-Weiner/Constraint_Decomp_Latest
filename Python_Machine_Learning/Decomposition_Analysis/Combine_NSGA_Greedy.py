import csv
import os
from pathlib import Path
import shutil
#for each problem type and test instance, combine the different decompositions and count how many different decompositions exist from NSGA and Greedy

def empty_line_test(line):

    ret_val = False
    if line == "\n":
        ret_val = True

    return ret_val

def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    output_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    decomp_filename = "duplicates_redund_duplicates_removed.csv"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist

        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # Greedy Decomp File
            combined_csv = output_root_folder + "/" + problem_type + "/" + instance_name + "/" + "decompositions/Combined/combined_decomps.csv"
            greedy_decomp_file = output_root_folder + "/" + problem_type + "/" + instance_name + "/" + "decompositions/Greedy" + "/" + instance_name + "_" + decomp_filename
            NSGA_decomp_file = output_root_folder + "/" + problem_type + "/" + instance_name + "/" + "decompositions/NSGA" + "/" + instance_name + "_" + decomp_filename
            # location of combined csv
            stats_csv = output_root_folder + "/" + problem_type + "/" + instance_name + "/" + "decompositions/Combined/decomp_stats.csv"
            with open(stats_csv, "w") as stats_output_fs:
                with open(combined_csv, "w") as output_file:
                    with open(greedy_decomp_file, "r") as input_file1:
                        decomp_count = 0
                        for row in input_file1:
                            if not empty_line_test(row):
                                output_file.write(row)
                                decomp_count += 1
                            else:
                                print("empty greedy decomp found")
                        stats_output_fs.write("Greedy, " + str(decomp_count) + "\n")
                    with open(NSGA_decomp_file, "r") as input_file2:
                        decomp_count = 0
                        for row in input_file2:
                            if not empty_line_test(row):
                                output_file.write(row)
                                decomp_count += 1
                            else:
                                print("empty NSGA decomp found")
                        stats_output_fs.write("NSGA, " + str(decomp_count) + "\n")


if __name__ == "__main__":

    main()
