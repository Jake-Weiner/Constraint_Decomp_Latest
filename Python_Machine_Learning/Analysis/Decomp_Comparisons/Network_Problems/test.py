import pandas as pd

df = pd.DataFrame([[1, 2], [4, 5], [7, 8]],
     index=['cobra', 'viper', 'sidewinder'],
     columns=['max_speed', 'shield'])


test_row_vals = df.nsmallest(2,'shield')['max_speed']

# df['shield'][df['max_speed'] == test_row_vals]

# print(type(test_row_vals))

shield_vals = []

for max_speed in test_row_vals:
    print(max_speed)
    print(df.loc[df['max_speed'] == max_speed]['shield'])
    shield_vals.append(df[df['max_speed'] == max_speed]['shield'].values[0])
# df['max_speed'] == test_row_vals

print(shield_vals)