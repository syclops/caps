"""
Parser for JSON-formatted TLS handshakes.
"""

import fileinput
import json
import sys


class DNSInfoParser:
    """
    Class that parses DNS information from JSON TLS handshake data.
    """

    def __init__(self, lfilename, ofilename='', ip=False, reverse=True):
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

    def __enter__(self):
        self.lfile = open(self.lfilename, 'w')
        if self.ofilename != '':
            self.ofile = open(self.ofilename, 'w')
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

    def add_name(self, name):
        self.added_names += 1
        if self.reverse:
            self.names.add('.'.join(name.split('.')[::-1]))
        else:
            self.names.add(name)

    def log_error(self, line, error):
        self.lfile.write('{}: {}\n'.format(line[self.ip], error))

    def add_json(self, ifilename):
        with open(ifilename, 'r') as ifile:
            for line in fileinput.input(ifile):
                self.total_names += 1
                json_dict = json.loads(line.strip())
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
                    self.add_name(name)
            else:
                self.add_name(cert['subject']['common_name'])
                if ('extensions' in cert
                    and 'subject_alt_name' in cert['extensions']):
                    for name in cert['extensions']['subject_alt_name'][
                            'dns_names']:
                        self.add_name(name)
        except KeyError:
            self.nameless_certs += 1
            raise

