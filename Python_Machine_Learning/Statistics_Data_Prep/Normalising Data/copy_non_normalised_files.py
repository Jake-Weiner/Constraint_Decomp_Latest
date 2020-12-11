#This script
import os
import sys
from shutil import copyfile

def main():

    input_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Raw_Data"
    output_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Normalised_Data"
    subproblem_filenames = ["Bin_props.csv", "Const_props.csv", "Cont_props.csv", "Densities.csv", "Equality_props.csv", "Int_props.csv", "Var_props.csv"]
    relaxed_constraint_filenames = ["single_stats.csv", "Bin_props.csv", "Int_props.csv", "Cont_props.csv"]
    LR_output_filenames = ["LR_outputs.csv"]

    # copy subproblem files
    for filename in subproblem_filenames:
        copyfile(input_root_folder + "/Subproblem_Statistics" + "/" + filename, output_root_folder + "/Subproblem_Statistics" + "/" + filename)

    # copy Relaxed Constraint files
    for filename in relaxed_constraint_filenames:
        copyfile(input_root_folder + "/Relaxed_Constraint_Statistics" + "/" + filename,
                 output_root_folder + "/Relaxed_Constraint_Statistics" + "/" + filename)

    # copy LR Outputs files
    for filename in LR_output_filenames:
        copyfile(input_root_folder + "/LROutputs" + "/" + filename,
                 output_root_folder + "/LROutputs" + "/" + filename)
    #
    # # copy LP Bounds files
    # for filename in relaxed_constraint_filenames:
    #     copyfile(input_root_folder + "/LP_Bound" + "/" + filename,
    #              output_root_folder + "/LP_Bound" + "/" + filename)

    # instance Statistics
    copyfile(input_root_folder + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv",
             output_root_folder + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv")

if __name__ == "__main__":

    main()