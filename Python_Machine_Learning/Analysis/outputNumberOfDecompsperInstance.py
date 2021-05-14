#importing libraries
import pandas as pd
from pathlib import Path

#global vars
problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "random_MIPLIB"]

instance_names_testing = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"],
                          ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps",
                           "neos-4387871-tavua.mps",
                           "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]

features_calculated_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
decomp_stats_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/DecompStatistics"

#print the best decomp scores possible for each problem type
def outputNumberOfDecomps():


    Path(decomp_stats_output_folder).mkdir(parents=True, exist_ok=True)
    # test if output file exists
    decomp_stats_output_path = decomp_stats_output_folder + "/" + "Decomp_Counts.csv"
    my_file = Path(decomp_stats_output_path)
    # write headers if file does not exist
    if not my_file.is_file():
        with open(decomp_stats_output_path,"w") as decomp_counts_output_fs:
            decomp_counts_output_fs.write(
                "Problem Type,Instance Name,Number of Decompositions" + "\n")
    with open(decomp_stats_output_path, "a+") as decomp_counts_output_fs:
        # get the best decomp scores possible across each problem type
        for problem_type_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
                input_data_filepath = features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv"
                df_collated = pd.read_csv(input_data_filepath)
                decomp_counts_output_fs.write("{},{},{}\n".format(problem_type,instance_name,df_collated.shape[0]))

    #reformat the output - Network Design - etc...


def main():
    #first test, same problemTest_Type_Enum

    outputNumberOfDecomps()


#store the important features in a list
if __name__ == "__main__":

    main()