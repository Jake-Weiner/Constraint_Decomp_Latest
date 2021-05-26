# libraries & dataset
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd
from my_plot import set_size

sns.set(style="darkgrid")
# # Using seaborn's style
plt.style.use('seaborn')
plt.style.use('tex')
width = 528.93675 #width of EJOR paper

# plt.rcParams.update(tex_fonts)
# df = pd.read_csv("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/boxplot_test.csv")
# # Usual boxplot
# fig, ax = plt.subplots(1, 1, figsize=set_size(width))
# #
#
# sns.boxplot(data=df, ax=ax)
# ax.set_xticklabels(df.columns, rotation=90)
# # Add jitter with the swarmplot function
# ax.set_ylabel("Predicted Decomposition Scores")
# ax.set_xlabel("Ranking Method")
# plt.savefig("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/boxplot_test.pdf", format='pdf', bbox_inches='tight')
# # plt.savefig("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/boxplot_test.pdf")

fig, axs = plt.subplots(1, 3, figsize=set_size(width))
#training type csv names
training_type_names = ["ND_vs.csv", "FCNF_vs.csv", "SNP_vs.csv"]
for i,training_type_name in enumerate(training_type_names):
    df = pd.read_csv("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/" + training_type_name)
    sns.boxplot(data=df, ax=axs[i])
    axs[i].set_xticklabels(df.columns, rotation=90)
    # Add jitter with the swarmplot function
    if i == 0:
        axs[i].set_ylabel("Predicted Decomposition Scores")
    axs[i].set_xlabel("Ranking Method")
    axs[i].set_title(training_type_name.split("_")[0] + " Test Instances")

plt.tight_layout()
plt.savefig("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/training_type_test.pdf", format='pdf', bbox_inches='tight')

#
#
