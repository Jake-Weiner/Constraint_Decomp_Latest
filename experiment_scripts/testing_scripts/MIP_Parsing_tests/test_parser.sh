#!/bin/bash

INPUT_FILE="/home/jake/PhD/Decomposition/Input/dataset/instance_statistics/Instance_Statistics.csv"
OUTPUT_FILE="/home/jake/PhD/Decomposition/Output/testing/Parser_Tests/local_test_results.csv"
executable_path="/home/jake/PhD/Decomposition/Constraint_Decomp_Latest"

if test -f $INPUT_FILE; then
    echo "$INPUT_FILE exists."
else
    echo "$INPUT_FILE does not exist" 
    exit 99
fi

line_number=0

while IFS=, read instance_name MIP_num_var MIP_num_bin MIP_num_int MIP_num_cont MIP_num_cons MIP_num_non_zeroes
do	
	if [ ! "$line_number" -eq "0" ]; then
		echo "starting main_test"
		echo "$instance_name $MIP_num_var $MIP_num_bin $MIP_num_int $MIP_num_cont $MIP_num_cons $MIP_num_non_zeroes"
		$executable_path/main_test --input_root_folder=/home/jake/PhD/Decomposition/Input/dataset \
		--subproblem_var_prop=0.1:0.2:0.3:0.4:0.5:0.6:0.8:0.9 --nsga_pop_size=16 --printLevel=3 \
		--output_root_folder=/home/jake/PhD/Decomposition/Constraint_Decomposition/output/Preliminary_Tests \
		--nsga_vector_root_folder=/home/jake/PhD/Decomposition/Constraint_Decomposition_Output/nsga_tests/Decomps \
		--solve_generic_MIP_output_root_folder=/home/jake/PhD/Decomposition/Constraint_Decomposition/output/generic_MIP \
		--set_ub=40417 --run_nsga_decomp=false --nsga_gen=5 --input_instance_name=$instance_name --decomp_type=NSGA --maxCPU=1000 \
		--set_generic_MIP_time=100 --run_generic_MIP_solver=false --run_lapso=false \
		--run_MIP_Parse_testing=true \
		--parsed_mip_random_seed=false generic_mip_random_seed=false \
		--MIP_Parse_testing_output_filename=/home/jake/PhD/Decomposition/Output/testing/Parser_Tests/Parser_test_Results.csv \
		--MIP_num_var=$MIP_num_var --MIP_num_cons=$MIP_num_cons --MIP_num_bin=$MIP_num_bin --MIP_num_cont=$MIP_num_cont --MIP_num_int=$MIP_num_int --MIP_num_non_zeroes=$MIP_num_non_zeroes 
	fi
((line_number=line_number+1))
done < $INPUT_FILE

