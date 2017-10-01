import fileinput
import numpy
import scipy
import scipy.stats

data = []

for line in fileinput.input():
    data.append(int(line.strip()))

print(scipy.stats.entropy(numpy.array(data)))
