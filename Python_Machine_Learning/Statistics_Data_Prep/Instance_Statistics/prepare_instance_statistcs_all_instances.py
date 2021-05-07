from sklearn.preprocessing import MinMaxScaler
import statistics
import csv
import os
import pandas as pd
from pathlib import Path
from sklearn.preprocessing import minmax_scale
import collections
#get instance statistics which are required to normalise the relaxed constraint statistics
def getRequiredNormVals(input_filename):

    RCNormReq = collections.namedtuple('RCNormReq', 'Num_Var Num_Constr Num_Bin Num_Int Num_Non_Zeroes Num_Equality')
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


    Requirements = RCNormReq(Num_Var = Num_Var, Num_Constr = Num_Constr, Num_Bin = Num_Bin, Num_Int = Num_Int, Num_Non_Zeroes = Num_Non_Zeroes, Num_Equality = Num_Equality)

    return Requirements

def main():

    # list of features which we want to get only average and stddev. Every other
    # we also want to get min and max

    # 20 features for instance classificate in total.
    # 2 (ave, stddev) for each raw result + Density, shape, equality_prop, Bin_prop, Int_prop, Bin_Int_prop
    #include Obj_terms when it is working again
    # raw_result_filenames = ["Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv", "Largest_RHSLHS.csv", "Non_Zeroes.csv", "Obj_Terms.csv", "RHS_Vals.csv", "Sum_RHSLHS.csv"]
    raw_result_filenames = ["Constr_Sum_Abs_Obj.csv", "Constr_Sum_Obj.csv", "Largest_RHSLHS.csv", "Non_Zeroes.csv", "RHS_Vals.csv", "Sum_RHSLHS.csv"]
    raw_result_feature_types = ["mean", "stddev"]
    other_features = ["Density", "Shape", "Equality_Prop", "Bin_Prop", "Int_Prop", "Bin_Int_Prop"]
    


    problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning", "random_MIPLIB"]
    instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                      ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                      ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                       "snp-10-052-052.mps"],  
                      ["blp-ic98.mps", "dws008-01.mps", "30n20b8.mps", "air03.mps", "traininstance2.mps", "neos-4387871-tavua.mps",
                                                   "neos-4338804-snowy.mps", "air05.mps", "neos-4954672-berkel.mps", "splice1k1.mps"]]
    raw_results_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Massive_Outputs"
    processed_results_root_folder = "/media/jake/Jakes_Harddrive/Machine_Learning/Processed_Results"
    output_folder = processed_results_root_folder + "/" + "Instance_Features"
    Path(output_folder).mkdir(parents=True, exist_ok=True)
    output_path = output_folder + "/" + "Instance_Features_all.csv"
    # features_calculated_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/Processed_Results/Features_Calculated"
    for problem_idx, problem_type in enumerate(problem_types):
        # create output folders if they don't already exists
        for instance_idx, instance_name in enumerate(instance_names[problem_idx]):
            feature_list = []
            #scale, calculate and capture mean,stddev of relevant statistics
            for raw_results_filename in raw_result_filenames:
                # print(instance_name)
                stat_type = raw_results_filename.replace(".csv","")
                # print(raw_results_filename.replace(".csv",""))
                filename_full_path = raw_results_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + raw_results_filename
                df_temp = pd.read_csv(filename_full_path)
                df_temp['Scaled Data'] = minmax_scale(df_temp['Raw Data'])
                for feature_type in raw_result_feature_types:
                    if feature_type == "mean":
                        feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].mean()))
                    elif feature_type == "stddev":
                        feature_list.append((stat_type + "_" + feature_type, df_temp['Scaled Data'].std()))
            print(feature_list)
             # = scaler.transform(df_temp['Raw Data'])
            for feature_type in other_features:
                Instance_Statistics = getRequiredNormVals( raw_results_root_folder + "/" + problem_type + "/" + instance_name + "/" + "Instance_Statistics" + "/" + "Instance_Statistics.csv")
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

            print(feature_list)
            # if file has not been created, create it with appropriate headings
            if not Path(output_path).is_file():
                with open(output_folder + "/" + "Instance_Features_all.csv", "w") as output_fs:
                    output_fs.write("instance_name" + "," + "problem_type" + ",")
                    for feature_tuple in feature_list[0:len(feature_list)-2]:
                        print(feature_tuple[0])
                        output_fs.write(feature_tuple[0] + ",")
                    output_fs.write(feature_list[len(feature_list) - 1][0] + "\n")

            with open(output_folder + "/" + "Instance_Features_all.csv", "a+") as output_fs:
                output_fs.write(instance_name + "," + problem_type + ",")
                for feature_tuple in feature_list[0:len(feature_list) - 2]:
                    output_fs.write(str(feature_tuple[1]) + ",")
                output_fs.write(str(feature_list[len(feature_list) - 1][1]) + "\n")





if __name__ == "__main__":

    main()