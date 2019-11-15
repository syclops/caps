"""
Convert all occurrences of international domain names in the input file to
their Punycode encoding.
"""

import argparse
import fileinput
import sys


def get_logfile_obj(logfile):
    return sys.stderr if logfile is None else open(logfile, 'w')


def get_outfile_obj(outfile):
    return sys.stdout if outfile is None else open(outfile, 'w')


def read_file(infile, logfile, outfile):
    for line in fileinput.input(infile):
        try:
            print(line.lower().strip().encode('idna').decode('ascii'), file = outfile)
        except UnicodeError:
            print(line.lower().strip(), file = logfile)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-i', '--infile', default='-',
                        help='input file (defaults to stdin)')
    parser.add_argument('-l', '--logfile', default=None,
                        help='log file (defaults to stderr)')
    parser.add_argument('-o', '--outfile', default=None,
                        help='output file (defaults to stdout)')
    args = parser.parse_args()
    with get_logfile_obj(args.logfile) as logfile_obj, \
         get_outfile_obj(args.outfile) as outfile_obj:
        read_file(args.infile, logfile_obj, outfile_obj)


if __name__ == '__main__':
    main()
