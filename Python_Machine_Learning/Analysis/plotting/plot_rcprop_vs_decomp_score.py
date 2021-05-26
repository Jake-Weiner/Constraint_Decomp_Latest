#importing libraries
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from my_plot import set_size
sns.set(style="darkgrid")
# # Using seaborn's style
plt.style.use('seaborn')
plt.style.use('tex')
width = 528.93675 #width of EJOR paper
#global vars
problem_types = ["fixed_cost_network_flow", "network_design", "supply_network_planning"]
instance_names_testing = [["g200x740.mps"], ["dfn-bwin-DBE.mps"], ["snp-10-052-052.mps"]]

features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"
model_comparisons_outputs_root_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/Decomposition_Benefit"

fig, axs = plt.subplots(1, 3, figsize=set_size(width))



for problem_type_idx, problem_type in enumerate(problem_types):
    for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
        df = pd.read_csv(features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv")
        rc_prop_np = df['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats'].to_numpy() * 100
        decomp_score_np = df['Decomp Score'].to_numpy()

        axs[problem_type_idx].set_ylabel("Decomp Score")
        axs[problem_type_idx].set_xlabel("Relaxed Constraint (\%)")
        axs[problem_type_idx].set_title(instance_name)
        # plot prediction vs actual gap
        axs[problem_type_idx].scatter(rc_prop_np, decomp_score_np)

fig.tight_layout()
fig.savefig(model_comparisons_outputs_root_folder + "/Score_vs_RCProp.pdf", format='pdf', bbox_inches='tight')
#
# def plotScoreDistribution():
#
#     for problem_type_idx, problem_type in enumerate(problem_types):
#         for instance_idx, instance_name in enumerate(instance_names_testing[problem_type_idx]):
#             df = pd.read_csv(features_calculated_folder + "/" + problem_type + "/" + instance_name + "/" + "Features_Collated" + "/" + "collated.csv")
#             decomp_index_np = df['Decomposition Index'].to_numpy()
#             decomp_score_np = df['Decomp Score'].to_numpy()
#             plt.figure()
#             plt.ylabel("Decomp Score")
#             plt.xlabel("Decomp Index")
#             # plot prediction vs actual gap
#             plt.scatter(decomp_index_np, decomp_score_np)
#             plt.title('Decomp Score Distribution ' + instance_name)
#             plt.tight_layout()
#             plt.savefig(
#                 model_comparisons_outputs_root_folder + "/" + problem_type + "/" + instance_name + "/" + instance_name.split(".")[0] + "_Score_distribution")
#
# def main():
#     plotRCPropVsScore()
#     # plotScoreDistribution()
#
#
# #store the important features in a list
# if __name__ == "__main__":
#
#     main()