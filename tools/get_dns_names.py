"""
Given a json file from Censys's Alexa Top 1 Million TLS Handshakes data set,
extract a list of hostnames.
"""

import argparse
import fileinput
import json
import sys


class DNSInfoParser:
    """
    Class that parses DNS information from JSON TLS handshake data.
    """

    def __init__(self, lfilename, ofilename='', ip=False, reverse=True,
                 tlds='tlds-alpha-by-domain.txt'):
        self.lfilename = lfilename
        self.ofilename = ofilename
        self.names = set()
        self.ip = 'ip' if ip else 'domain'
        self.reverse = reverse
        self.total_names = 0
        self.certificates = 0
        self.error_names = 0
        self.added_names = 0
        self.nameless_certs = 0
        self.tlds = set()
        for line in fileinput.input(tlds):
            if line.strip()[0] != '#':
                self.tlds.add(line.strip().lower())

    def __enter__(self):
        self.lfile = open(self.lfilename, 'wt')
        if self.ofilename != '':
            self.ofile = open(self.ofilename, 'wt')
        else:
            self.ofile = sys.stdout
        return self

    def __exit__(self, exception_type, exception_value, traceback):
        self.lfile.close()
        self.lfile = None
        self.ofile.close()
        self.ofile = None

    def output_names(self):
        for name in sorted(self.names):
            self.ofile.write('{}\n'.format(name))

    def validate_name(self, name):
        """
        Ensure that a name conforms to
        :param name:
        :return:
        """
        return ('.' in name and name.split('.')[-1] in self.tlds
                and '*' not in name[1:] and '/' not in name
                and '@' not in name)

    def add_name(self, name):
        if not self.validate_name(name):
            return
        self.added_names += 1
        if self.reverse:
            self.names.add('.'.join(name.split('.')[::-1]))
        else:
            self.names.add(name)

    def log_error(self, line, error):
        self.lfile.write('{}: {}\n'.format(line[self.ip], error))

    def add_json(self, ifilename):
        for line in fileinput.input(ifilename):
            self.total_names += 1
            try:
                json_dict = json.loads(line.strip())
            except:
                print(line.strip())
                raise
            try:
                self.parse_json_dict(json_dict)
            except KeyError as e:
                self.log_error(json_dict, e.args[0])
            except:
                raise

    def parse_json_dict(self, json_dict):
        if 'error' in json_dict:
            self.log_error(json_dict, json_dict['error'])
            self.error_names += 1
        else:
            self.parse_cert(json_dict['data']['tls']['server_certificates'][
                'certificate']['parsed'])
            self.certificates += 1

    def parse_cert(self, cert):
        try:
            if 'names' in cert:
                for name in cert['names']:
                    self.add_name(name.encode('idna').decode('utf-8').lower())
            else:
                for name in cert['subject']['common_name']:
                    self.add_name(name.encode('idna').decode('utf-8').lower())
                if ('extensions' in cert
                    and 'subject_alt_name' in cert['extensions']):
                    for name in cert['extensions']['subject_alt_name'][
                            'dns_names']:
                        self.add_name(
                            name.encode('idna').decode('utf-8').lower())
        except UnicodeError:
            self.nameless_certs += 1
        except KeyError:
            self.nameless_certs += 1
            raise


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile',
                        help='input file (json) or "-" for standard input')
    parser.add_argument('logfile', help='log file (txt)')
    parser.add_argument('outfile', help='output file (txt)')
    parser.add_argument('--ip', action='store_true',
                        help='log IP addresses instead of DNS names')
    parser.add_argument('--reverse', action='store_true',
                        help='store DNS names in reverse label order')
    args = parser.parse_args()
    with DNSInfoParser(args.logfile, args.outfile, args.ip,
                       args.reverse) as dns_parser:
        dns_parser.add_json(args.infile)
        print('Out of {} domains...'.format(dns_parser.total_names))
        print('  {} have certificates'.format(dns_parser.certificates))
        print('  {} have certificates with valid DNS names'.format(
            dns_parser.certificates))
        print('  {} unique, valid DNS names are present'.format(
            len(dns_parser.names)))
        print('  {} had TLS connection errors'.format(dns_parser.error_names))
        print('  {} have certificates without DNS names'.format(
            dns_parser.nameless_certs))
        print('Sorting and printing unique names...')
        dns_parser.output_names()


if __name__ == '__main__':
    main()
