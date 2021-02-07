import csv
#create script to find the specific subproblem (decomp index, col_idx) and look up the lp _solve time in the corresponding file

def main():


    problem_type = "supply_network_planning"
    # , "supply_network_planning"]
    instance_name = "snp-02-004-104.mps"

    raw_data_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/Massive_Outputs"

    line_number_inf = 0
    col_number_inf = 0
    with open(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/LP_bounds.csv", "r") as input_fs:
        csv_reader = csv.reader(input_fs, delimiter = ",")
        for line_num, line_split in enumerate(csv_reader):
            for col_idx, value in enumerate(line_split):
                if "inf" in value:
                    line_number_inf = line_num
                    col_number_inf = col_idx
                    break

    with open(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/LP_times.csv", "r") as input_fs:
        csv_reader = csv.reader(input_fs, delimiter = ",")
        for line_num, line_split in enumerate(csv_reader):
            for col_idx, value in enumerate(line_split):
                if line_num == line_number_inf and col_idx == col_number_inf:
                    print("line number is " + str(line_num))
                    print("col idx is " + str(col_idx))
                    print(value)
                    break

    with open(raw_data_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/MIP_bounds.csv", "r") as input_fs:
        csv_reader = csv.reader(input_fs, delimiter = ",")
        for line_num, line_split in enumerate(csv_reader):
            for col_idx, value in enumerate(line_split):
                if line_num == line_number_inf and col_idx == col_number_inf:
                    print("line number is " + str(line_num))
                    print("col idx is " + str(col_idx))
                    print(value)
                    break



if __name__ == "__main__":
    main()