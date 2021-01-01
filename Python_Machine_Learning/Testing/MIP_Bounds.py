import csv

def main():


    input_bounds_file = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/error_checking/ta2-UUE.mps/Subproblem_Statistics/MIP_bounds.csv"
    decomp_idx_col = 0
    desired_decomp_number = 1

    with open(input_bounds_file, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number > 0:
                # print(line_split)
                if float(line_split[decomp_idx_col]) == desired_decomp_number:
                    mip_bound_sum = 0
                    min = 1000000
                    sp_min_idx = -1
                    print(line_split[1:])
                    print("number of sps is " + str(len(line_split[1:])))
                    for sp_idx, sp_bound in enumerate(line_split[1:]):
                        if (float(sp_bound) < min):
                            min = float(sp_bound)
                            sp_min_idx = sp_idx
                        mip_bound_sum += float(sp_bound)

                    print(min)
                    print(sp_min_idx)
                    print(mip_bound_sum)







if __name__ == "__main__":

    main()