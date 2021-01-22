import csv
from typing import NamedTuple
import collections
from pathlib import Path


# open input file


#list of features requiring normalisation

# subproblem folder


#normalise based on decomposition values which are calculated on the fly. Normalisation is done based on values of other subproblems
def normaliseSubproblems(input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Normalised Data"])
                else:
                    normalised_row = []
                    min_max_tuple = calculateMinMaxRow(line_split[1:])
                    #calculate min/max of the row
                    for col_idx, value in enumerate(line_split):
                        if col_idx == 0:
                            normalised_row.append(value)
                        else:
                            normalised_val = 0
                            # if min and max are equal, set normalised val to 0
                            if min_max_tuple[0] == min_max_tuple[1]:
                                normalised_val = 0
                            else:
                                normalised_val = (float(value) - min_max_tuple[0]) / (min_max_tuple[1] - min_max_tuple[0])
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


def main():

    #list of subproblem features which require normalisation
    subproblem_normalisation_filenames = ["Sum_obj.csv", "Sum_abs_obj.csv", "Obj_range.csv", "average_RHS.csv",
                                          "average_abs_RHS.csv", "Largest_RHSLHS.csv", "RHS_range.csv", "Shapes.csv"]

    problem_types = ["network_design", "fixed_cost_network_flow"]

    # , "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"]]

    # , "ta2-UUE.mps"
                      # ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                      #  "snp-10-052-052.mps"]]

    raw_data_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # create output folders if they don't already exists
            Path(processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics").mkdir(
                parents=True, exist_ok=True)
            for subproblem_filename in subproblem_normalisation_filenames:
                normaliseSubproblems(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/" + subproblem_filename,
                                     processed_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics" + "/" + subproblem_filename)







if __name__ == "__main__":

    main()