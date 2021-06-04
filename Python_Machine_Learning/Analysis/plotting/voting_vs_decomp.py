import matplotlib.pyplot as plt
import pandas as pd
from my_plot import set_size
plt.style.use('seaborn')
plt.style.use('tex')
width = 528.93675/2 #  1/2 width of EJOR paper

problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]
output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/OLS_vs_decomp"
features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated/"

for problem_type_idx, problem_type in enumerate(problem_types):
    for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
        fig, ax = plt.subplots(1, 1, figsize=set_size(width))
        df_collated = pd.read_csv("{}/{}/{}/{}/{}".format(features_calculated_folder, problem_type, instance_name, "Features_Collated","collated.csv"))
        df_prediction = pd.read_csv("{}/{}/{}/{}/{}_{}.csv".format(features_calculated_folder, problem_type, instance_name,"Decomp_Method_Scores","OLM_all_problem_types",instance_name))
        decomp_score = df_collated['Decomp Score'].to_numpy()
        prediction_score = df_prediction['ML predicted val'].to_numpy()
        plot = ax.scatter(decomp_score, prediction_score, color="blue")
        ax.set_xlabel('Decomp score')
        ax.set_ylabel('Predicted score')
        fig.tight_layout()
        fig.savefig("{}/prediction_vs_actual_{}.pdf".format(output_folder,instance_name), format = "pdf", bbox_inches='tight')


