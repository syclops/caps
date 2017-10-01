"""
Compute interesting stats about a graph.
"""
import argparse
import collections
import fileinput
import graph_tool
import numpy
import scipy
import scipy.stats
import sys

import graph_wrapper


def get_outfile_obj(infile, outfile):
    if outfile is None:
        return sys.stdout
    return open(outfile, 'w')


def process(infile, outfile_obj):
    graph = graph_wrapper.SignalGraph(infile)
    uncompressed_vertices = graph.num_vertices()
    uncompressed_edges = graph.num_edges()
    uncompressed_degree_counts = graph.degree_counts()
    uncompressed_label_counts = graph.label_counts()
    uncompressed_degree_entropy = scipy.stats.entropy(
        uncompressed_degree_counts[:, 2], base=2)
    uncompressed_label_entropy = scipy.stats.entropy(
        uncompressed_label_counts[:, 1].astype('int64'), base=2)
    graph.compress_edges()
    compressed_vertices = graph.num_vertices()
    compressed_edges = graph.num_edges()
    compressed_degree_counts = graph.degree_counts()
    compressed_label_counts = graph.label_counts()
    compressed_degree_entropy = scipy.stats.entropy(
        compressed_degree_counts[:, 2], base=2)
    compressed_label_entropy = scipy.stats.entropy(
        compressed_label_counts[:, 1].astype('int64'), base=2)
    print('States: {} ({} compressed, ratio {:.2f})'.format(
        uncompressed_vertices, compressed_vertices,
        compressed_vertices / uncompressed_vertices), file=outfile_obj)
    print('Edges: {} ({} compressed, ratio {:.2f})'.format(
        uncompressed_edges, compressed_edges,
        compressed_edges / uncompressed_edges), file=outfile_obj)
    print('Degree entropy: {0:.2f} bits '
          '({1:.2f} compressed, ratio {2:.2f})'.format(
              uncompressed_degree_entropy, compressed_degree_entropy,
              compressed_degree_entropy / uncompressed_degree_entropy),
          file=outfile_obj)
    print('Label entropy: {0:.2f} bits '
          '({1:.2f} compressed, ratio {2:.2f})'.format(
              uncompressed_label_entropy, compressed_label_entropy,
              compressed_label_entropy / uncompressed_label_entropy),
          file=outfile_obj)
    print('Degree histogram', file=outfile_obj)
    for io_count in uncompressed_degree_counts:
        print('{} in, {} out: {}'.format(io_count[0], io_count[1],
                                         io_count[2]), file=outfile_obj)
    print('Uncompressed label histogram', file=outfile_obj)
    for label_count in uncompressed_label_counts:
        print('{}: {}'.format(label_count[0], label_count[1]),
              file=outfile_obj)
    print('Compressed label histogram', file=outfile_obj)
    for label_count in compressed_label_counts:
        print('{}: {}'.format(label_count[0], label_count[1]),
              file=outfile_obj)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--infile', default='-',
                        help='input graph (.gv) file (defaults to stdin)')
    parser.add_argument('-o', '--outfile', default=None,
                        help='output file (defaults to stdout if reading from'
                             'stdin, or to <infile>.report.txt otherwise)')
    args = parser.parse_args()
    with get_outfile_obj(args.infile, args.outfile) as outfile_obj:
        process(args.infile, outfile_obj)


if __name__ == '__main__':
    main()

