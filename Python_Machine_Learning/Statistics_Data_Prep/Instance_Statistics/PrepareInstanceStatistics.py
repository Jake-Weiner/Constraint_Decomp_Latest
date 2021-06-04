import csv
import pandas as pd
from pathlib import Path
import os
from sklearn.preprocessing import minmax_scale
import collections

problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "random_MIPLIB"]
updated_problem_type_names = ["Network Design", "Fixed Cost Network Flow", "Supply Network Planning",
                              "Random MIPLIB"]
instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                  ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                  ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                   "snp-10-052-052.mps"],
                  ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps",
                   "neos-4387871-tavua.mps",
                   "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]
raw_results_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
processed_results_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Instance_Statistics"
output_folder = processed_results_root_folder + "/" + "Instance_Features"
Path(output_folder).mkdir(parents=True, exist_ok=True)

#returns instance num_var, num_constr, num_bin/int, num_nz, num_equality
def getInstanceStatistics(input_filename):
    #current col idx's of values in csv files
    Num_Var_idx = 0
    Num_Constr_idx = 1
    Num_Bin_idx = 2
    Num_Int_idx = 3
    Num_Non_Zeroes_idx = 5
    Num_Equality_idx = 16
    Num_Var, Num_Constr, Num_Bin, Num_Int, Num_Non_Zeroes, Num_Equality = [0 for i in range(6)]

    with open(input_filename, "r") as input_fs:
        csvreader = csv.reader(input_fs, delimiter=',')
        for line_number, line_split in enumerate(csvreader):
            if line_number == 1:
                Num_Var = float(line_split[Num_Var_idx])
                Num_Constr = float(line_split[Num_Constr_idx])
                Num_Bin = float(line_split[Num_Bin_idx])
                Num_Int = float(line_split[Num_Int_idx])
                Num_Non_Zeroes = float(line_split[Num_Non_Zeroes_idx])
                Num_Equality = float(line_split[Num_Equality_idx])

    # create named tuple
    InstanceStatTuple = collections.namedtuple('RCNormReq', 'Num_Var Num_Constr Num_Bin Num_Int Num_Non_Zeroes Num_Equality')
    return InstanceStatTuple(Num_Var = Num_Var, Num_Constr = Num_Constr, Num_Bin = Num_Bin, Num_Int = Num_Int, Num_Non_Zeroes = Num_Non_Zeroes, Num_Equality = Num_Equality)

def writeFeatures(output_path,feature_list, instance_name, problem_idx):
    if not Path(output_path).is_file():
        with open(output_path, "w") as output_fs:
            output_fs.write("instance_name" + "," + "problem_type" + ",")
            for feature_tuple in feature_list[0:len(feature_list) - 2]:
                print(feature_tuple[0])
                output_fs.write(feature_tuple[0] + ",")
            output_fs.write(feature_list[len(feature_list) - 1][0] + "\n")

    with open(output_path, "a+") as output_fs:
        output_fs.write(instance_name + "," + updated_problem_type_names[problem_idx] + ",")
        for feature_tuple in feature_list[0:len(feature_list) - 2]:
            output_fs.write(str(feature_tuple[1]) + ",")
        output_fs.write(str(feature_list[len(feature_list) - 1][1]) + "\n")

