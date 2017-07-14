import argparse
import graph_tool


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', help='input file (.gv)')
    args = parser.parse_args()
    g = graph_tool.Graph()
    g.load(args.infile, 'dot')
    in_degrees = g.get_in_degrees(g.get_vertices())
    out_degrees = g.get_out_degrees(g.get_vertices())
    for in_deg, out_deg in zip(in_degrees, out_degrees):
        print("{},{}".format(in_deg, out_deg))


if __name__ == '__main__':
    main()
