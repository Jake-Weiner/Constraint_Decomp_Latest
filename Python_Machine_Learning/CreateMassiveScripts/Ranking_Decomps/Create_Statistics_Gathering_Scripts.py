
from pathlib import Path
import shutil


class BatchRequirements:
    def __init__(self, instance_name, instance_placeholder, problem_type, problem_placeholder, batch_number, batch_number_placeholder):
        self.instance_name = instance_name
        self.instance_placeholder = instance_placeholder
        self.problem_type = problem_type
        self.problem_placeholder = problem_placeholder
        self.batch_number = batch_number
        self.batch_number_placeholder = batch_number_placeholder

class JobRequirements:
    def __init__(self, job_batchnumber_placeholder, batch_number, run_time_placeholder, runtime,
                 memory_placeholder, memory, job_name_placeholder, job_name):
        self.job_batchnumber_placeholder = job_batchnumber_placeholder
        self.batch_number = batch_number
        self.run_time_placeholder = run_time_placeholder
        self.runtime = runtime
        self.memory_placeholder = memory_placeholder
        self.memory = memory
        self.job_name = job_name
        self.job_name_placeholder = job_name_placeholder


class RunAllRequirements:
    def __init__(self, runall_placeholder, number_of_jobs):
        self.runall_placeholder = runall_placeholder
        self.number_of_jobs = number_of_jobs

#change generic batch scipts by filling in generic placeholders with specific requirements
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

#change generic job scipts by filling in generic placeholders with specific requirements
def copy_job_script(input_filepath, output_filepath, job_requirements):

    with open(output_filepath, "w") as output_fs:
        with open(input_filepath, "r") as input_fs:
            for line in input_fs:
                new_line = line
                if job_requirements.job_batchnumber_placeholder in line:
                    new_line = new_line.replace(job_requirements.job_batchnumber_placeholder, str(job_requirements.batch_number))
                if job_requirements.job_name_placeholder in line:
                    new_line = new_line.replace(job_requirements.job_name_placeholder, job_requirements.job_name)
                if job_requirements.run_time_placeholder in line:
                    new_line = new_line.replace(job_requirements.run_time_placeholder, job_requirements.runtime)
                if job_requirements.memory_placeholder in line:
                    new_line = new_line.replace(job_requirements.memory_placeholder, job_requirements.memory)
                output_fs.write(new_line)

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

    output_root_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts"
    action_name = "Gather_Statistics"

    Generic_script_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/experiment_scripts/generic_scripts/Gather_Statistics_Scripts/Ranking"

    instance_placeholder = "Instance_XXXXX"
    problem_type_placeholder = "ProblemType_XXXXX"
    batch_number_placeholder = "BATCHNUMBER"
    job_batchnumber_placeholder = "XXXXX"
    jobname_placeholder = "$JOBNAME"
    runtime_placeholder = "$RUNTIME"
    memory_placeholder = "$MEMORY"
    runall_placeholder = "XXXXX"

    number_of_batches = 10
    problem_types_runtimes = ["72:00:00", "72:00:00", "72:00:00"]
    problem_types_memory = ["8192", "8192", "16384"]

    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exist

        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            # create bash scripts
            output_folder = output_root_folder + "/" + problem_type + "/" + action_name + "/Ranking" + "/" + instance_name

            Path(output_folder).mkdir(
                parents=True, exist_ok=True)
            for batch_number in range(0,number_of_batches):
                batch_requirements = BatchRequirements(instance_name, instance_placeholder, problem_type,
                                                       problem_type_placeholder, batch_number, batch_number_placeholder)
                generic_batch_path = Generic_script_folder + "/batch0"
                specific_batch_output_path = output_folder + "/batch" + str(batch_number)
                copy_batch_script(generic_batch_path,
                                  specific_batch_output_path, batch_requirements)

                # create job scripts
                job_requirements = JobRequirements(job_batchnumber_placeholder, batch_number,
                                                   runtime_placeholder, problem_types_runtimes[problem_idx],
                                                   memory_placeholder, problem_types_memory[problem_idx],
                                                   jobname_placeholder,
                                                   instance_name + "_" + str(batch_number) + "_Gather_Statistics")


                processing_job_script_output_file_path = output_folder + "/job" + str(
                    batch_number) + "_script"
                job_script_input_path = Generic_script_folder + "/" + "job0_script"
                # create job scripts
                copy_job_script(job_script_input_path, processing_job_script_output_file_path, job_requirements)

            # create run_all scripts
            runall_requirements = RunAllRequirements(runall_placeholder, number_of_batches)
            output_run_all_path = output_folder + "/run_all"
            input_run_all_path = Generic_script_folder + "/" + "run_all"
            copy_run_all_script(input_run_all_path, output_run_all_path, runall_requirements)
            # create the run_all script to run each batch job


        # if the run_every_test script which iterates through all sub-folders and runs the run_all scripts has not been copied across,
        # then copy it in
        if not Path(
                output_root_folder + "/" + problem_type + "/" + action_name + "/Ranking" + "/" + "run_every_test").is_file():
            shutil.copyfile(Generic_script_folder + "/" + "run_every_test",
                            output_root_folder + "/" + problem_type + "/" + action_name + "/Ranking" + "/" + "run_every_test")

        # if chmod all files hasn't been copied across
        if not Path(output_root_folder + "/" + problem_type + "/" + action_name + "/Ranking" + "/" + "change_file_permissions").is_file():
            shutil.copyfile(Generic_script_folder + "/" + "change_file_permissions",
                            output_root_folder + "/" + problem_type + "/" + action_name + "/Ranking" + "/" + "change_file_permissions")


if __name__ == "__main__":
    main()