import csv
#linecache is used to extract a specific line from input file
import linecache

def combine_bin_int_stats(input_root_folder, output_root_folder):

    # open up bin and int csvs to combine
    # store values of bin, then add to these the int values

    # store total number of lines in input_file
    total_lines = 0
    with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
        total_lines = sum(1 for row in bin_prop_input_fs)

    print(total_lines)
    with open(output_root_folder + "/" + "Bin_Int_Combined.csv", "w") as bin_int_combined_output_fs:
        bin_int_csv_writer = csv.writer(bin_int_combined_output_fs)

        # copy the first line across, containing column names
        with open(input_root_folder + "/" + "Bin_props.csv", "r") as bin_prop_input_fs:
            for line in bin_prop_input_fs:
                bin_int_combined_output_fs.write(line)
                break

        # lines in the file are indexed from 1. Ignore the first row as these are just column header names
        current_row_number = 2
        while current_row_number < total_lines + 1:
            bin_line = linecache.getline(input_root_folder + "/" + "Bin_props.csv", current_row_number)
            bin_line_split = bin_line.split(",")
            bin_props = [float(element) for element in bin_line_split[1:]]
            int_line = linecache.getline(input_root_folder + "/" + "Int_props.csv", current_row_number)
            int_line_split = int_line.split(",")
            int_props = [float(element) for element in int_line_split[1:]]

            #zip function combines two different list
            int_bin_combined_props = [bin_prop + int_prop for int_prop, bin_prop in zip(bin_props, int_props)]
            int_bin_line = [current_row_number - 2] + int_bin_combined_props
            bin_int_csv_writer.writerow(int_bin_line)
            current_row_number += 1

def main():

    # problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    # instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
    #                   ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
    #                   ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps", "snp-10-052-052.mps"]]
    #
    #
    # raw_data_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    # processed_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results"
    #
    # for problem_idx, problem_type in enumerate(problem_types):
    #     # create output folders if they don't already exists
    #     for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
    #         combine_bin_int_stats(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/Relaxed_Constraint_Statistics",processed_results_folder)

    combine_bin_int_stats("/home/jake/PhD/Decomposition/Input/Unit_Testing/bin_int_combined", "/home/jake/PhD/Decomposition/Input/Unit_Testing/bin_int_combined")


if __name__ == "__main__":

    main()