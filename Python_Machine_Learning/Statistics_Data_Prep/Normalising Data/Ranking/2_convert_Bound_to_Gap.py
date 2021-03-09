import sys
#local imports
sys.path.append("/Statistics_Data_Prep/Normalising Data")
import csv
from typing import NamedTuple
import collections
from pathlib import Path


#convert the LR bounds to gap percentages based on best known solution
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
            print("Error: Unable to find best solution val for instance " + instance_name)
            exit(-1)

    with open(LR_output_path, "w") as LR_gap_output_fs:
        writer = csv.writer(LR_gap_output_fs, delimiter=',')
        with open(LR_input_path, "r") as LR_Bound_input_fs:
            csvreader = csv.reader(LR_Bound_input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Gap (%)", "LR Solve Time(s)"])
                    continue
                else:
                    gap_row = []
                    for col_idx, value in enumerate(line_split):
                        # first column is just decomposition index
                        if col_idx == 0 or col_idx == 2:
                            gap_row.append(value)
                        elif col_idx == 1:
                            gap = ((best_known_sol - float(value)) / (best_known_sol)) * 100
                            #ensure gap is greater than 0
                            gap = max(gap, 0)
                            if gap < 0:
                                print(gap)
                            gap_row.append(gap)
                    writer.writerow(gap_row)


def main():

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names_testing = [["germany50-UUM.mps"], ["k16x240b.mps"], ["snp-10-052-052.mps"]]

    raw_data_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs/Ranking"
    processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Ranking"
    best_known_solutions_path = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results/Best_Known_Solutions.csv"
    number_of_batches = 10
    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names_testing[problem_idx]):
            for batch_number in range(number_of_batches):
                #convert Bounds to gap percentage
                Path(
                    processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Normalised_Data" + "/" + "LROutputs").mkdir(
                    parents=True, exist_ok=True)
                # Convert LR bounds to gap percentages
                # LR Bounds for no con relaxed
                LR_input_path = raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/LROutputs" + "/" + "LR_outputs.csv"
                LR_output_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + str(batch_number) + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_gaps.csv"
                convertBoundsToGap(best_known_solutions_path, LR_input_path, LR_output_path, instance_name)

if __name__ == "__main__":
    main()