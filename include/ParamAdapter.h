#ifndef __PARAMADAPTER__
#define __PARAMADAPTER__

#include "Param.h"

#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>

using std::string;
using std::vector;

enum DecompType { NSGA, CPLEX };

class ParamAdapter{
    
    public:
        ParamAdapter(const mainParam::Param& p);
        bool get_generic_MIP_Solver_Flag(){
            return generic_MIP_Solver_Flag;
        }
        bool get_run_nsga_decomp_flag(){
            return run_nsga_decomp_flag;
        }
        bool get_run_lapso_flag(){
            return run_lapso_flag;
        }

        bool get_generic_MIP_randomSeed_flag(){
            return generic_MIP_randomSeed_flag;
        }

        bool get_parsed_MIP_randomSeed_flag(){
            return parsed_MIP_randomSeed_flag;
        }

        bool get_test_greedy_decomp_flag(){
            return test_greedy_decomp_flag;
        }

        bool get_test_random_decomp_flag(){
            return test_random_decomp_flag;
        }

        bool get_MIP_Parse_Test_Exception_flag(){
            return MIP_Parse_Test_Exception_flag;
        }

        bool get_run_MIP_Parse_testing_flag(){
            return run_MIP_Parse_testing_flag;
        }

        bool get_run_Hypergraph_Partitioning_testing_flag(){
            return run_Hypergraph_Partitioning_testing_flag;
        }

        bool get_run_NSGA_testing_flag(){
            return run_NSGA_testing_flag;
        }

        std::vector<double> get_nsga_props(){
            return nsga_props;
        }

        std::string get_NSGA_decomp_plot_filename(){
            return NSGA_decomp_plot_filename;
        }

        string getProblemFilename(){
            return Problem_File;
        }

    private:
        void createInputFilePaths(const mainParam::Param& p);
        void createTestFlags(const mainParam::Param& p);
        void create_nsga_props(const mainParam::Param& p);
        void create_NSGA_decomp_plot_filename(const mainParam::Param& p);
        string NSGA_decomp_plot_filename;
        vector<double>  nsga_props;
        bool generic_MIP_Solver_Flag = false; 
        bool run_nsga_decomp_flag = false;
        bool run_lapso_flag = false;
        bool test_greedy_decomp_flag = false;
        bool test_random_decomp_flag = false;
        bool MIP_Parse_Test_Exception_flag = false;
        bool generic_MIP_randomSeed_flag = true;
        bool parsed_MIP_randomSeed_flag = true;
        bool run_MIP_Parse_testing_flag = false;
        bool run_Hypergraph_Partitioning_testing_flag = false;
        bool run_NSGA_testing_flag = false;
        bool getBoolVal(const char* input);

        string Problem_File;
        std::map<std::string, DecompType> str_to_DT = {{"NSGA", NSGA}, {"CPLEX", CPLEX}};
        std::map<std::string, bool> char_to_BOOL = {{"true", true} , {"false" , false}};
};

#endif