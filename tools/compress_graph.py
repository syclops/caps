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


class GraphStats(object):

    def __init__(self, infile=None, fmt='dot'):
        self.infile = infile
        self.graph = None
        if self.infile is not None:
            self.load_file(infile, fmt)

    def load_file(self, infile, fmt='dot'):
        self.graph = graph_tool.load_graph(infile, fmt)

    def degree_counts(self):
        vs = self.graph.get_vertices()
        (io_degrees, counts) = numpy.unique(
            numpy.stack((self.graph.get_in_degrees(vs),
                         self.graph.get_out_degrees(vs)), axis=-1),
            return_counts=True, axis=0)
        with_counts = numpy.hstack((io_degrees,
                                    counts[:, None])).astype('int64')
        return with_counts[with_counts[:, 2].argsort()][::-1]

    def label_counts(self):
        chars, counts = numpy.unique(self.graph.ep['label'].get_2d_array([0]),
                                     return_counts=True)
        with_counts = numpy.stack((chars, counts)).transpose()
        return with_counts[counts.argsort()][::-1]


def get_outfile_obj(infile, outfile):
    if outfile is None:
        return sys.stdout
    return open(outfile, 'w')


def process(infile, outfile_obj):
    stats = GraphStats(infile)
    print('States: {}'.format(stats.graph.num_vertices()), file=outfile_obj)
    print('Edges: {}'.format(stats.graph.num_edges()), file=outfile_obj)
    degree_counts = stats.degree_counts()
    label_counts = stats.label_counts()
    print('In/out-degree entropy: {} bits'.format(
        scipy.stats.entropy(degree_counts[:, 2], base=2)), file=outfile_obj)
    print('Label entropy: {} bits'.format(
        scipy.stats.entropy(label_counts[:, 1].astype('int64'), base=2)),
        file=outfile_obj)
    for io_count in degree_counts:
        print('{} in, {} out: {}'.format(io_count[0], io_count[1],
                                         io_count[2]), file=outfile_obj)
    for label_count in label_counts:
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

