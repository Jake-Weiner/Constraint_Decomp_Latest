#include "Writer.h"
#include <fstream>
#include <iostream>

using std::endl;
using std::cout;

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
