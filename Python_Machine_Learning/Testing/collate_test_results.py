import csv
import os

def main():

    # folder of input files to collate
    subproblem_input_folder= "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Normalised_Data/Subproblem_Statistics"
    #output location
    subproblem_output_file_path = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Normalised_Data/Subproblem_Statistics/Subproblem_collated.csv"
    # location of csv1

    with open(subproblem_output_file_path, "w") as output_file_stream:
        for filename in os.listdir(subproblem_input_folder):
            if "collated" not in filename:
                output_file_stream.write(filename + ",")
                with open(subproblem_input_folder + "/" + filename, "r") as input_file_stream:
                    #skip to the first line
                    for line_number, row in enumerate(input_file_stream):
                        if line_number == 1:
                            output_file_stream.write(row)
                        elif line_number > 1:
                            break

    # folder of input files to collate
    relaxed_constraint_input_folder = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Normalised_Data/Relaxed_Constraint_Statistics"
    # output location
    relaxed_constraint_output_file_path = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Normalised_Data/Relaxed_Constraint_Statistics/Relaxed_Constraint_Statistics_collated.csv"
    # location of csv1

    with open(relaxed_constraint_output_file_path, "w") as output_file_stream:
        for filename in os.listdir(relaxed_constraint_input_folder):
            if "collated" not in filename:
                output_file_stream.write(filename + ",")
                with open(relaxed_constraint_input_folder + "/" + filename, "r") as input_file_stream:
                    # skip to the first line
                    for line_number, row in enumerate(input_file_stream):
                        if line_number == 1:
                            output_file_stream.write(row)
                        elif line_number > 1:
                            break


if __name__ == "__main__":

    main()
