import os
from os import path
from pathlib import Path
import shutil

#this script will copy and change the generic scripts by filling in the placeholders within and changing the script names to
#their new appropriate ones

class BatchRequirements:
    def __init__(self, instance_name, instance_placeholder, problem_type, problem_placeholder, batch_number, batch_number_placeholder):
        self.instance_name = instance_name
        self.instance_placeholder = instance_placeholder
        self.problem_type = problem_type
        self.problem_placeholder = problem_placeholder
        self.batch_number = batch_number
        self.batch_number_placeholder = batch_number_placeholder

class JobRequirements:
    def __init__(self, job_batchnumber_placeholder, batch_number, instance_placeholder, instance):
        self.job_batchnumber_placeholder = job_batchnumber_placeholder
        self.batch_number = batch_number
        self.instance_placeholder = instance_placeholder
        self.instance = instance

class RunAllRequirements:
    def __init__(self, runall_placeholder, number_of_jobs):
        self.runall_placeholder = runall_placeholder
        self.number_of_jobs = number_of_jobs

def copy_batch_script(input_filepath, output_filepath, batch_requirements):

    with open(output_filepath, "w") as output_fs:
        with open(input_filepath, "r") as input_fs:
            for line in input_fs:
                new_line = line
                if batch_requirements.instance_placeholder in line:
                    new_line = line.replace(batch_requirements.instance_placeholder, batch_requirements.instance_name)
                if batch_requirements.problem_placeholder in line:
                    new_line = line.replace(batch_requirements.problem_placeholder, batch_requirements.problem_type)
                if batch_requirements.batch_number_placeholder in line:
                    new_line = line.replace(batch_requirements.batch_number_placeholder, str(batch_requirements.batch_number))
                output_fs.write(new_line)

def copy_job_script(input_filepath, output_filepath, job_requirements):
    with open(output_filepath, "w") as output_fs:
        with open(input_filepath, "r") as input_fs:
            for line in input_fs:
                new_line = line
                if job_requirements.job_batchnumber_placeholder in line:
                    new_line = new_line.replace(job_requirements.job_batchnumber_placeholder,
                                                str(job_requirements.batch_number))
                if job_requirements.instance_placeholder in line:
                    new_line = new_line.replace(job_requirements.instance_placeholder, job_requirements.instance)
                output_fs.write(new_line)


def copy_run_all_script(input_filepath, output_filepath, runall_requirements):
    with open(output_filepath, "w") as run_all_output_fs:
        with open(input_filepath, "r") as run_all_input_fs:
            for line in run_all_input_fs:
                if runall_requirements.runall_placeholder in line:
                    run_all_output_fs.write(line.replace(runall_requirements.runall_placeholder, str(runall_requirements.number_of_jobs - 1)))
                else:
                    run_all_output_fs.write(line)

#change generic run all scipts by filling in generic placeholders with specific requirements
def copy_run_all_script(input_filepath, output_filepath, runall_requirements):
    with open(output_filepath, "w") as run_all_output_fs:
        with open(input_filepath, "r") as run_all_input_fs:
            for line in run_all_input_fs:
                if runall_requirements.runall_placeholder in line:
                    run_all_output_fs.write(line.replace(runall_requirements.runall_placeholder, str(runall_requirements.number_of_jobs - 1)))
                else:
                    run_all_output_fs.write(line)


def main():
    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"]]

    script_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts"
    decomp_type = "Ranking"
    Constraint_Processing_Type = ["duplicate", "redundant"]


    # "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts"
    Generic_script_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts"

    #placeholders within generic scripts which are replaced with problem types and instance names
    instance_placeholder = "Instance_XXXXX"
    problem_type_placeholder = "ProblemType_XXXXX"
    batch_number_placeholder = "BATCHNUMBER"
    job_batchnumber_placeholder = "BATCHNUMBER"
    job_instance_placeholder = "INSTANCE"
    runall_placeholder = "XXXXX"

    number_of_batches = 10

    #generic input paths


    for processing_type in Constraint_Processing_Type:
        for problem_idx, problem_type in enumerate(problem_types):
            for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
                batch_output_folder = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + instance_name
                job_output_folder = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + instance_name
                # create output folders if they don't already exist
                Path(batch_output_folder).mkdir(
                    parents=True, exist_ok=True)
                for batch_number in range(0, number_of_batches):
                    # create bash scripts
                    batch_requirements = BatchRequirements(instance_name, instance_placeholder, problem_type,
                                                           problem_type_placeholder, batch_number, batch_number_placeholder)
                    batch_script_filename = "remove_" + processing_type + "_Ranking_constraints_script"
                    generic_batch_input_path = Generic_script_folder + "/" + decomp_type + "/remove_" + processing_type + "/" + batch_script_filename

                    batch_output_path = batch_output_folder + "/" + batch_script_filename + "_" + str(batch_number)
                    #BATCH SCRIPTS
                    # duplicate removal script needs to be run twice

                    copy_batch_script(generic_batch_input_path, batch_output_path, batch_requirements)

                    #JOB SCRIPTS
                    # create job scripts
                    job_requirements = JobRequirements(job_batchnumber_placeholder, batch_number,
                                                       job_instance_placeholder, instance_name)
                    job_filename = "remove_" + processing_type + "_constraints_job_script"
                    processing_job_script_output_file_path = job_output_folder + "/" + job_filename + "_" + str(
                        batch_number)
                    job_script_input_path = Generic_script_folder + "/" + decomp_type + "/remove_" + processing_type + "/" + job_filename
                    # create job scripts
                    copy_job_script(job_script_input_path, processing_job_script_output_file_path, job_requirements)

                run_all_output_folder = job_output_folder
                # create run_all scripts
                runall_requirements = RunAllRequirements(runall_placeholder, number_of_batches)
                output_run_all_path = job_output_folder + "/run_all"
                input_run_all_path = Generic_script_folder + "/" + decomp_type + "/remove_" + processing_type + "/" + "run_all"
                copy_run_all_script(input_run_all_path, output_run_all_path, runall_requirements)

            # if the run_every_test script which iterates through all sub-folders and runs the run_all scripts has not been copied across,
            # then copy it in
            run_every_test_output_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + "run_every_test"
            run_every_test_generic_path = Generic_script_folder + "/" + decomp_type + "/remove_" + processing_type + "/" + "run_every_test"
            if not Path(run_every_test_output_path).is_file():
                shutil.copyfile(run_every_test_generic_path, run_every_test_output_path)

            #Chmod file
            chmod_output_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + "change_file_permissions"
            chmod_script_generic_path = Generic_script_folder + "/" + decomp_type + "/" + "change_file_permissions"
            # if chmod all files hasn't been copied across
            if not Path(chmod_output_path).is_file():
                shutil.copyfile(chmod_script_generic_path, chmod_output_path)


if __name__ == "__main__":
    main()