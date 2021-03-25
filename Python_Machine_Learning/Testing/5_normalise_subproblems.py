import csv
from typing import NamedTuple
import collections
from pathlib import Path


# open input file
#list of features requiring normalisation
# subproblem folder
#normalise based on decomposition values which are calculated on the fly. Normalisation is done based on values of other subproblems
# in addition, inf values occuring in Obj_range files are excluded
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
                            #inf values should be 0 - they are only in obj_range.csv, if subproblem does not have appear in obj function (essentially, obj coeff of 0)
                            if "inf" in value:
                                normalised_val = 0
                                normalised_row.append(normalised_val)
                            else:
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
        if "inf" not in data_point:
            if float(data_point) < min:
                min = float(data_point)
            if float(data_point) > max:
                max = float(data_point)
    return (min,max)


def main():

    #list of subproblem features which require normalisation
    subproblem_normalisation_filenames = ["Sum_obj.csv", "Sum_abs_obj.csv", "Obj_range.csv", "average_RHS.csv",
                                          "average_abs_RHS.csv", "Largest_RHSLHS.csv", "RHS_range.csv", "Shapes.csv"]



    raw_data_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"
    processed_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Processed_Results"

    #collate subproblem statistics to manually normalise
    raw_subproblem_statistics_folder = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Raw_Data/Subproblem"
    processed_subproblem_statistics_folder = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Normalised_Data/Subproblem_Statistics"
    for subproblem_filename in subproblem_normalisation_filenames:
        normaliseSubproblems(raw_subproblem_statistics_folder + "/" + subproblem_filename,
                             processed_subproblem_statistics_folder + "/" + subproblem_filename)



if __name__ == "__main__":

    main()