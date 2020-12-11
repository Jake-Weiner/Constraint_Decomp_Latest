import csv

def main():
    # location of csv1
    csv_1 = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/Decompositions/Greedy/cost266-UUE.mps"
    # location of csv2
    csv_2 = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/Decompositions/NSGA_Decompositions/cost266-UUE.mps"

    # location of combined csv
    combined_csv = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/Decompositions/cost266_combined_decomps"

    with open(combined_csv, "w") as output_file:
        with open(csv_1, "r") as input_file1:
            for row in input_file1:
                output_file.write(row)
        with open(csv_2, "r") as input_file2:
            for row in input_file2:
                output_file.write(row)


if __name__ == "__main__":

    main()
