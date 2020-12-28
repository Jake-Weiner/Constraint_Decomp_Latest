import csv
import os

def main():

    # folder of input files to collate
    input_folder = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Raw_Data/Subproblem"
    #output location
    output_file_path = "/home/jake/PhD/Decomposition/Output/testing/subproblem_statistics/test2.mps/Raw_Data/Subproblem_collated.csv"
    # location of csv1

    with open(output_file_path, "w") as output_file_stream:
        for filename in os.listdir(input_folder):
            output_file_stream.write(filename + ",")
            with open(input_folder + "/" + filename, "r") as input_file_stream:
                #skip to the first line
                for line_number, row in enumerate(input_file_stream):
                    if line_number == 1:
                        output_file_stream.write(row)
                    elif line_number > 1:
                        break



if __name__ == "__main__":

    main()
