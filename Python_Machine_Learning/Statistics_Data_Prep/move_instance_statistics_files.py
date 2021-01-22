#this script will copy across the instance statistics to the correct folders
#as these instance statistics weren't captured during the run
from shutil import copyfile


def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps"
                          , "snp-10-004-052.mps", "snp-10-052-052.mps"]]

    raw_results_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Massive_Outputs"
    instance_statistics_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Instance_Statistics_Local"
    for problem_type_idx, problem_type in enumerate(problem_types):
        for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
            copyfile(instance_statistics_root_folder + "/" + problem_type + "/" + instance_name + "/Instance_Statistics.csv", raw_results_folder + "/" + problem_type + "/" + instance_name + "/Instance_Statistics" + "/Instance_Statistics.csv")

if __name__ == "__main__":
    main()