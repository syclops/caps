import fileinput

for line in fileinput.input():
    degree, count = map(int, line.strip().split())
    for _ in range(count):
        print(degree)
