

# go through each LR value and make sure it lies between LP and best bound +/- 1%

import sys
import csv
from typing import NamedTuple
import collections
from pathlib import Path

def getBestKnownSol(best_known_solutions_path, instance_name):
    instance_name_col_idx = 0
    instance_sol_col_idx = 1
    best_known_sol = -999999999999999

    with open(best_known_solutions_path, "r") as best_known_sol_fs:
        csvreader = csv.reader(best_known_sol_fs, delimiter=',')
        best_known_sol_found = False
        for line_number, line_split in enumerate(csvreader):
            if line_split[instance_name_col_idx] == instance_name:
                best_known_sol_found = True
                best_known_sol = float(line_split[instance_sol_col_idx])

        if best_known_sol_found == False:
            print("Error: Unable to find best solution val for instance " + instance_name)
            exit(-1)

    return best_known_sol

def getLPSol(LP_input_path):

    instance_sol_col_idx = 0
    lp_bounds = -999999999999999

    with open(LP_input_path, "r") as lp_fs:
        csvreader = csv.reader(lp_fs, delimiter=',')
        best_known_sol_found = False
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                lp_bounds = float(line_split[instance_sol_col_idx])


    return lp_bounds


#convert the LR bounds to gap percentages based on best known solution
def checkLRValues(best_sol, LP_val,  LR_input_path, instance_name):

        with open(LR_input_path, "r") as LR_Bound_input_fs:
            csvreader = csv.reader(LR_Bound_input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number > 0:
                    for col_idx, value in enumerate(line_split):
                        # first column is just decomposition index
                        if col_idx == 1:
                            LR_val = float(value)
                            if LR_val < LP_val:
                                print("LR val of {} is less than LP val of {} in instance {}".format(LR_val, LP_val, instance_name))
                            if LR_val > 1.01 * best_sol:
                                print("LR val of {} is greater than best bound val of {} in instance {}".format(LR_val, LP_val,
                                                                                                     instance_name))

def main():

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    # problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    #
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps",  "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]
    raw_data_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    best_known_solutions_path = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results/Best_Known_Solutions.csv"
    LP_path = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs/random_MIPLIB/30n20b8.mps/LP_Bound"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            LP_path = raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/LP_Bound" + "/" + "LP_outputs.csv"
            LP_val = getLPSol(LP_path)
            best_sol = getBestKnownSol(best_known_solutions_path, instance_name)
            LR_input_path = raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/LROutputs" + "/" + "LR_outputs.csv"
            checkLRValues(best_sol, LP_val, LR_input_path, instance_name)

if __name__ == "__main__":
    main()