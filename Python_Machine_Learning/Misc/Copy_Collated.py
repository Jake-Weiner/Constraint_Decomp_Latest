#this script copies all collated data into a single folder in order to transfer it to the Max-Process server

#This script copies across all files which do not require any normalisation
from shutil import copyfile
from pathlib import Path
import csv
import linecache


collated_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "random_MIPLIB"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps",  "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"],
            ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
             "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]



    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # copy across the subproblem files

            copyfile("{}/{}/{}/Features_Collated/collated.csv".format(collated_root_folder,problem_type,instance_name),
                    "{}/Collated/collated_{}.csv".format(collated_root_folder,instance_name))

            print("Finished " + instance_name)
if __name__ == "__main__":

    main()