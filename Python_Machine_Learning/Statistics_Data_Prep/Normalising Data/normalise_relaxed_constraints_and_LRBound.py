import csv
from typing import NamedTuple
import collections
from pathlib import Path

#get instance statistics which are required to normalise the relaxed constraint statistics
def getRequiredNormVals(input_filename):

    RCNormReq = collections.namedtuple('RCNormReq', 'num_non_zeroes min_rhs max_rhs min_rhslhs max_rhslhs min_sum_obj max_sum_obj min_sum_abs_obj max_sum_abs_obj')
    non_zeroes_idx = 5
    min_rhs_idx = 8
    max_rhs_idx = 9
    max_rhslhs_idx = 10
    min_rhslhs_idx = 11
    max_sum_obj_idx = 12
    min_sum_obj_idx = 13
    max_sum_abs_obj_idx = 14
    min_sum_abs_obj_idx = 15

    non_zeroes, min_rhs, max_rhs, min_rhslhs, max_rhslhs, min_sum_obj, max_sum_obj, min_sum_abs_obj, max_sum_abs_obj = [0 for i in range(9)]

    with open(input_filename, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                non_zeroes = float(line_split[non_zeroes_idx])
                min_rhs = float(line_split[min_rhs_idx])
                max_rhs = float(line_split[max_rhs_idx])
                min_rhslhs = float(line_split[min_rhslhs_idx])
                max_rhslhs = float(line_split[max_rhslhs_idx])
                min_sum_obj = float(line_split[min_sum_obj_idx])
                max_sum_obj = float(line_split[max_sum_obj_idx])
                min_sum_abs_obj = float(line_split[min_sum_abs_obj_idx])
                max_sum_abs_obj = float(line_split[max_sum_abs_obj_idx])

    Requirements = RCNormReq(num_non_zeroes = non_zeroes, min_rhs= min_rhs, max_rhs = max_rhs, min_rhslhs = min_rhslhs
                             , max_rhslhs = max_rhslhs, min_sum_obj = min_sum_obj, max_sum_obj = max_sum_obj, min_sum_abs_obj = min_sum_abs_obj
                             , max_sum_abs_obj = max_sum_abs_obj)

    return Requirements


#MIP/LP convert to GAP Percentages
#obj val normalise to 0,1?
#normalise based on instance min/max values
def normaliseMinMax(min, max, input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Data"])
                else:
                    normalised_row = []
                    for col_idx, value in enumerate(line_split):
                        if col_idx == 0:
                            normalised_row.append(value)
                        else:
                            normalised_val = (float(value) - min) / (max - min)
                            normalised_row.append(normalised_val)
                    writer.writerow(normalised_row)

#normalise data against a count
def normaliseCount(count, input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Data"])
                else:
                    normalised_row = []
                    for col_idx, value in enumerate(line_split):
                        if col_idx == 0:
                            normalised_row.append(value)
                        else:
                            normalised_val = (float(value) / count)
                            normalised_row.append(normalised_val)
                    writer.writerow(normalised_row)

#calculate the min and max values of the list input
def calculateMinMaxRow(data_list):

    min = 999999999999999999999999999
    max = -999999999999999999999999999

    for data_point in data_list:
        if float(data_point) < min:
            min = float(data_point)
        if float(data_point) > max:
            max = float(data_point)

    return (min,max)

#conver the LR bounds to gap percentages based on best known solution
def convertBoundsToGap(best_known_solutions_path, LR_input_path, LR_output_path, instance_name):

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
            print("Error: Unable to find best solution vaL for instance " + instance_name)
            exit(-1)

    with open(LR_output_path, "w") as LR_gap_output_fs:
        writer = csv.writer(LR_gap_output_fs, delimiter=',')
        with open(LR_input_path, "r") as LR_Bound_input_fs:
            csvreader = csv.reader(LR_Bound_input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Gap (%)"])
                else:
                    gap_row = []
                    for col_idx, value in enumerate(line_split):
                        # first column is just decomposition index
                        if col_idx == 0:
                            gap_row.append(value)
                        else:
                            gap = ((best_known_sol - float(value)) / (best_known_sol)) * 100
                            gap_row.append(gap)
                    writer.writerow(gap_row)



def main():

    # list of features requiring normalisation
    relaxed_constraints_min_max_scaling_filenames = ["Largest_RHSLHS.csv", "Sum_obj.csv", "Sum_abs_obj.csv",
                                                     "RHS_vals.csv"]

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    raw_data_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):

            instance_statistics_filepath = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv"

            # get instance values required to normalise relaxed constraint statistics
            Requirements = getRequiredNormVals(instance_statistics_filepath)
            # store min max of required norm vals in order
            relaxed_constraints_normalisation_min_max = [(Requirements.min_rhslhs, Requirements.max_rhslhs),
                                                         (Requirements.min_sum_obj, Requirements.max_sum_obj)
                , (Requirements.min_sum_abs_obj, Requirements.max_sum_abs_obj),
                                                         (Requirements.min_rhs, Requirements.max_rhs)]

            for idx, relaxed_constraints_filename in enumerate(relaxed_constraints_min_max_scaling_filenames):
                # create output folders if they don't already exists

                input_raw_path = raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Relaxed_Constraint_Statistics"
                output_normalised_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Relaxed_Constraint_Statistics"
                Path(output_normalised_path).mkdir(parents=True, exist_ok=True)

                normaliseMinMax(relaxed_constraints_normalisation_min_max[idx][0],
                                relaxed_constraints_normalisation_min_max[idx][1]
                                , input_raw_path + "/" + relaxed_constraints_filename,
                                output_normalised_path + "/" + relaxed_constraints_filename)

            #non zero count scaling is done as a percentage of total non zeroes of the instance
            normaliseCount(Requirements.num_non_zeroes, input_raw_path + "/" + "Non_zero_counts.csv", output_normalised_path + "/" + "Non_zero_counts.csv")

            # Convert LR bounds to gap percentages
            best_known_results_path = processed_results_folder + "/" + "Best_Known_Solutions.csv"
            LR_input_path = raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/LROutputs" + "/" + "LR_outputs.csv"
            LR_output_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs.csv"
            convertBoundsToGap(best_known_results_path, LR_input_path, LR_output_path, instance_name)

if __name__ == "__main__":

    main()