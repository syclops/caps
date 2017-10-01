import collections
import fileinput

counts = collections.defaultdict(int)

for line in fileinput.input():
    if '"' not in line:
        continue
    char = line.split('"')[1]
    counts[char] += 1

for k, v in counts.items():
    print('{} {}'.format(k, v))
