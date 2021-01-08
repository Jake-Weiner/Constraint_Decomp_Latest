import csv
# extract the LP bound contained within the file
def getLPBound(LP_input_filepath):

    lp_bound_col_idx = 0
    with open(LP_input_filepath, "r") as lp_input_fs:
        lp_csvreader = csv.reader(lp_input_fs, delimiter=",")
        lp_bound = -999999999999
        for line_number, line_split in enumerate(lp_csvreader):
            if line_number == 1:
                lp_bound = line_split[lp_bound_col_idx]

    return float(lp_bound)

#this function will write the decomposition index, filename and LR bounds for decompositions in which the LP bounds are better
# than the LR bounds
def checkLRBoundsAgainstLP(output_path, LR_path, instance_name, LP_bound):

    decomposition_number_col_idx = 0
    lr_bound_col_idx = 1
    with open(output_path, "a+") as output_fs:
        with open(LR_path, "r") as lr_input_fs:
            lr_csvreader = csv.reader(lr_input_fs, delimiter=",")
            for line_number, line_split in enumerate(lr_csvreader):
                if line_number > 0:
                    LR_bound = float(line_split[lr_bound_col_idx])
                    #for minimisation problem, the higher the bound, the better
                    if LP_bound > LR_bound:
                        decomp_number = line_split[decomposition_number_col_idx]
                        output_fs.write(instance_name + "," + str(decomp_number) + "," + str(LR_bound) + "," + str(LP_bound) + "\n")

def main():
    problem_types = ["network_design", "fixed_cost_network_flow"]
    # , "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]
    #
    #
    # "ta2-UUE.mps"
    # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #  "snp-10-052-052.mps"]]
    # read in dataset


    raw_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"

    # compare the LR Results with the LP bound found.

    output_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/error_checking/LPBounds_vs_LRBounds/lpbounds_vs_lrbounds.csv"


    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            LP_filepath =  raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + "LP_Bound/LP_outputs.csv"
            LR_filepath =  raw_results_folder + "/" + problem_type + "/" + instance_name + "/" + "LROutputs/LR_outputs.csv"
            lp_bound = getLPBound(LP_filepath)
            checkLRBoundsAgainstLP(output_path, LR_filepath, instance_name, lp_bound)


if __name__ == "__main__":

    main()