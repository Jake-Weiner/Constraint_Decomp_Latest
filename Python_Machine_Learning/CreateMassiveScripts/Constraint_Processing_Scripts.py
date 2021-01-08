import os
from os import path
from pathlib import Path
import shutil

#this script will copy and change the generic scripts by filling in the placeholders within and changing the script names to
#their new appropriate ones

class BatchRequirements:
    def __init__(self, instance_name, instance_placeholder, problem_type, problem_placeholder):
        self.instance_name = instance_name
        self.instance_placeholder = instance_placeholder
        self.problem_type = problem_type
        self.problem_placeholder = problem_placeholder

class JobRequirements:
    def __init__(self, job_batchnumber_placeholder, jobname_placeholder, jobname, decomp_type_placeholder, decomp_type, instance_idx):
        self.job_batchnumber_placeholder = job_batchnumber_placeholder
        self.jobname_placeholder = jobname_placeholder
        self.jobname = jobname
        self.decomp_type_placeholder = decomp_type_placeholder
        self.decomp_type = decomp_type
        self.instance_idx = instance_idx

class RunAllRequirements:
    def __init__(self, runall_placeholder, number_of_jobs):
        self.runall_placeholder = runall_placeholder
        self.number_of_jobs = number_of_jobs

def copy_batch_script(input_filepath, output_filepath, batch_requirements):

    with open(output_filepath, "w") as output_fs:
        with open(input_filepath, "r") as input_fs:
            for line in input_fs:
                if batch_requirements.instance_placeholder in line:
                    output_fs.write(line.replace(batch_requirements.instance_placeholder, batch_requirements.instance_name))
                elif batch_requirements.problem_placeholder in line:
                    output_fs.write(line.replace(batch_requirements.problem_placeholder, batch_requirements.problem_type))
                else:
                    output_fs.write(line)

