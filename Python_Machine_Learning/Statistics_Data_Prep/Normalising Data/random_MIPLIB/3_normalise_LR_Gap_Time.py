import csv
from typing import NamedTuple
import collections
from pathlib import Path

#this script will normalise all gap/times in both decompos and no_con_rel decomps
def normaliseLRGaps(LR_input_path,LR_output_path):
    
    
    min_gap, max_gap, min_time, max_time = extractMinMax(LR_input_path)
    print(max_gap)
    decomp_col_idx = 0
    gap_val_col_idx = 1
    time_val_col_idx = 2
    with open(LR_output_path, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(LR_input_path, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Gap (%)", "LR Solve Time(s)"])
                else:
                    normalised_row = []
                    decomp_number = line_split[decomp_col_idx]
                    gap_val = float(line_split[gap_val_col_idx])
                    if max_gap == min_gap:
                        print(LR_input_path)
                        print(max_gap)
                        print(min_gap)
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


    # problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    #
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps",  "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
    #                    "snp-10-052-052.mps"]]

    problem_types = ["random_MIPLIB"]
    instance_names = [
        ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
         "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

    processed_results_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            #set up input and output paths
            LR_input_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_gaps.csv"
            LR_output_path = processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "LROutputs" + "/" + "LR_outputs_normalised.csv"
            #normalise all LR gaps and Times
            normaliseLRGaps(LR_input_path, LR_output_path)


if __name__ == "__main__":
    main()