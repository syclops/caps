import fileinput

bit_size = 0

for line in fileinput.input():
    degree, count = map(int, line.strip().split())
    bit_size += (degree + 1) * count

print(bit_size / 8)
