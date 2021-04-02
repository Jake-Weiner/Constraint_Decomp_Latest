import os
from os import path
from pathlib import Path
import shutil


def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "random_MIPLIB"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"],
                      ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps",
                       "neos-4387871-tavua.mps", "neos-3656078-kumeu.mps", "neos-4338804-snowy.mps", "air05.mps",
                       "neos-4954672-berkel.mps", "splice1k1.mps"]]

    output_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts"
    action_name = "Gather_Statistics"

    Generic_script_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Gather_Statistics_Scripts"

    instance_placeholder = "Instance_XXXXX"
    problem_type_placeholder = "ProblemType_XXXXX"
    job_batchnumber_placeholder = "XXXXX"
    jobname_placeholder = "$JOBNAME"
    runtime_placeholder = "$RUNTIME"
    memory_placeholder = "$MEMORY"
    runall_placeholder = "XXXXX"

    problem_types_runtimes = ["72:00:00", "72:00:00", "168:00:00", "72:00:00"]
    problem_types_memory = ["8192", "8192", "16384", "8192"]

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist
        Path(output_root_folder + "/" + problem_type + "/" + action_name).mkdir(
            parents=True, exist_ok=True)
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # create bash scripts
            with open(
                    output_root_folder + "/" + problem_type + "/" + action_name + "/" + "/batch" + str(
                            instance_idx), "w") as batch_output_fs:
                with open(Generic_script_folder + "/" + "batch0", "r") as batch_input_fs:
                    for line in batch_input_fs:
                        if instance_placeholder in line:
                            batch_output_fs.write(line.replace(instance_placeholder, instance_name))
                        elif problem_type_placeholder in line:
                            batch_output_fs.write(line.replace(problem_type_placeholder, problem_type))
                        else:
                            batch_output_fs.write(line)

            # create job scripts
            with open(
                    output_root_folder + "/" + problem_type + "/" + action_name + "/" + "/job" + str(
                            instance_idx) + "_script", "w") as job_output_fs:
                with open(Generic_script_folder + "/" + "job0_script",
                          "r") as job_input_fs:
                    for line in job_input_fs:
                        if job_batchnumber_placeholder in line:
                            job_output_fs.write(line.replace(job_batchnumber_placeholder, str(instance_idx)))
                        elif jobname_placeholder in line:
                            job_output_fs.write(line.replace(jobname_placeholder, instance_name + "_" + action_name))
                        elif runtime_placeholder in line:
                            job_output_fs.write(line.replace(runtime_placeholder, problem_types_runtimes[problem_idx]))
                        elif memory_placeholder in line:
                            job_output_fs.write(line.replace(memory_placeholder, problem_types_memory[problem_idx]))
                        else:
                            job_output_fs.write(line)

        # create the run_all script to run each batch job
        with open(
                output_root_folder +  "/" + problem_type + "/" + action_name + "/" + "run_all",
                "w") as run_all_output_fs:
            with open(Generic_script_folder + "/" + "run_all", "r") as run_all_input_fs:
                for line in run_all_input_fs:
                    if runall_placeholder in line:
                        run_all_output_fs.write(
                            line.replace(runall_placeholder, str(len(instance_names[problem_idx]) - 1)))
                    else:
                        run_all_output_fs.write(line)

        # if the run_every_test script which iterates through all sub-folders and runs the run_all scripts has not been copied across,
        # then copy it in
        if not Path(
                output_root_folder + "/" + problem_type + "/" + "run_every_test").is_file():
            shutil.copyfile(Generic_script_folder + "/" + "run_every_test",
                            output_root_folder + "/" + problem_type + "/" + "run_every_test")

        # if chmod all files hasn't been copied across
        if not Path(output_root_folder + "/" + problem_type + "/" + "change_file_permissions").is_file():
            shutil.copyfile(Generic_script_folder + "/" + "change_file_permissions",
                            output_root_folder + "/" + problem_type + "/" + "change_file_permissions")


if __name__ == "__main__":
    main()