from rich import print
from collections import Counter
import seaborn as sns
import polars as pl
import os

input_dir = "inputs/"
input_file = "input.csv"
input_path = os.path.join(input_dir, input_file)

query = (
    pl.scan_csv(input_path)
    .select([
        'question',
        'answer',
        'topic'
    ])
    .collect()
)

print('Processing data...')
print('Total number of data:', f'{len(query):,}')

topics = query['topic'].to_list()
del query
topics = Counter(topics)

print('Number of unique topics:', len(topics))

# sort the topics by frequency
topics = dict(sorted(topics.items(), key=lambda x: x[1], reverse=True))

print('Topic frequency:')
for topic, freq in topics.items():
    print(f'{topic}: {freq:,}')

# Visualize the data with horizontal bar plot
plot_path = 'topics.png'

sns.set_theme(style="darkgrid")
sns.set(rc={'figure.figsize':(19.2, 10.8)})
sns.set_context("paper", font_scale=1.5)
sns.barplot(x=list(topics.values()), y=list(topics.keys())).get_figure().savefig(plot_path)