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


df = pd.read_csv("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/boxplot_test.csv")
# Usual boxplot
fig, ax = plt.subplots(1, 1, figsize=set_size(width))
#

sns.boxplot(data=df, ax=ax)
ax.set_xticklabels(df.columns, rotation=90)
# Add jitter with the swarmplot function
ax.set_ylabel("Predicted Decomposition Scores")
ax.set_xlabel("Ranking Method")
plt.savefig("/home/jake/PhD/Machine_Learning/Processed_Results/Model_Comparisons/boxplot_test.pdf", format='pdf', bbox_inches='tight')
