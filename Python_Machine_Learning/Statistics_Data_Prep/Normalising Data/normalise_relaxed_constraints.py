import csv
from typing import NamedTuple
import collections


# open input file




#list of features requiring normalisation

relaxed_constraints_min_max_scaling_filenames = ["Largest_RHSLHS.csv", "Sum_obj.csv", "Sum_abs_obj.csv", "RHS_vals.csv"]

# subproblem folder

instance_name = "cost266-UUE.mps"

#relaxed constraints folder
relaxed_constraint_input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Raw_Data/Relaxed_Constraint_Statistics"

output_relaxed_constraint_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Normalised_Data/Relaxed_Constraint_Statistics"

#instance statistics

instance_statistics_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Raw_Data/Instance_Statistics/Instance_Statistics.csv"

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



def main():

    # normalise Relaxed Constraints
    Requirements = getRequiredNormVals(instance_statistics_filepath)

    #store min max of required norm vals in order
    relaxed_constraints_normalisation_min_max = [(Requirements.min_rhslhs, Requirements.max_rhslhs), (Requirements.min_sum_obj, Requirements.max_sum_obj)
    , (Requirements.min_sum_abs_obj, Requirements.max_sum_abs_obj), (Requirements.min_rhs, Requirements.max_rhs)]

    for idx, filename in enumerate(relaxed_constraints_min_max_scaling_filenames):
        normaliseMinMax(relaxed_constraints_normalisation_min_max[idx][0], relaxed_constraints_normalisation_min_max[idx][1]
        ,relaxed_constraint_input_folder + "/" + filename, output_relaxed_constraint_folder + "/" + filename)

    #non zero count scaling is done as a percentage of total non zeroes
    normaliseCount(Requirements.num_non_zeroes, relaxed_constraint_input_folder + "/" + "Non_zero_counts.csv", output_relaxed_constraint_folder + "/" + "Non_zero_counts.csv")











if __name__ == "__main__":

    main()