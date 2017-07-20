import fileinput

for line in fileinput.input():
    print('.'.join(line.strip().split('.')[::-1]))
