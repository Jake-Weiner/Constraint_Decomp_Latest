#This script
import os
import sys
from shutil import copyfile
from pathlib import Path

def main():


    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]
    subproblem_filenames = ["Bin_props.csv", "Const_props.csv", "Cont_props.csv", "Densities.csv", "Equality_props.csv",
                            "Int_props.csv", "Var_props.csv"]
    relaxed_constraint_filenames = ["single_stats.csv", "Bin_props.csv", "Int_props.csv", "Cont_props.csv"]


    raw_data_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"



    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            for subproblem_filename in subproblem_filenames:
                # create output folders if they don't already exists
                Path(
                    processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics").mkdir(
                    parents=True, exist_ok=True)
                relative_path = problem_type + "/" + instance_name +  "/Subproblem_Statistics" + "/" + subproblem_filename
                copyfile(raw_data_root_folder + "/" + relative_path,
                         processed_results_folder + "/" + "/" + relative_path)

            for relaxed_constraint_filename in relaxed_constraint_filenames:
                # create output folders if they don't already exists
                Path(
                    processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics").mkdir(
                    parents=True, exist_ok=True)
                relative_path = problem_type + "/" + instance_name + "/Relaxed_Constraint_Statistics" + "/" + relaxed_constraint_filename
                copyfile(raw_data_root_folder + "/" + relative_path,
                         processed_results_folder + "/" + "/" + relative_path)

            # instance Statistics
            Path(processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Instance_Statistics").mkdir(
                parents=True, exist_ok=True)

            copyfile(raw_data_root_folder + "/" +  problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv",
                     processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv")

if __name__ == "__main__":

    main()