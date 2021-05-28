import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.ticker import LinearLocator
import numpy as np
import pandas as pd
from my_plot import set_size
plt.style.use('seaborn')
plt.style.use('tex')
width = 528.93675/2 #1/2 width of EJOR paper

problem_types = ["network_design", "fixed_cost_network_flow", "supply_network_planning"]
instance_names = [["cost266-UUE.mps", "dfn-bwin-DBE.mps", "germany50-UUM.mps", "ta1-UUM.mps", "ta2-UUE.mps"],
                          ["g200x740.mps", "h50x2450.mps", "h80x6320d.mps", "k16x240b.mps"],
                          ["snp-02-004-104.mps", "snp-04-052-052.mps", "snp-06-004-052.mps", "snp-10-004-052.mps",
                           "snp-10-052-052.mps"]]
output_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Machine_Learning_Outputs/RC_LSP"
features_calculated_folder = "/home/jake/PhD/Machine_Learning/Processed_Results/Features_Calculated"

for problem_type_idx, problem_type in enumerate(problem_types):
    for instance_idx, instance_name in enumerate(instance_names[problem_type_idx]):
        fig, ax = plt.subplots(1, 1, figsize=set_size(width))
        input_csv = "{}/{}/{}/{}/{}".format(features_calculated_folder, problem_type, instance_name, "Features_Collated", "collated.csv")
        df = pd.read_csv(input_csv)

        # df_best_index = df.nsmallest(1, 'Decomp Score')['Decomposition Index']
        # best_decomp_row = df[df['Decomp Score'] == df['Decomp Score'].min()]

        # df_best_index = df['Decomposition Index']
        # # best_predicted_decomp_scores.append(
        # #     true_instance_df[true_instance_df['Decomposition Index'] == decomp_idx]["Decomp Score"].values[0])
        # print(df_best_index.values)
        rc_prop =  df['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats'].to_numpy() * 100
        LSP_prop =  df['Max_Subproblem_Statistics_Var_props'].to_numpy() * 100
        # df_best_index_row = df[df['Decomposition Index'] == df_best_index[0]]
        decomp_score = df['Decomp Score'].to_numpy()
        # ax.scatter(rc_prop, LSP_prop)
        #
        # ax.scatter(best_decomp_row['Relaxed Constraint Prop_Relaxed_Constraint_Statistics_single_stats'] * 100, best_decomp_row['Max_Subproblem_Statistics_Var_props'] * 100, color="red")
        plot = ax.scatter(rc_prop, LSP_prop, c=decomp_score, edgecolor='none', alpha=0.7
                    , cmap=plt.cm.Blues)
        cbar = fig.colorbar(plot)
        cbar.set_label('Decomp Score', rotation = 270, labelpad = 20)
        ax.set_xlabel('Relaxed Constraint (\%)')
        ax.set_ylabel('Largest Subproblem (\%)')
        fig.tight_layout()
        fig.savefig("{}/{}_{}.pdf".format(output_folder, "RC_LSP", instance_name), format="pdf", bbox_inches='tight')
# Customize the z axis.
# ax.set_zlim(0, 1.0)
# ax.zaxis.set_major_locator(LinearLocator(10))
# # A StrMethodFormatter is used automatically
# ax.zaxis.set_major_formatter('{x:.02f}')

# Add a color bar which maps values to colors.
# fig.colorbar(surf, shrink=0.5, aspect=5)

