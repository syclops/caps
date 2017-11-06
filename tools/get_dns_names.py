"""
Given a json file from Censys's Alexa Top 1 Million TLS data data set,
extract a list of hostnames.
"""

import argparse
import fileinput
import json
import multiprocessing as mp
import sortedcontainers as sc
import sys

import tlds


class LockSet(object):

    def __init__(self):
        self.set = sc.SortedSet()
        self.lock = mp.Lock()

    def add(self, item):
        with self.lock:
            self.set.add(item)


class LockFile(object):

    def __init__(self, file_):
        self.file = file_
        self.lock = mp.Lock()

    def write(self, string):
        with self.lock:
            self.file.write(string)

    def close(self):
        self.file.close()


def read_data(data_queue, infile, jobs):
    for line in fileinput.input(infile):
        data_queue.put(line.strip())
    for _ in range(jobs):
        data_queue.put('DONE')


def parse_data(data_queue, names, lfile, ip):
    while True:
        data = data_queue.get()
        if data == 'DONE':
            break
        json_dict = json.loads(data)
        try:
            if 'error' in json_dict:
                log_error(lfile, json_dict[ip], json_dict['error'])
            else:
                cert = json_dict['data']['tls']['server_certificates'][
                    'certificate']['parsed']
                try:
                    if 'names' in cert:
                        for name in cert['names']:
                            validate_and_add_name(
                                names, name.encode('idna').decode('utf-8'))
                    else:
                        for name in cert['subject']['common_name']:
                            validate_and_add_name(
                                names, name.encode('idna').decode('utf-8'))
                        for name in cert['extension']['subject_alt_name'][
                            'dns_names']:
                            validate_and_add_name(
                                names, name.encode('idna').decode('utf-8'))
                except:
                    pass
        except KeyError as e:
            log_error(lfile, json_dict[ip], e.args[0])
        except:
            raise


def log_error(lfile, line, error):
    lfile.write('{}: {}\n'.format(line, error))


def validate_and_add_name(name_set, name):
    if '.' in name and name.split('.')[-1] in tlds.TLDS:
        name_set.add('.'.join(name.split('.')[::-1]))


def process_file(infile, logfile, outfile, ip, jobs, max_queue_size):
    queue = mp.Queue(max_queue_size)
    names = LockSet()
    lfile = LockFile(open(logfile, 'wt'))
    read_process = mp.Process(target=read_data, args=(queue, infile, jobs))
    parse_processes = [mp.Process(target=parse_data, args=(queue, names, lfile,
                                                           ip))
                       for _ in range(jobs)]
    read_process.start()
    for parser in parse_processes:
        parser.start()
    read_process.join()
    for parser in parse_processes:
        parser.join()
    lfile.close()
    # write_process.join()
    with open(outfile, 'wt') as ofile:
        for name in names.set:
            ofile.write(name + '\n')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile',
                        help='input file (json) or "-" for standard input')
    parser.add_argument('logfile', help='log file (txt)')
    parser.add_argument('outfile', help='output file (txt)')
    parser.add_argument('--ip', action='store_true',
                        help='log IP addresses instead of DNS names')
    parser.add_argument('-j', '--jobs', type=int, default=1,
                        help='number of jobs')
    parser.add_argument('-q', '--max-queue-size', type=int, default=20000,
                        help='maximum queue size for I/O')
    args = parser.parse_args()
    ip = 'ip' if args.ip else 'domain'
    process_file(args.infile, args.logfile, args.outfile, ip, args.jobs,
                 args.max_queue_size)
    # with DNSInfoParser(args.logfile, args.outfile, args.ip,
                       # args.reverse) as dns_parser:
        # dns_parser.add_json(args.infile)
        # print('Out of {} domains...'.format(dns_parser.total_names))
        # print('  {} have certificates'.format(dns_parser.certificates))
        # print('  {} have certificates with valid DNS names'.format(
            # dns_parser.certificates))
        # print('  {} unique, valid DNS names are present'.format(
            # len(dns_parser.names)))
        # print('  {} had TLS connection errors'.format(dns_parser.error_names))
        # print('  {} have certificates without DNS names'.format(
            # dns_parser.nameless_certs))
        # print('Sorting and printing unique names...')
        # dns_parser.output_names()


if __name__ == '__main__':
    main()
