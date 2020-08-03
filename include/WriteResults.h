#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

struct WriteResults{
    int var_in_orig;
    int var_in_largest_sp;
    int con_in_orig;
    int con_relaxed;
    double initial_dual_min;
    double initial_dual_max;
    double final_dual_min;
    double final_dual_max;
    vector<double> best_LB_tracking;
    vector<double> average_LB_tracking;
    vector<double> timing_tracking;
    
    void writeStatsToFile(const string& filename){
        std::ofstream outfile;
        outfile.open(filename);
        outfile << "Size of original Problem: " << var_in_orig << endl;
        outfile << "Size of largest subproblem: " << var_in_largest_sp << endl;
        outfile << "Number of Constraints in original problem: " << con_in_orig << endl;
        outfile << "Number of Constraints relaxed: " << con_relaxed << endl;
        outfile << "Range of Initial Duals: " << initial_dual_max << " - " << initial_dual_max << endl;
        outfile << "Range of Final Duals: " << final_dual_min << " - " << final_dual_max << endl;
        outfile.close();
    }

    void writeBestLBPlot(const string& filename){
        // best lb
        std::ofstream outfile;
        outfile.open(filename);
        outfile << "Size of largest subproblem:" << "," << var_in_largest_sp << endl;
        outfile << "Number of Constraints relaxed: " << "," << con_relaxed << endl;
        outfile << "Lower Bound Tracking: " << "," << "-------"<< endl;
        for (int i=0; i<best_LB_tracking.size(); i++){
            outfile << i << "," << best_LB_tracking[i] << "," << timing_tracking[i] << endl;
        }
        outfile.close();
    }

    void writeAverageLBPlot(const string& filename){
        // best lb
        std::ofstream outfile;
        outfile.open(filename);
        outfile << "Size of largest subproblem:" << "," << var_in_largest_sp << endl;
        outfile << "Number of Constraints relaxed: " << "," << con_relaxed << endl;
        outfile << "Lower Bound Tracking: " << "," << "-------"<< endl;
        for (int i=0; i<average_LB_tracking.size(); i++){
            outfile << i << "," << average_LB_tracking[i] << "," << timing_tracking[i] << endl;
        }
        outfile.close();
    }
};