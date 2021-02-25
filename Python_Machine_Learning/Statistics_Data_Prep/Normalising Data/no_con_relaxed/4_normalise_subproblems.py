import sys
#local imports
sys.path.append("/home/jake/PycharmProjects/Decomposition_Machine_Learning/Statistics_Data_Prep/Normalising Data")
import Subproblem_Normalisation_Functions as SNF
from pathlib import Path



def main():

    #list of subproblem features which require normalisation
    subproblem_normalisation_filenames = ["Sum_obj.csv", "Sum_abs_obj.csv", "Obj_range.csv", "average_RHS.csv",
                                          "average_abs_RHS.csv", "Largest_RHSLHS.csv", "RHS_range.csv", "Shapes.csv"]

    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]

    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                        "snp-10-052-052.mps"]]

  

    raw_no_con_rel_root_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Index_Updated"
    processed_no_con_rel_results_folder = "/media/jake/Jakes_Harddrive/PhD/Decomposition/Machine_Learning/No_Con_Relaxed_Processed_Results"

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # create output folders if they don't already exists
            Path(processed_no_con_rel_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics").mkdir(
                parents=True, exist_ok=True)
            for subproblem_filename in subproblem_normalisation_filenames:
                SNF.normaliseSubproblems(raw_no_con_rel_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Subproblem_Statistics" + "/" + subproblem_filename,
                                     processed_no_con_rel_results_folder + "/" + problem_type + "/" + instance_name + "/" + "Normalised_Data" + "/" + "Subproblem_Statistics" + "/" + subproblem_filename)

            print("Finished Processing " + instance_name)

if __name__ == "__main__":

    main()