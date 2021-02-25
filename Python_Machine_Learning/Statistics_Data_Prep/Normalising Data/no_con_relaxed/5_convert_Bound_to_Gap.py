import sys
#local imports
sys.path.append("/home/jake/PycharmProjects/Decomposition_Machine_Learning/Statistics_Data_Prep/Normalising Data")
import csv
from typing import NamedTuple
import collections
from pathlib import Path
import Convert_Bound_To_Gap_Functions as CBTGF

def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps",  "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    raw_no_con_rel_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Index_Updated"
    processed_no_rel_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Processed_Results"
    best_known_solutions_path = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Best_Known_Solutions.csv"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):

            #convert Bounds to gap percentage
            Path(
                processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs").mkdir(
                parents=True, exist_ok=True)
            # Convert LR bounds to gap percentages
            # LR Bounds for no con relaxed
            LR_no_con_rel_input_path = raw_no_con_rel_folder + "/" + problem_type + "/" + instance_name + "/LROutputs" + "/" + "LR_outputs.csv"
            LR_no_con_rel_output_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_gaps.csv"
            CBTGF.convertBoundsToGap(best_known_solutions_path, LR_no_con_rel_input_path, LR_no_con_rel_output_path, instance_name)

if __name__ == "__main__":
    main()