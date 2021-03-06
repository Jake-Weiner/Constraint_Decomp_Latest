#ifndef __WRITER__
#define __WRITER__

#include <vector>
#include <string>
#include "Util.h"
#include "DecompositionStatistics.h"
#include <memory>

using DecompositionStatistics::RelaxedConstraints;
using DecompositionStatistics::Subproblems;
using DecompositionStatistics::Instance;
using DecompositionStatistics::LROutputs;

class Writer{

    struct relaxedConstraintStatistics
    {
        double average_non_zero_perc; // = sum(average_non_zero_prop) / number_relaxed_constraints: where average_non_zero_prop = non_zero / total var
        double equality_perc; // = prop of relaxed constraints which are equality constraints
        double non_equality_perc; // = 1 - equality_perc
        double average_ratio_largest_smallest_coeff; // ratio between abs(largest_coeff) and abs(smallest_coeff)
        double constraints_relaxed_perc; // percentage of the number of constraints relaxed
    };

    struct relaxedProblemsStatistics
    {
        double variance;
        double max;
        double min;
        double count;
        // double binary_count;
        // double integer_count;
    };
    
    struct subproblemSolverStatistics
    {
        double max_mip_time;
        double max_sp_lp_time;
        double average_mip_time;
        double min_mip_time;
        double min_lp_time;
        double mip_time_variance;
        double lp_time_variance;
        
    };

    public:
        Writer(){};
        void writeFinalResults();
        void writeStats();
        void writeAverageLBPlot(const int var_in_largest_sp, const int con_relaxed, const std::vector<double>& average_LB_tracking,
            const std::vector<double>& timing_tracking, const std::string& filename);
         void writeBestLBPlot(const int var_in_largest_sp, const int con_relaxed, const std::vector<double>& best_LB_tracking,
            const std::vector<double>& timing_tracking, const std::string& filename);
        void writeNSGADecompStats(const int var_orig_prob, const int var_largest_sp,  
        const int con_orig_prob, const int con_relaxed , const std::string output_filename);
        void writeConVecToFile(const std::vector<double>& con_vec, const std::string& con_vec_filename);
        // void writeDecompInfoToFile(HG, con_relax_info.first, decomp_info_filename);
        void writeRelaxedConstraintStatistics(){};
        void writeCPLEXResults(const std::string& output_filename, const std::string& instnace_name, const CPLEX_Return_struct& CRS);
        void writeMIPParsingResults(const std::string& output_filename, const MIP_Parsing_Test_struct& MPTS);
        void writeSubproblemStatistics(const std::string& output_filename, std::shared_ptr<Subproblems> ss_ptr);
        void writeRawSubproblemStatistics(const LaPSOOutputFilenames& LOF, std::shared_ptr<Subproblems> ss_ptr);    
        void writeRawRelaxedConstraintStatistics(const LaPSOOutputFilenames& LOF, std::shared_ptr<RelaxedConstraints> rcs_ptr);
        void writeInstanceStatistics(const LaPSOOutputFilenames& LOF, std::shared_ptr<Instance> ins_ptr);
        void writeRelaxedConstraintSingleValues(const LaPSOOutputFilenames& LOF,  std::shared_ptr<RelaxedConstraints> rcs_ptr);
        void writeLROutputs(const LaPSOOutputFilenames& LOF, std::shared_ptr<LROutputs> lro_ptr);
        void writeLPOutputs(const string& output_filename, double bound, double time);
    
    private:

        // generic raw subproblem statistics output
        // required filename, decomposition idx, vector of data
        template<class T1>
        void genericRawSubproblemOutput(const string& output_filename, int decomposition_idx, vector<T1> raw_data){      
            std::cout << "writing Raw Subproblem Statistics to " << output_filename << endl; 
            // file doesn't exist
            if (!fileExists(output_filename)){
                std::ofstream outfile;
                outfile.open(output_filename);
                if (outfile){
                    outfile << "Decomposition Index" << "," << "Raw Data" << std::endl;
                }
                outfile.close();
            }

            // Then populate file with statistics
            std::ofstream outfile;
            outfile.open(output_filename, std::ofstream::app);
            if (outfile) {
                outfile << decomposition_idx;
                for (const auto& val : raw_data){
                    outfile << "," << val;
                }
                outfile << endl;
            }
        }
        template<class T1>
        void genericRawInstanceOutput(const string& output_filename, vector<T1> raw_data){
            std::cout << "writing Raw Instance Statistics to " << output_filename << endl; 
            // file doesn't exist
            if (!fileExists(output_filename)){
                std::ofstream outfile;
                outfile.open(output_filename);
                if (outfile){
                    outfile << "Raw Data" << std::endl;
                }
                outfile.close();
            }

            // Then populate file with statistics
            std::ofstream outfile;
            outfile.open(output_filename, std::ofstream::app);
            if (outfile) {
                for (const auto& val : raw_data){
                    outfile << val << endl;
                }
            }
        }

        
       
};


#endif



