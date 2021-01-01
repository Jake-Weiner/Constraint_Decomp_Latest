#run script from root output folder
import os
import sys

def main():

    root_output_folder = "/home/jake/PhD/Decomposition/Massive/Machine_Learning/output/network_flow/cost266-UUE.mps"
    normalised_data_folder = root_output_folder + "/Normalised_Data"
    features_calculated_folder = root_output_folder + "/Features_Calculated"
    #create normalised data folder
    try:
        os.mkdir(normalised_data_folder)
    except:
        print("Normalised Data Folder Already Exists")

    #create required output folders
    try:
        os.mkdir(normalised_data_folder + "/Instance_Statistics")
        os.mkdir(normalised_data_folder + "/LP_Bound")
        os.mkdir(normalised_data_folder + "/LROutputs")
        os.mkdir(normalised_data_folder + "/Relaxed_Constraint_Statistics")
        os.mkdir(normalised_data_folder + "/Subproblem_Statistics")
    except:
        print("Normalised Data Subfolders already exits")

    # create required features_calculated folders
    try:
        os.mkdir(features_calculated_folder + "/Instance_Statistics")
        os.mkdir(features_calculated_folder + "/LP_Bound")
        os.mkdir(features_calculated_folder + "/LROutputs")
        os.mkdir(features_calculated_folder + "/Relaxed_Constraint_Statistics")
        os.mkdir(features_calculated_folder + "/Subproblem_Statistics")
    except:
        print("Normalised Data Subfolders already exits")

if __name__ == "__main__":

    main()