def main():

    # list of features which we want to get only average and stddev. Every other
    # we also want to get min and max

    # 20 features for instance classificate in total.
    # 2 (ave, stddev) for each raw result + Density, shape, equality_prop, Bin_prop, Int_prop, Bin_Int_prop
    #include Obj_terms when it is working again
    # raw_result_filenames = ["Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv", "Largest_RHSLHS.csv", "Non_Zeroes.csv", "Obj_Terms.csv", "RHS_Vals.csv", "Sum_RHSLHS.csv"]

    # Files requiring Min/max normalisation - Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv"
    # Non-zeroes should be normalised against non-zeroes found in instance
    raw_result_filenames = ["Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv", "Largest_RHSLHS.csv", "Non_Zeroes.csv",
                            "RHS_Vals.csv", "Sum_RHSLHS.csv"]
    # Files requiring Min/max scaling
    min_max_files = ["Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv", "Largest_RHSLHS.csv", "RHS_Vals.csv", "Sum_RHSLHS.csv"]
    # Non zero file requires normalisation against total non zeroes in the instance
    general_norm_files = ["Non_Zeroes.csv"]
    scaling_feature_types = ["mean", "stddev"]
    other_features = ["Density", "Shape", "Equality_Prop", "Bin_Prop", "Int_Prop", "Bin_Int_Prop"]

    #problem analysis can be for network problems or all instances including random MIPLIB
    problem_type_analysis = "Network_Problems"
    problem_types_analysed = []
    if problem_type_analysis == "Network_Problems":
        problem_types_analysed = problem_types[0:len(problem_types) - 1]
    elif problem_type_analysis == "All_Problems":
        problem_types_analysed = problem_types[0:len(problem_types)]

    output_path = "{}/Instance_Features_{}.csv".format(output_folder, problem_type_analysis)
    # delete output file if it exists
    if Path(output_path).is_file():
        os.remove(output_path)

    # # features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    for problem_idx, problem_type in enumerate(problem_types_analysed):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            feature_list = []
            Instance_Statistics = getInstanceStatistics(
                raw_results_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv")
            #scale, calculate and capture mean,stddev of relevant statistics
            for normalisation_req_fn in (min_max_files + general_norm_files):
                
                stat_type = normalisation_req_fn.replace(".csv","")
                # print(raw_results_filename.replace(".csv",""))
                filename_full_path = "{}/{}/{}/Instance_Statistics/{}".format(raw_results_root_folder, problem_type, instance_name, normalisation_req_fn)
                df_temp = pd.read_csv(filename_full_path)
                # scale the data using Min-Max normalisation
                if normalisation_req_fn in min_max_files:
                    df_temp['Scaled Data'] = minmax_scale(df_temp['Raw Data'])
                else:
                    if normalisation_req_fn ==  "Non_Zeroes.csv":
                        df_temp['Scaled Data'] = df_temp['Raw Data'] / Instance_Statistics.Num_Non_Zeroes
                for feature_type in scaling_feature_types:
                    if feature_type == "mean":
                        feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].mean()))
                    elif feature_type == "stddev":
                        feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].std()))
            
            # for normalisation_filename in normalisation_fileanames:
            #     stat_type = normalisation_filename.replace(".csv", "")
            #     filename_full_path = "{}/{}/{}/Instance_Statistics/{}".format(raw_results_root_folder, problem_type,
            #                                                                   instance_name, normalisation_filename)
            #     df_temp = pd.read_csv(filename_full_path)
            #     # scale the data using Min-Max normalisation
            #     df_temp['Scaled Data'] = minmax_scale(df_temp['Raw Data'])
            #     for feature_type in scaling_feature_types:
            #         if feature_type == "mean":
            #             feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].mean()))
            #         elif feature_type == "stddev":
            #             feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].std()))
                
            for feature_type in other_features:
                feature_val = 0
                if feature_type == 'Density':
                    feature_val = Instance_Statistics.Num_Non_Zeroes / (Instance_Statistics.Num_Var * Instance_Statistics.Num_Constr)
                elif feature_type == 'Shape':
                    feature_val = Instance_Statistics.Num_Var / Instance_Statistics.Num_Constr
                elif feature_type == 'Equality_Prop':
                    feature_val = Instance_Statistics.Num_Equality / Instance_Statistics.Num_Constr
                elif feature_type == 'Bin_Prop':
                    feature_val = Instance_Statistics.Num_Bin / Instance_Statistics.Num_Var
                elif feature_type == 'Int_Prop':
                    feature_val = Instance_Statistics.Num_Int / Instance_Statistics.Num_Var
                elif feature_type == 'Bin_Int_Prop':
                    feature_val = (Instance_Statistics.Num_Bin + Instance_Statistics.Num_Int) / Instance_Statistics.Num_Var
                feature_list.append((feature_type, feature_val))

            writeFeatures(output_path, feature_list,instance_name, problem_idx)


if __name__ == "__main__":

    main()