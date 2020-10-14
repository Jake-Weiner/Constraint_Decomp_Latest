#include "Writer.h"
#include <fstream>
#include <iostream>

using std::endl;
using std::cout;
using std::ifstream;

void Writer::writeNSGADecompStats(const int var_orig_prob, const int var_largest_sp,
    const int con_orig_prob, const int con_relaxed, const std::string output_filename)
{
    std::ofstream outfile;
    outfile.open(output_filename);
    if (!outfile) {
        std::cout << "error - NSGA Decomp Stats Outfile Not Found" << std::endl;
        return;
    }
    outfile << "Size of original Problem: " << var_orig_prob << std::endl;
    outfile << "Size of largest subproblem: " << var_largest_sp << std::endl;
    outfile << "Number of Constraints in original problem: " << con_orig_prob << std::endl;
    outfile << "Number of Constraints relaxed: " << con_relaxed << std::endl;
    outfile.close();
}

void Writer::writeConVecToFile(const std::vector<double>& con_vec, const std::string& filename)
{
    std::ofstream outfile;
    outfile.open(filename);
    if (!outfile) {
        std::cout << "error - NSGA Decomp Constraint Vector Outfile Not Found" << std::endl;
    }
    for (auto& con_val : con_vec) {
        outfile << con_val << ",";
    }
    outfile.close();
}

void Writer::writeAverageLBPlot(const int var_in_largest_sp, const int con_relaxed, const std::vector<double>& average_LB_tracking,
    const std::vector<double>& timing_tracking, const std::string& filename)
{
    // best lb
    std::ofstream outfile;
    outfile.open(filename);
    if (outfile) {
        outfile << "Size of largest subproblem:"
                << "," << var_in_largest_sp << std::endl;
        outfile << "Number of Constraints relaxed: "
                << "," << con_relaxed << std::endl;
        outfile << "Lower Bound Tracking: "
                << ","
                << "-------" << std::endl;
        for (int i = 0; i < average_LB_tracking.size(); i++) {
            outfile << i << "," << average_LB_tracking[i] << "," << timing_tracking[i] << std::endl;
        }
        outfile.close();
    } else {
        std::cout << "Unable to open Average LB Output File" << std::endl;
    }
}

void Writer::writeBestLBPlot(const int var_in_largest_sp, const int con_relaxed, const std::vector<double>& best_LB_tracking,
    const std::vector<double>& timing_tracking, const std::string& filename)
{
    // best lb
    std::ofstream outfile;
    outfile.open(filename);
    if (outfile) {
        outfile << "Size of largest subproblem:"
                << "," << var_in_largest_sp << std::endl;
        outfile << "Number of Constraints relaxed: "
                << "," << con_relaxed << std::endl;
        outfile << "Lower Bound Tracking: "
                << ","
                << "-------" << std::endl;
        for (int i = 0; i < best_LB_tracking.size(); i++) {
            outfile << i << "," << best_LB_tracking[i] << "," << timing_tracking[i] << std::endl;
        }
        outfile.close();
    } else {
        std::cout << "Unable to open Best LB Output File" << std::endl;
    }
}

void Writer::writeCPLEXResults(const std::string& output_filename, const std::string& instance_name, const CPLEX_Return_struct& CRS){
 
    std::ofstream outfile;
    // append the results to the given filename
    outfile.open(output_filename, std::ofstream::app);
    if (outfile) {
        outfile << instance_name << "," << CRS.bound << "," << CRS.obj_val << endl;
    }
    else{
        cout << "unable to open CPLEX Results Output File" << endl;
    }
    outfile.close();
}

// if File is empty, create it with the column headers, otherwise append..


void Writer::writeMIPParsingResults(const std::string& output_filename, const MIP_Parsing_Test_struct& MPTS){

    cout << "writing MIP Parsing Results to " << output_filename << endl; 
    if (!fileExists(output_filename)){
        std::ofstream outfile;
        outfile.open(output_filename);
        if (outfile){
            outfile << "Instance Name" << "," << "MIP Bound" << "," << "MIP Objective" << ","
            << "Parsed Bound" << "," << "Parsed Objective" << "," << "MIP No. Var" 
            << "," << "Parsed No. Var" << "," << "MIP No. Constr" << "," 
            << "Parsed No. Constr" << "," << "MIP No. Bin" << "," 
            << "Parsed No. Bin" << "," << "MIP No. Int" << "," 
            << "Parsed No. Int" << "," << "MIP No. Cont" << "," << "Parsed No. Cont" 
            << "," << "MIP No. Non Zero" << "," << "Parsed No. Non Zero" << endl;
        }
        else{
            cout << "unable to create output file for MIP Partsing Results in Writer.cpp" << endl;
        }
        outfile.close();
    }
    // if file does exist, append the results

    std::ofstream outfile;
    // append the results to the given filename
    outfile.open(output_filename, std::ofstream::app);
    if (outfile) {
        outfile << MPTS.instance_name << "," << MPTS.MPS_bound << "," << MPTS.MPS_obj_val 
        << "," << MPTS.Parsed_bound << "," << MPTS.Parsed_obj_val << "," << MPTS.MIP_num_var 
        << "," << MPTS.Parsed_num_var << "," << MPTS.MIP_num_const << "," << MPTS.Parsed_num_const 
        << "," << MPTS.MIP_num_bin << "," << MPTS.Parsed_num_bin << "," << MPTS.MIP_num_int 
        << "," << MPTS.Parsed_num_int << "," << MPTS.MIP_num_cont << "," << MPTS.Parsed_num_cont 
        << "," << MPTS.num_non_zeroes << "," << MPTS.Parsed_non_zeroes << endl;
    }
    else{
        cout << "unable to open MIP Parser Test Output File" << endl;
    }
    outfile.close();
}


