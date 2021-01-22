import csv
# extract the LP bound contained within the file
from pathlib import Path


def getLPBound(LP_input_filepath):

    lp_bound_col_idx = 0
    with open(LP_input_filepath, "r") as lp_input_fs:
        lp_csvreader = csv.reader(lp_input_fs, delimiter=",")
        lp_bound = -999999999999
        for line_number, line_split in enumerate(lp_csvreader):
            if line_number == 1:
                lp_bound = line_split[lp_bound_col_idx]

    return float(lp_bound)

# extract the best known bound associated with the instance name
def getBestKnownSol(input_file_path, instance_name):

    instance_name_col_idx = 0
    best_sol_col_idx = 1
    instance_found_flag = False
    with open(input_file_path, "r") as best_sol_input_fs:
        best_sol_csvreader = csv.reader(best_sol_input_fs, delimiter=",")
        best_sol = -999999999999
        for line_number, line_split in enumerate(best_sol_csvreader):
            if line_split[instance_name_col_idx] == instance_name:
                best_sol = float(line_split[best_sol_col_idx])
                instance_found_flag = True
                break

    if not instance_found_flag:
        print("Error, Best sol for " + instance_name + " is unable to be located ")
        exit(-1)
    return best_sol

#this function will write the decomposition index, filename and LR bounds for decompositions in which the LP bounds are better
# than the LR bounds
def checkLRBounds(output_root_path, LR_path, instance_name, LP_bound, best_sol):
    decomposition_number_col_idx = 0
    lr_bound_col_idx = 1
    with open(output_root_path + "/lp_bounds_compare.csv", "a+") as output_lp_compare_fs, open(output_root_path + "/best_sol_compare.csv", "a+") as output_best_sol_compare_fs:
        with open(LR_path, "r") as lr_input_fs:
            lr_csvreader = csv.reader(lr_input_fs, delimiter=",")
            for line_number, line_split in enumerate(lr_csvreader):
                if line_number > 0:
                    LR_bound = float(line_split[lr_bound_col_idx])
                    #for minimisation problem, the higher the bound, the better
                    if LP_bound > LR_bound:
                        decomp_number = line_split[decomposition_number_col_idx]
                        output_lp_compare_fs.write(instance_name + "," + str(decomp_number) + "," + str(LR_bound) + "," + str(LP_bound) + "\n")
                    if LR_bound > best_sol:
                        decomp_number = line_split[decomposition_number_col_idx]
                        output_best_sol_compare_fs.write(instance_name + "," + str(decomp_number) + "," + str(LR_bound) + "," + str(best_sol) + "\n")



def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"], ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps"
                        , "snp-10-004-052.mps","snp-10-052-052.mps"]]

    raw_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    # compare the LR Results with the LP bound found.
    output_root_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/error_checking/Bounds_Checks"
    Path(output_root_path).mkdir(parents=True, exist_ok=True)
    best_sol_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Best_Known_Solutions.csv"

    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            LP_filepath =  raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + "LP_Bound/LP_outputs.csv"
            LR_filepath =  raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + "LROutputs/LR_outputs.csv"
            lp_bound = getLPBound(LP_filepath)
            best_sol = getBestKnownSol(best_sol_path, instance_name)
            checkLRBounds(output_root_path, LR_filepath, instance_name, lp_bound, best_sol)

if __name__ == "__main__":

    main()