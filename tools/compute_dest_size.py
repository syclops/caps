import argparse
import collections


def calculate_dest_size(label_flags_size, num_edges, start):
    while (label_flags_size + num_edges * start >= 2 ** start):
        start += 1
    return start


def print_dest_sizes(max_label_flags_size, max_num_edges):
    table = collections.defaultdict(dict)
    table[1][1] = 2
    for i in range(1, max_label_flags_size):
        for j in range(1, max_num_edges):
            start_values = []
            if i == 1 and j == 1:
                continue
            if i > 1:
                start_values.append(table[i-1][j])
            if j > 1:
                start_values.append(table[i][j-1])
            start = min(start_values)
            table[i][j] = calculate_dest_size(i, j, start)
            print("{} {} {}".format(i, j, table[i][j]))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('max_label_flags_size', type=int,
                        help='max total size (bits) of label encodings, flags')
    parser.add_argument('max_num_edges', type=int,
                        help='maximum size in bits of all labels and edges')
    args = parser.parse_args()
    print_dest_sizes(args.max_label_flags_size, args.max_num_edges)

if __name__ == '__main__':
    main()
