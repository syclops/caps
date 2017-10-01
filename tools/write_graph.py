import argparse
import graph_tool
import graph_tool.draw


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', help='input file')
    parser.add_argument('outfile', help='output file')
    args = parser.parse_args()
    g = graph_tool.Graph()
    g.load(args.infile, 'dot')
    graph_tool.draw.graph_draw(g, output_size=(600, 600), output=args.outfile)


if __name__ == '__main__':
    main()
