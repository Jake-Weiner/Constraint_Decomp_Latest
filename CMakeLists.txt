cmake_minimum_required (VERSION 3.2.0) 

project (Constraint_Decomp_Latest VERSION 1.0) #project name

cmake_policy(SET CMP0074 NEW) # instructs cmake to use new or old functionality

#Required - stop if package can't be found
#PATHS - hard coded path to check

#find_path(TBB_INCLUDE_DIRS tbb/tbb.h HINTS ${TBB_DIR} "/home/jake/PhD/Coding_Libraries/tbb/tbb2019_20191006oss/lib/intel64/gcc4.8" PATH_SUFFIXES include)
#find_library(TBB_LIBRARIES tbb${CMAKE_STATIC_LIBRARY_SUFFIX} HINTS ${TBB_DIR} "/home/jake/PhD/Coding_Libraries/tbb/tbb2019_20191006oss/lib/intel64/gcc4.8" PATH_SUFFIXES ${TBB_LIBRARY_SUFFIX})

find_package(Pagmo REQUIRED PATHS /home/jake/PhD/Coding_Libraries/tbb/tbb2019_20191006oss/lib/intel64/gcc4.8)
find_package(Boost 1.67.0 REQUIRED COMPONENTS filesystem)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -g -O0 -m64 -O -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

include_directories("/home/jake/PhD/Edge_Disjoint/c++/boost_1_67_0/Build/include")
include_directories("/home/jake/PhD/Coding_Libraries/cplex/include")
include_directories("/home/jake/PhD/Coding_Libraries/concert/include")
include_directories("/home/jake/PhD/Coding_Libraries/pagmo2-2.11.4/include")
include_directories("/home/jake/PhD/Decomposition/Constraint_Decomp_Latest/include")
include_directories(${Boost_INCLUDE_DIRS})


set(src "/home/jake/PhD/Decomposition/Constraint_Decomp_Latest/src")

link_directories("/home/jake/PhD/Coding_Libraries/cplex/lib/x86-64_linux/static_pic")
link_directories("/home/jake/PhD/Coding_Libraries/concert/lib/x86-64_linux/static_pic")
link_directories("/home/jake/PhD/Edge_Disjoint/c++/boost_1_67_0/Build/lib")
link_directories(${Boost_LIBRARY_DIRS})


add_executable(main_test ${src}/SolveGenericMIP.cpp ${src}/Problem_Adapter.cpp ${src}/main.cpp ${src}/Writer.cpp ${src}/ParamAdapter.cpp ${src}/SolveLaPSO.cpp ${src}/Param.cpp ${src}/MIP_Problem_CPLEX_Solver.cpp ${src}/anyoption.cpp ${src}/CpuTimer.cpp ${src}/MIP_Problem.cpp ${src}/LaPSO.cpp ${src}/ConDecomp_LaPSO_Connector.cpp ${src}/DecompMIP.cpp ${src}/Decomp.cpp ${src}/MIP_to_Hypergraph.cpp ${src}/Hypergraph.cpp ${src}/MIP_Fileparser.cpp ${src}/HG_Edge.cpp ${src}/HG_Node.cpp)

target_link_libraries(main_test Pagmo::pagmo ${Boost_LIBRARIES} concert ilocplex cplex m pthread dl)