//
void Writer::writeSubproblemStatistics(const std::string& output_filename, std::shared_ptr<Subproblem_Statistics> ss_ptr){

    // If file doesn't exist populate the column names for features
    if (!fileExists(output_filename)){
        std::ofstream outfile;
        outfile.open(output_filename);
        if (outfile) {
            outfile << "Decomposition Index" << "," << "Average MIP Time (s)"<< "," << "Stddev MIP Time" 
            << "," << "Max MIP Time (s)" << "," << "Average MIP Bound" << "," << "Stddev MIP Bound" 
            << "," << "Average LP Time (s)" << "," << "Stddev LP Time" << "," << "Max LP Time (s)"
            << "," << "Average LP Bound" << "," << "Stddev LP Bound" << "," << "Average Subproblem Size (Prop of Variables)" 
            << "," << "Stddev Subproblem Size" << "," << "Max Subproblem Size" << "," << "Average Block Bin Prop"
            << "," << "Stddev Block Bin Prop" << "," << "Average Block Int Prop" << "," << "Stddev Block Int Prop"
            << "," << "Average Block Cont Prop" << "," << "Stddev Block Cont Prop" << "," << "Average Block Constraint Prop"
            << "," << "Stddev Block Constraint Prop" << "," << "Average Block Equality Prop" <<  "," << "Stddev Block Equality Prop"
            << "," << "Average of Sum Block Objectives" << "," << "Stddev of Sum Block Objectives" 
            << "," << "Average of Average Block RHS" << "," << "Stddev of Average Block RHS"
            << "," << "Average of Average Block Largest RHS/LHS Ratio" << "," << "Stddev of Average Block Largest RHS/LHS Ratio"
            << "," << "Average of Average Block Shape" << "," << "Stddev of Average Block Shape"
            << "," << "Average Block RHS Range" << "," << "Stddev Block RHS Range"
            << "," << "Average Block Density" << "," << "Stddev Block Density"
            << endl;
        }
        else{
            cout << "unable to open Subproblem Statistics File: " << output_filename << endl;
        }
        outfile.close();
    }
    // Then populate file with statistics
    
    std::ofstream outfile;
    outfile.open(output_filename, std::ofstream::app);
    if (outfile) {
        outfile << ss_ptr->decomposition_idx << "," << ss_ptr->average_mip_time << "," << ss_ptr->stddev_mip_time
        << "," << ss_ptr->max_mip_time << "," << ss_ptr->average_mip_obj_soln << "," << ss_ptr->stddev_mip_obj_soln
        << "," << ss_ptr->average_lp_time << "," << ss_ptr->stddev_lp_time << "," << ss_ptr->max_lp_time
        << "," << ss_ptr->average_lp_obj_soln << "," << ss_ptr->stddev_lp_obj_soln << "," << ss_ptr->average_block_variable_prop
        << "," << ss_ptr->stddev_block_variable_prop << "," << ss_ptr->max_block_variable_prop << "," << ss_ptr->average_bin_prop
        << "," << ss_ptr->stddev_bin_prop << "," << ss_ptr->average_int_prop << "," << ss_ptr->stddev_int_prop
        << "," << ss_ptr->average_cont_prop << "," << ss_ptr->stddev_cont_prop << "," << ss_ptr->average_total_constraint_prop
        << "," << ss_ptr->stddev_total_constraint_prop << "," << ss_ptr->average_equality_prop <<  "," << ss_ptr->stddev_equality_prop
        << "," << ss_ptr->average_of_sum_block_obj_values << "," << ss_ptr->stddev_of_sum_block_obj_values
        << "," << ss_ptr->average_of_average_block_RHS_values << "," << ss_ptr->stddev_of_average_block_RHS_values
        << "," << ss_ptr->average_of_average_block_Largest_RHSLHS_ratio << "," << ss_ptr->stddev_of_average_block_Largest_RHSLHS_ratio
        << "," << ss_ptr->average_of_average_block_shapes << "," << ss_ptr->stddev_of_average_block_shapes
        << "," << ss_ptr->average_block_RHS_range << "," << ss_ptr->stddev_block_RHS_range
        << "," << ss_ptr->average_block_density << "," << ss_ptr->stddev_block_density
        << endl;
    }
    else{
        cout << "unable to open Subproblem Statistics File: " << output_filename << endl;
    }
    outfile.close();
    
    cout << "finished writing Subproblem Statistics" << endl;
    
}