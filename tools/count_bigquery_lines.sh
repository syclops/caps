#!/bin/bash

set -o errexit

DIRECTORY=/Volumes/Censys/bigquery
OUTPUT=$DIRECTORY/sample_sizes.txt

echo "------------------------------------$1------------------------------------"
echo "$1 `wc -l $DIRECTORY/$1.dns.rev.sorted.txt`" | awk '{print $1, $2}' >> $OUTPUT
