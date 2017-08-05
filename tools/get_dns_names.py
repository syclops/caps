"""
Given a json file from Censys's Alexa Top 1 Million TLS Handshakes data set,
extract a list of hostnames.
"""

import argparse
import fileinput
import json
import sys


def extract_hostnames(infile, logfile, outfile, ip):
    total_names = 0
    num_error_names = 0
    num_invalid_certs = 0
    num_valid_names = 0
    num_cert_domains = 0
    num_valid_domains = 0
    with open(logfile, 'w') as lfile, open(outfile, 'w') as ofile:
        for line in fileinput.input(infile):
            total_names += 1
            parsed = json.loads(line.strip())
            if 'error' in parsed:
                num_error_names += 1
                if ip:
                    lfile.write('{}: {}\n'.format(parsed['ip'],
                                                  line.strip()))
                else:
                    lfile.write('{}: {}\n'.format(parsed['domain'],
                                                  line.strip()))
                continue
            num_cert_domains += 1
            cert = parsed['data']['tls']['server_certificates']['certificate'][
                'parsed']
            if 'names' not in cert:
                num_invalid_certs += 1
                if ip:
                    lfile.write('{}: {}\n'.format(parsed['ip'], cert))
                else:
                    lfile.write('{}: {}\n'.format(parsed['domain'], cert))
                continue
            num_valid_domains += 1
            for name in cert['names']:
                num_valid_names += 1
                ofile.write('{}\n'.format(name))
    return [total_names, num_error_names, num_invalid_certs, num_valid_names,
            num_cert_domains, num_valid_domains]


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile',
                        help='input file (json) or "-" for standard input')
    parser.add_argument('logfile', help='log file (txt)')
    parser.add_argument('outfile', help='output file (txt)')
    parser.add_argument('--ip', action='store_true',
                        help='log IP addresses instead of DNS names')
    args = parser.parse_args()
    [total_names, num_error_names, num_invalid_certs,
     num_valid_names, num_cert_domains, num_valid_domains] = extract_hostnames(
         args.infile, args.logfile, args.outfile, args.ip)
    print('Out of {} domains...'.format(total_names))
    print('  {} have certificates'.format(num_cert_domains))
    print('  {} have certificates with valid DNS names'.format(
        num_valid_domains))
    print('  {} valid DNS names are present'.format(num_valid_names))
    print('  {} had TLS connection errors'.format(num_error_names))
    print('  {} have certificates without DNS names'.format(num_invalid_certs))


if __name__ == '__main__':
    main()
