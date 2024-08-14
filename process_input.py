import polars as pl
import os
import time

# This file preprocesses data to ready for import into TopicCluster
# It will takes user's csv, then generates 2 files: input.csv and topics.txt

input_dir = "inputs/"
input_file = "single_qna.csv"
out_csv = 'input.csv'
out_txt = 'topics.txt'
input_path = os.path.join(input_dir, input_file)
out_csv_path = os.path.join(input_dir, out_csv)
out_txt_path = os.path.join(input_dir, out_txt)
BATCH_SIZE = 1000

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
    .filter(pl.col("topic").is_not_null())  # Filter out rows with empty topic value
    .with_columns(pl.col('topic')
    .map_elements(lambda x: x.replace(' ', '-'), return_dtype=pl.Utf8).alias('topic')) # Replace space with dash in topic
)

# Write new data to output csv
print(f'Save new data to {out_csv}...')
query.sink_csv(out_csv_path, batch_size=BATCH_SIZE, include_header=False)
# write distinct categories to topics.txt, each line is a topic
print(f'Writing topics to {out_txt}...')

# Write topics to txt file
unique_topics = query.select('topic').collect()
topics = unique_topics['topic'].unique().to_list()
with open(out_txt_path, 'w') as f:
    for topic in topics:
        f.write(f'{topic}\n')

print(f'Takes {time.perf_counter() - start} seconds to process')
