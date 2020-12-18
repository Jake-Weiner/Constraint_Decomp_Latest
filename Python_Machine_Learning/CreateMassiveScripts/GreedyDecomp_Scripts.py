import os
from os import path
from pathlib import Path
import shutil
def main():


    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"], ["g200x740.mps","h50x2450.mps","h80x6320.mps","h80x6320d.mps","k16x240b.mps"], ["snp-02-004-104.mps","snp-04-052-052.mps","snp-06-004-052.mps","snp-10-004-052.mps","snp-10-052-052.mps"]]

    script_root_folder =  "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts"

    Greedy_Generic_script_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Creation_Scripts/Greedy"
    run_all_generic_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/run_all"
    run_every_test_generic_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/run_every_test"
    chmod_script_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/change_file_permissions"
    #
    # Path.mkdir(mode=0o777, parents=False, exist_ok=False)

    instance_placeholder = "Instance_XXXXX"
    problem_type_placeholder = "ProblemType_XXXXX"
    job_batchnumber_placeholder = "XXXXX"
    runall_placeholder = "XXXXX"
    for problem_idx, problem_type in enumerate(problem_types):

        #create output folders if they don't already exist
        Path(script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "Greedy").mkdir(parents=True, exist_ok=True)

        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):

            #create bash scripts
            with open(script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "Greedy" + "/batch" + str(instance_idx), "w") as Greedy_batch_output_fs:
                with open(Greedy_Generic_script_folder + "/" + "batch0", "r") as Greedy_batch_input_fs:
                    for line in Greedy_batch_input_fs:
                        if instance_placeholder in line:
                            Greedy_batch_output_fs.write(line.replace(instance_placeholder,instance_name))
                        elif problem_type_placeholder in line:
                            Greedy_batch_output_fs.write(line.replace(problem_type_placeholder, problem_type))
                        else:
                            Greedy_batch_output_fs.write(line)

            #create job scripts
            with open(script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "Greedy" + "/job" + str(instance_idx) + "_script", "w") as Greedy_job_output_fs:
                with open(Greedy_Generic_script_folder + "/" + "job0.script", "r") as Greedy_job_input_fs:
                    for line in Greedy_job_input_fs:
                        if job_batchnumber_placeholder in line:
                            Greedy_job_output_fs.write(line.replace(job_batchnumber_placeholder,str(instance_idx)))
                        else:
                            Greedy_job_output_fs.write(line)

            # create the run_all script to run each batch job
        with open(script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "Greedy" + "/" + "run_all",
                  "w") as run_all_output_fs:
            with open(run_all_generic_filepath, "r") as run_all_input_fs:
                for line in run_all_input_fs:
                    if runall_placeholder in line:
                        run_all_output_fs.write(
                            line.replace(runall_placeholder, str(len(instance_names[problem_idx]) - 1)))
                    else:
                        run_all_output_fs.write(line)


        # if the run_every_test script which iterates through all sub-folders and runs the run_all scripts has not been copied across,
        # then copy it in
        if not Path(script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "run_every_test").is_file():
            shutil.copyfile(run_every_test_generic_filepath, script_root_folder + "/" + problem_type + "/" + "Decomps" + "/" + "run_every_test")

        # if chmod all files hasn't been copied across
        if not Path(script_root_folder + "/" + problem_type + "/" + "change_file_permissions").is_file():
            shutil.copyfile(chmod_script_filepath,
                            script_root_folder + "/" + problem_type + "/" + "change_file_permissions")


if __name__ == "__main__":

    main()