import csv

#convert the LR bounds to gap percentages based on best known solution
def convertBoundsToGap(best_known_solutions_path, LR_input_path, LR_output_path, instance_name):

    instance_name_col_idx = 0
    instance_sol_col_idx = 1
    best_known_sol = -999999999999999

    with open(best_known_solutions_path, "r") as best_known_sol_fs:
        csvreader = csv.reader(best_known_sol_fs, delimiter=',')
        best_known_sol_found = False
        for line_number, line_split in enumerate(csvreader):
            if line_split[instance_name_col_idx] == instance_name:
                best_known_sol_found = True
                best_known_sol = float(line_split[instance_sol_col_idx])

        if best_known_sol_found == False:
            print("Error: Unable to find best solution val for instance " + instance_name)
            exit(-1)

    with open(LR_output_path, "w") as LR_gap_output_fs:
        writer = csv.writer(LR_gap_output_fs, delimiter=',')
        with open(LR_input_path, "r") as LR_Bound_input_fs:
            csvreader = csv.reader(LR_Bound_input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Gap (%)", "LR Solve Time(s)"])
                    continue
                else:
                    gap_row = []
                    for col_idx, value in enumerate(line_split):
                        # first column is just decomposition index
                        if col_idx == 0 or col_idx == 2:
                            gap_row.append(value)
                        elif col_idx == 1:
                            gap = ((best_known_sol - float(value)) / (best_known_sol)) * 100
                            gap = max(gap, 0)
                            if gap < 0:
                                print(gap)

                            gap_row.append(gap)
                    writer.writerow(gap_row)
