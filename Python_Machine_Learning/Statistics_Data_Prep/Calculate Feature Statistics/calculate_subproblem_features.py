import statistics
import csv
import os

#list of features which we want to get only average and stddev
subproblem_ave_stddev = ["Sum_obj.csv", "Sum_abs_obj.csv", "Obj_range.csv", "average_RHS.csv", "average_abs_RHS.csv", "Largest_RHSLHS.csv", "RHS_range.csv"]

def calculateStats(data_list):

    min_val, max_val, ave, stddev = 0, 0, 0, 0

    data_list_float = [float(i) for i in data_list]
    min_val = min(data_list_float)
    max_val = max(data_list_float)
    ave = statistics.mean(data_list_float)
    stddev = statistics.pstdev(data_list_float)

    return(min_val,max_val,ave,stddev)

def writeAveStddev(input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Average", "Stddev"])
                else:
                    stats_row = []
                    # calculate ave,stddev of row
                    ave_stddev_tuple = calculateStats(line_split[1:])
                    stats_row.append(line_split[0])
                    stats_row.append(ave_stddev_tuple[2])
                    stats_row.append(ave_stddev_tuple[3])
                    writer.writerow(stats_row)

def writeAllStats(input_file, output_file):
    with open(output_file, "w") as output_fs:
        writer = csv.writer(output_fs, delimiter=',')
        with open(input_file, "r") as input_fs:
            csvreader = csv.reader(input_fs, delimiter=',')
            for line_number, line_split in enumerate(csvreader):
                if line_number == 0:
                    writer.writerow(["Decomposition Index", "Min", "Max", "Average", "Stddev"])
                else:
                    stats_row = []
                    # calculate ave,stddev of row
                    stats_tuple = calculateStats(line_split[1:])
                    stats_row.append(line_split[0])
                    #min
                    stats_row.append(stats_tuple[0])
                    #max
                    stats_row.append(stats_tuple[1])
                    #ave
                    stats_row.append(stats_tuple[2])
                    #stddev
                    stats_row.append(stats_tuple[3])
                    writer.writerow(stats_row)

def main():

    input_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Normalised_Data/Subproblem_Statistics"
    output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps/Features_Calculated/Subproblem_Statistics"
    for filename in subproblem_ave_stddev:
        writeAveStddev(input_folder + "/" + filename, output_folder + "/" + filename)
    #every other file in subproblem statistics folder, we want the min, max, ave and stddev
    for filename in os.listdir(input_folder):
        if filename not in subproblem_ave_stddev:
            writeAllStats(input_folder + "/" + filename, output_folder + "/" + filename)



if __name__ == "__main__":

    main()