import csv
from typing import NamedTuple
import collections
from pathlib import Path

#this script will normalise all gap/times in both decompos and no_con_rel decomps


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
                            gap = max(gap, 0)
                            if gap < 0:
                                print(gap)

                            gap_row.append(gap)
                    writer.writerow(gap_row)

def normaliseLRGaps(LR_decomp_input_path, LR_no_con_relaxed_input_path,LR_decomp_output_path, LR_no_con_relaxed_output_path):
    
    
    min_decomp_gap, max_decomp_gap, min_decomp_time, max_decomp_time = extractMinMax(LR_decomp_input_path)
    min_no_con_relaxed_gap, max_no_con_relaxed_gap, min_no_con_relaxed_time, max_no_con_relaxed_time = extractMinMax(LR_no_con_relaxed_input_path)
    
    min_gap = min(min_decomp_gap, min_no_con_relaxed_gap)
    max_gap = max(max_decomp_gap, max_no_con_relaxed_gap)
    min_time = min(min_decomp_time, min_no_con_relaxed_time)
    max_time = max(max_decomp_time, max_no_con_relaxed_time)
    
    decomp_col_idx = 0
    gap_val_col_idx = 1
    time_val_col_idx = 2
    with open(LR_decomp_output_path, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(LR_decomp_input_path, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Gap (%)", "LR Solve Time(s)"])
                else:
                    normalised_row = []
                    decomp_number = line_split[decomp_col_idx]
                    gap_val = float(line_split[gap_val_col_idx])

                    time_val = float(line_split[time_val_col_idx])
                    gap_val_normalised = (gap_val-min_gap) / (max_gap - min_gap)

                    time_val_normalised = (time_val-min_time) / (max_time - min_time)
                    normalised_row.append(decomp_number)
                    normalised_row.append(gap_val_normalised)
                    normalised_row.append(time_val_normalised)
                    writer.writerow(normalised_row)

    with open(LR_no_con_relaxed_output_path, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(LR_no_con_relaxed_input_path, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Gap (%)", "LR Solve Time(s)"])
                else:
                    normalised_row = []
                    decomp_number = line_split[decomp_col_idx]
                    gap_val = float(line_split[gap_val_col_idx])
                    time_val = float(line_split[time_val_col_idx])
                    gap_val_normalised = (gap_val-min_gap) / (max_gap - min_gap)
                    time_val_normalised = (time_val-min_time) / (max_time - min_time)
                    normalised_row.append(decomp_number)
                    normalised_row.append(gap_val_normalised)
                    normalised_row.append(time_val_normalised)
                    writer.writerow(normalised_row)

# extract min and max gap values for normalisation
def extractMinMax(LR_input_file):
    gap_col_idx = 1
    time_col_idx = 2
    min_gap = 999999999999999999999999999
    max_gap = -999999999999999999999999999
    min_time = 999999999999999999999999999
    max_time = -999999999999999999999999999
    
    with open(LR_input_file, "r") as LR_input_file_fs:
        csv_reader = csv.reader(LR_input_file_fs, delimiter = ",")
        for line_number, line_split in enumerate(csv_reader):
            if line_number > 0:
                gap_val = float(line_split[gap_col_idx])
                time_val = float(line_split[time_col_idx])
                if gap_val < min_gap:
                    min_gap = gap_val
                if gap_val > max_gap:
                    max_gap = gap_val
                if time_val < min_time:
                    min_time = time_val
                if time_val > max_time:
                    max_time = time_val
                                       
                
    return min_gap, max_gap, min_time, max_time

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

            #first, convert Bounds to gap percentages

            Path(
                processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs").mkdir(
                parents=True, exist_ok=True)
            # Convert LR bounds to gap percentages
            # LR Bounds for original results
            LR_input_path = raw_no_con_rel_folder + "/" + problem_type + "/" + instance_name + "/LROutputs" + "/" + "LR_outputs.csv"
            LR_output_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs.csv"
            #
            # #make output folder if not already created

            convertBoundsToGap(best_known_solutions_path, LR_input_path, LR_output_path, instance_name)

            # # LR Bounds for no con relaxed
            # LR_no_con_rel_input_path = raw_no_con_rel_folder + "/" + problem_type + "/" + instance_name + "/LROutputs" + "/" + "LR_outputs.csv"
            # LR_no_con_rel_output_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            # convertBoundsToGap(best_known_results_path, LR_no_con_rel_input_path, LR_no_con_rel_output_path, instance_name)
            #
            # LR_decomp_input_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs.csv"
            # LR_no_con_relaxed_input_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_no_con_rel_outputs.csv"
            # LR_decomp_output_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            # LR_no_con_relaxed_output_path = processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_no_con_rel_outputs_normalised.csv"
            # #make output folder if not already created
            # Path(processed_no_rel_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs").mkdir(parents=True, exist_ok=True)
            # normaliseLRGaps(LR_decomp_input_path, LR_no_con_relaxed_input_path, LR_decomp_output_path, LR_no_con_relaxed_output_path)
            # print("Finished " + instance_name)

if __name__ == "__main__":
    main()