def copy_job_script(input_filepath, output_filepath, job_requirements):

    with open(output_filepath, "w") as output_fs:
        with open(input_filepath, "r") as input_fs:
            for line in input_fs:
                new_line = line
                if job_requirements.job_batchnumber_placeholder in line:
                    new_line = new_line.replace(job_requirements.job_batchnumber_placeholder, str(job_requirements.instance_idx))
                if job_requirements.jobname_placeholder in line:
                    new_line = new_line.replace(job_requirements.jobname_placeholder, job_requirements.jobname)
                if job_requirements.decomp_type_placeholder in line:
                    new_line = new_line.replace(job_requirements.decomp_type_placeholder, job_requirements.decomp_type)
                output_fs.write(new_line)

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
    Decomp_types = ["NSGA", "Greedy", "Combined"]
    Constraint_Processing_Type = ["duplicate", "redundant"]


    # "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts"
    Generic_script_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts"
    run_all_generic_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/run_all"

    chmod_script_filepath = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/change_file_permissions"

    #placeholders within generic scripts which are replaced with problem types and instance names
    instance_placeholder = "Instance_XXXXX"
    problem_type_placeholder = "ProblemType_XXXXX"
    job_batchnumber_placeholder = "XXXXX"
    jobname_placeholder = "$JOBNAME"
    decomp_type_placeholder = "$DECOMPTYPE"
    runall_placeholder = "XXXXX"


    for decomp_type in Decomp_types:
        for processing_type in Constraint_Processing_Type:
            for problem_idx, problem_type in enumerate(problem_types):
                # create output folders if they don't already exist
                Path(script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type).mkdir(parents=True,exist_ok=True)
                for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
                    # create bash scripts
                    batch_requirements = BatchRequirements(instance_name, instance_placeholder, problem_type,
                                                           problem_type_placeholder)

                    #BATCH SCRIPTS
                    # duplicate removal script needs to be run twice
                    if processing_type == "duplicate" and decomp_type != "Combined":
                        for duplicate_number in range(1,3):
                            batch_script_filename = "remove_" + processing_type + str(duplicate_number) + "_" + decomp_type + "_constraints_script"
                            processing_batch_script_input_file_path = Generic_script_folder + "/" + "remove_duplicate" + "/" + batch_script_filename
                            processing_batch_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + batch_script_filename + "_" + str(
                                instance_idx)
                            copy_batch_script(processing_batch_script_input_file_path, processing_batch_script_output_file_path, batch_requirements)

                    elif processing_type == "duplicate" and decomp_type == "Combined":
                        batch_script_filename = "remove_" + processing_type + "_" + decomp_type + "_constraints_script"
                        processing_batch_script_input_file_path = Generic_script_folder + "/" + "remove_duplicate" + "/" + batch_script_filename
                        processing_batch_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + batch_script_filename + "_" + str(
                            instance_idx)
                        copy_batch_script(processing_batch_script_input_file_path,
                                          processing_batch_script_output_file_path, batch_requirements)

                    elif processing_type == "redundant" and decomp_type != "Combined":
                        batch_script_filename = "remove_" + processing_type + "_" + decomp_type + "_constraints_script"
                        processing_batch_script_input_file_path = Generic_script_folder + "/" + "remove_redundant" + "/" + batch_script_filename
                        processing_batch_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + batch_script_filename + "_" + str(
                            instance_idx)
                        copy_batch_script(processing_batch_script_input_file_path, processing_batch_script_output_file_path, batch_requirements)

                    #JOB SCRIPTS
                    # duplicate removal script needs to be run twice
                    if processing_type == "duplicate" and decomp_type != "Combined":
                        for duplicate_number in range(1, 3):
                            job_requirements = JobRequirements(job_batchnumber_placeholder, jobname_placeholder, instance_name + "_" +
                                                               "remove_" + processing_type + str(duplicate_number), decomp_type_placeholder, decomp_type,
                                                               instance_idx)
                            job_script_filename = "remove_" + processing_type + str(duplicate_number) + "_constraints_job_script"
                            processing_job_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + job_script_filename + "_" + str(
                                instance_idx)
                            job_script_input_path = Generic_script_folder + "/" + "remove_duplicate" + "/" + job_script_filename
                            # create job scripts
                            copy_job_script(job_script_input_path, processing_job_script_output_file_path, job_requirements)
                    elif processing_type == "duplicate" and decomp_type == "Combined":
                        job_requirements = JobRequirements(job_batchnumber_placeholder, jobname_placeholder,
                                                           instance_name + "_" +
                                                           "remove_" + processing_type,
                                                           decomp_type_placeholder, decomp_type,
                                                           instance_idx)
                        job_script_filename = "remove_" + processing_type + "_" + decomp_type + "_constraints_job_script"
                        processing_job_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + job_script_filename + "_" + str(
                            instance_idx)
                        job_script_input_path = Generic_script_folder + "/" + "remove_duplicate" + "/" + job_script_filename
                        # create job scripts
                        copy_job_script(job_script_input_path, processing_job_script_output_file_path, job_requirements)



                    elif processing_type == "redundant" and decomp_type != "Combined":
                        job_requirements = JobRequirements(job_batchnumber_placeholder, jobname_placeholder, instance_name + "_" +
                                                           "remove_" + processing_type, decomp_type_placeholder, decomp_type,
                                                           instance_idx)
                        job_script_filename = "remove_" + processing_type + "_constraints_job_script"
                        job_script_input_path = Generic_script_folder + "/" + "remove_redundant" + "/" + job_script_filename
                        processing_job_script_output_file_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + job_script_filename +  "_" + str(
                            instance_idx)
                        copy_job_script(job_script_input_path, processing_job_script_output_file_path, job_requirements)

                #RUN ALL SCRIPTS
                # create the run_all script to run each batch job
                runall_requirements = RunAllRequirements(runall_placeholder, len(instance_names[problem_idx]))
                if processing_type == "duplicate" and decomp_type != "Combined":
                    for duplicate_number in range(1, 3):
                        output_run_all_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + "run_all_remove_" + processing_type + str(duplicate_number) + "_constraints"
                        input_run_all_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" +  "remove_" + processing_type + "/" +"run_all_remove_" + processing_type +  str(duplicate_number) + "_constraints"
                        copy_run_all_script(input_run_all_path, output_run_all_path, runall_requirements)
                elif processing_type == "duplicate" and decomp_type == "Combined":
                    output_run_all_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + "run_all_remove_" + processing_type + "_" + decomp_type + "_constraints"
                    input_run_all_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" + "remove_" + processing_type + "/" + "run_all_remove_" + processing_type + "_" + decomp_type + "_constraints"
                    copy_run_all_script(input_run_all_path, output_run_all_path, runall_requirements)
                elif processing_type == "redundant" and decomp_type != "Combined":
                    output_run_all_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + decomp_type + "/" + processing_type + "/" + "run_all_remove_" + processing_type  + "_constraints"
                    input_run_all_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" + "remove_" + processing_type + "/" + "run_all_remove_" + processing_type  + "_constraints"
                    copy_run_all_script(input_run_all_path, output_run_all_path, runall_requirements)


                #RUN EVERY RUN ALL SCRIPTS
                # if the run_every_test script which iterates through all sub-folders and runs the run_all scripts has not been copied across,
                # then copy it in
                if processing_type == "duplicate" and decomp_type != "Combined":
                    for duplicate_number in range(1, 3):
                        run_every_test_filename = "run_every_run_all_remove_" + processing_type + str(duplicate_number) + "_scripts"
                        run_every_test_input_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" + "remove_" + processing_type + "/" + run_every_test_filename
                        run_every_test_output_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + run_every_test_filename
                        if not Path(run_every_test_output_path).is_file():
                            shutil.copyfile(run_every_test_input_path,
                                            run_every_test_output_path)
                elif processing_type == "duplicate" and decomp_type == "Combined":
                    run_every_test_filename = "run_every_run_all_remove_" + processing_type + "_" + decomp_type + "_scripts"
                    run_every_test_input_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" + "remove_" + processing_type + "/" + run_every_test_filename
                    run_every_test_output_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + run_every_test_filename
                    if not Path(run_every_test_output_path).is_file():
                        shutil.copyfile(run_every_test_input_path,
                                        run_every_test_output_path)
                elif processing_type == "redundant" and decomp_type != "Combined":
                    run_every_test_filename = "run_every_run_all_remove_" + processing_type + "_scripts"
                    run_every_test_input_path = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Decomp_Processing_Scripts" + "/" + "remove_" + processing_type + "/" + run_every_test_filename
                    run_every_test_output_path = script_root_folder + "/" + problem_type + "/" + "Process_Decomps" + "/" + run_every_test_filename
                    if not Path(run_every_test_output_path).is_file():
                        shutil.copyfile(run_every_test_input_path,
                                        run_every_test_output_path)

                # if chmod all files hasn't been copied across
                if not Path(script_root_folder + "/" + problem_type + "/" + "change_file_permissions").is_file():
                    shutil.copyfile(chmod_script_filepath,
                                    script_root_folder + "/" + problem_type + "/" + "change_file_permissions")



if __name__ == "__main__":
    main()