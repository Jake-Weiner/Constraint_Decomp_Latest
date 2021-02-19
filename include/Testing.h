#ifndef __TESTING__
#define __TESTING__

#include <string>
#include <vector>



using std::vector;
using std::string;

// Forward Declarations
class MIP_Problem;
// namespace mainParam{
//     class Param;
// }
// class ParamAdapter;
class Hypergraph;
class MIPProblemProbe;
struct LaPSOOutputFilenames;

typedef std::pair<int,double> initial_dual_value_pair;

class Testing{
    public:

        Testing(int& argc, const char** argv);
        void RunMIPTesting(MIP_Problem& MP);
        void testEmptyConVec(MIP_Problem& MP, Hypergraph& HG);
    
    private:
        void solveLapso(MIP_Problem& MP, MIPProblemProbe& MPP, Hypergraph& HG, vector<int>& con_relax_vector,
            const LaPSOOutputFilenames& LOF, int decomposition_idx,
            const std::vector<initial_dual_value_pair>& original_intial_dual_value_pairs, bool set_initial_dual_values,
            bool capture_statistics);
        int argc;
        const char** argv;    
    

};

#endif        