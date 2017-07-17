"""
Given a json file from Censys's Alexa Top 1 Million TLS Handshakes data set,
extract a list of hostnames.
"""

import argparse
import fileinput
import json
import sys


def extract_hostnames(infile, logfile, outfile):
    total_names = 0
    num_error_names = 0
    num_invalid_certs = 0
    num_valid_names = 0
    with open(logfile, 'w') as lfile, open(outfile, 'w') as ofile:
        for line in fileinput.input(infile):
            total_names += 1
            parsed = json.loads(line.strip())
            if 'error' in parsed:
                num_error_names += 1
                lfile.write('{}: {}\n'.format(parsed['domain'], line.strip()))
                continue
            cert = parsed['data']['tls']['server_certificates']['certificate'][
                'parsed']
            if 'names' not in cert:
                num_invalid_certs += 1
                lfile.write('{}: {}\n'.format(parsed['domain'], cert))
                continue
            for name in cert['names']:
                num_valid_names += 1
                ofile.write('{}\n'.format(name))
    return [total_names, num_error_names, num_invalid_certs, num_valid_names]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile',
                        help='input file (json) or "-" for standard input')
    parser.add_argument('logfile', help='log file (txt)')
    parser.add_argument('outfile', help='output file (txt)')
    args = parser.parse_args()
    [total_names, num_error_names, num_invalid_certs,
     num_valid_names] = extract_hostnames(args.infile, args.logfile,
                                          args.outfile)
    print('{} out of {} names have valid certificates'.format(num_valid_names,
                                                              total_names))
    print('{} TLS connection errors'.format(num_error_names))
    print('{} certificates without DNS names'.format(num_invalid_certs))


if __name__ == '__main__':
    main()
