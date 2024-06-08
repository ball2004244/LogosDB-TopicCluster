# This file preprocesses data to ready for import into TopicCluster
# It will takes user's csv, then generates 2 files: input.csv and topics.txt
import polars as pl
import os
import time

input_dir = "inputs/"
input_file = "single_qna.csv"
out_csv = 'input.csv'
out_txt = 'topics.txt'
input_path = os.path.join(input_dir, input_file)
out_csv_path = os.path.join(input_dir, out_csv)
out_txt_path = os.path.join(input_dir, out_txt)

start = time.perf_counter()
print(f'Reformatting {input_file}')
query = (
    pl.scan_csv(input_path) # lazy operation, allow large file processing
    .select([
        'Question',
        'Answer',
        'Category'
    ])
    .rename({
        'Question': 'question',
        'Answer': 'answer',
        'Category': 'topic'
    })
)

df = query.collect()

# Write new data to output csv
print(f'Save new data to {out_csv}...')
df.write_csv(out_csv_path)

# write distinct categories to topics.txt, each line is a topic
print('Writing topics.txt...')
categories = df['topic'].unique().to_list()

# convert all space to dash in category because docker command does not accept space
categories = [cat.strip().replace(' ', '-') for cat in categories]
with open(out_txt_path, 'w') as f:
    for cat in categories:
        f.write(f'{cat}\n')

print(f'Takes {time.perf_counter() - start} seconds to process')