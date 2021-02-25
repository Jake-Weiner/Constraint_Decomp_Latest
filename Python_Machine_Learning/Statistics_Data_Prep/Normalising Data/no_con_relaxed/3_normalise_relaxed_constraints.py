import csv
from typing import NamedTuple
import collections
from pathlib import Path
import sys
#local imports
sys.path.append("/home/jake/PycharmProjects/Decomposition_Machine_Learning/Statistics_Data_Prep/Normalising Data")
import Relaxed_Constraint_Normalisation_Functions as RCNF
def main():

    # list of features requiring normalisation
    relaxed_constraints_min_max_scaling_filenames = ["Largest_RHSLHS.csv", "Sum_obj.csv", "Sum_abs_obj.csv",
                                                     "RHS_vals.csv"]

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps",  "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    raw_no_con_rel_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Index_Updated"
    processed_no_con_rel_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Processed_Results"
    decomp_processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):

            instance_statistics_filepath = decomp_processed_results_folder + "/" + problem_type + "/" + instance_name + "/" "Normalised_Data" + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv"

            # get instance values required to normalise relaxed constraint statistics
            Requirements = RCNF.getRequiredNormVals(instance_statistics_filepath)
            # store min max of required norm vals in order
            relaxed_constraints_normalisation_min_max = [(Requirements.min_rhslhs, Requirements.max_rhslhs),
                                                         (Requirements.min_sum_obj, Requirements.max_sum_obj)
                , (Requirements.min_sum_abs_obj, Requirements.max_sum_abs_obj),
                                                         (Requirements.min_rhs, Requirements.max_rhs)]

            for idx, relaxed_constraints_filename in enumerate(relaxed_constraints_min_max_scaling_filenames):
                # create output folders if they don't already exists

                input_raw_path = raw_no_con_rel_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Relaxed_Constraint_Statistics"
                output_normalised_path = processed_no_con_rel_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics"
                Path(output_normalised_path).mkdir(parents=True, exist_ok=True)

                RCNF.normaliseMinMax(relaxed_constraints_normalisation_min_max[idx][0],
                                relaxed_constraints_normalisation_min_max[idx][1]
                                , input_raw_path + "/" + relaxed_constraints_filename,
                                output_normalised_path + "/" + relaxed_constraints_filename)

            #non zero count scaling is done as a percentage of total non zeroes of the instance
            RCNF.normaliseCount(Requirements.num_non_zeroes, input_raw_path + "/" + "Non_zero_counts.csv", output_normalised_path + "/" + "Non_zero_counts.csv")

            print("Finished " + instance_name)
if __name__ == "__main__":

    main()