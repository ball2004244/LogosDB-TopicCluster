from rich import print
from collections import Counter
import seaborn as sns
import polars as pl
import os

input_dir = "inputs/"
input_file = "input.csv"
input_path = os.path.join(input_dir, input_file)

query = (
    pl.scan_csv(input_path, has_header=False, with_column_names=lambda cols: ['question', 'answer', 'topic'])
    .select([
        'question',
        'answer',
        'topic'
    ])
    .collect()
)

print('Processing data...')
print('Total number of data:', f'{len(query):,}')

# Find rows with any null values
null_rows_mask = query.with_columns([pl.col(column).is_null() for column in query.columns]).sum_horizontal() > 0
null_rows = query.filter(null_rows_mask)
query = query.with_columns(pl.arange(0, query.height).alias("index"))
null_rows_with_index = query.filter(null_rows_mask)
null_indices_arr = null_rows_with_index.select("index").to_numpy().flatten().tolist()

print(f'There are {len(null_indices_arr)} rows with null values.')
# print('Rows with null values:', null_indices_arr)


print(f'Save topic Automotive into a new csv file...')
# save data of 'Automotive' topic into a new csv called Automotive.csv
automotive = query.filter(pl.col('topic') == 'Automotive')
automotive_path = 'Automotive.csv'
automotive.write_csv(automotive_path)
# sorted_automotive = automotive.sort('question')
# sorted_automotive.write_csv(automotive_path)
print(f'Saved {len(automotive)} rows with topic "Automotive" into {automotive_path}.')


topics = query['topic'].to_list()
del query
topics = Counter(topics)

print('Number of unique topics:', len(topics))

# sort the topics by frequency
topics = dict(sorted(topics.items(), key=lambda x: x[1], reverse=True))

print('\nTopic frequency:')
for topic, freq in topics.items():
    print(f'{topic}: {freq:,}')

# Visualize the data with horizontal bar plot
plot_path = 'topics.png'

sns.set_theme(style="darkgrid", rc={'figure.figsize':(19.2, 10.8)})
sns.set_context("paper", font_scale=1.5)
sns.barplot(x=list(topics.values()), y=list(topics.keys())).get_figure().savefig(plot_path)