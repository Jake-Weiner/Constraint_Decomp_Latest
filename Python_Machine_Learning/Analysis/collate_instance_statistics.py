
def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]


    raw_data_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
    output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Model_Comparisons"
    with open(output_folder + "/" + "instances_statistics_collated.csv", "w") as instance_statistics_collated_output_fs:
        instance_statistics_collated_output_fs.write("Problem Type, Instance Name, No. Var,No. Constr,No. Bin,No. Int,No. Cont,No. Non-Zeroes,Min Obj Term,Max Obj Term,Min Rhs,Max Rhs,Max RHSLHS,Min RHSLHS,Max Sum Obj,Min Sum Obj,Max Sum Abs Obj,Min Sum Abs Obj,No. Equality Constr \n")
        for problem_idx, problem_type in enumerate(problem_types):
            # create output folders if they don't already exists
            for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
                with open(raw_data_root_folder + '/' + problem_type + "/" + instance_name + '/Instance_Statistics' + "/" + "Instance_Statistics.csv", "r") as instance_statistics_input_fs:
                    instance_statistics_collated_output_fs.write(problem_type + "," + instance_name + ',')
                    for line_number, line in enumerate(instance_statistics_input_fs):
                        if line_number != 0:
                            instance_statistics_collated_output_fs.write(line)



if __name__ == "__main__":

    main()