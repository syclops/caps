"""
Wrapper class to enable useful features and statistics for tries and FSAs.
"""
import graph_tool
import graph_tool.search
import numpy
import sys


class SignalGraph(graph_tool.Graph):

    def __init__(self, infile=None, fmt='dot', outfile=None):
        if infile is not None:
            super().__init__(graph_tool.load_graph(infile, fmt))
            self.root = self.vertex(0)
        else:
            super().__init__()
            self.root = self.add_vertex()
            self.ep['label'] = self.new_edge_property('string')
        if outfile is None:
            self.outfile = sys.stdout
        else:
            self.outfile = open(outfile, 'w')

    def __del__(self):
        self.outfile.close()

    def degree_counts(self):
        vs = self.get_vertices()
        (io_degrees, counts) = numpy.unique(
            numpy.stack((self.get_in_degrees(vs), self.get_out_degrees(vs)),
                        axis=-1),
            return_counts=True, axis=0)
        with_counts = numpy.hstack((io_degrees,
                                    counts[:, None])).astype('int64')
        return with_counts[with_counts[:, 2].argsort()][::-1]

    def label_counts(self):
        chars, counts = numpy.unique(self.ep['label'].get_2d_array([0]),
                                     return_counts=True)
        with_counts = numpy.stack((chars, counts)).transpose()
        return with_counts[counts.argsort()][::-1]

    def compress_edges(self):
        """
        Compress sequences of single-character edges to form string-based
        edges.
        """
        vertex_filter = self.new_vertex_property('bool')
        edge_filter = self.new_edge_property('bool')
        ecv = EdgeCompressVisitor(self.ep.label, vertex_filter, edge_filter)
        graph_tool.search.dfs_search(self, self.root, ecv)
        for edge in ecv.add_set:
            (source, target, label) = edge
            new_edge = self.add_edge(source, target)
            self.ep.label[new_edge] = label
        self.set_vertex_filter(vertex_filter, inverted=True)
        self.set_edge_filter(edge_filter, inverted=True)


class EdgeCompressVisitor(graph_tool.search.DFSVisitor):

    def __init__(self, label, vertex_filter, edge_filter):
        self.label = label
        self.add_set = set()
        self.vertex_filter = vertex_filter
        self.edge_filter = edge_filter

    def get_long_edge(self, e):
        current_label = self.label[e]
        current_edge = e
        while (current_edge.target().in_degree() == 1
               and current_edge.target().out_degree() == 1):
            self.edge_filter[current_edge] = 1
            for edge in current_edge.target().out_edges():
                current_edge = edge
                current_label += self.label[current_edge]
        if len(current_label) > 1:
            self.edge_filter[current_edge] = 1
        return current_label, current_edge.target()

    def discover_vertex(self, u):
        if u.in_degree() == 1 and u.out_degree() == 1:
            self.vertex_filter[u] = 1
        else:
            for edge in u.out_edges():
                label, target = self.get_long_edge(edge)
                if len(label) > 1:
                    self.add_set.add((u, target, label))
