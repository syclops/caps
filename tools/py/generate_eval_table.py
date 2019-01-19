import argparse
import enum
import fileinput
import math
import os
import subprocess
import sys


BZIP = 'bzip2'
ZPAQ = 'zpaq'
LARGE_FP_RATE = 0.0001
MED_FP_RATE = 0.001
SMALL_FP_RATE = 0.01
BZIP_OUT_FILE = '{input}.{ext}'
ZPAQ_OUT_FILE = '{input}.{param}.{ext}'
BZIP_PARAMS = BZIP + ' -k -{param} {input}'
ZPAQ_PARAMS = ZPAQ + ' add ' + ZPAQ_OUT_FILE + ' files {input} -m{param}'
BZIP_EXT = 'bz2'
ZPAQ_EXT = 'zpaq'


class VarNames(enum.Enum):
    NUM_NAMES = '\\numnames'
    PLAINTEXT_SIZE = '\\plaintextsize'
    BLOOM_LARGE_SIZE = '\\bloomlargesize'
    BLOOM_MED_SIZE = '\\bloommedsize'
    BLOOM_SMALL_SIZE = '\\bloomsmallsize'
    BZ_LARGE_SIZE = '\\bzlargesize'
    BZ_MED_SIZE = '\\bzmedsize'
    BZ_SMALL_SIZE = '\\bzsmallsize'
    ZPAQ_LARGE_SIZE = '\\zpaqlargesize'
    ZPAQ_MED_SIZE = '\\zpaqmedsize'
    ZPAQ_SMALL_SIZE = '\\zpaqsmallsize'
    FSA_PLAIN_SIZE = '\\fsaplainsize'
    FSA_PATH_SIZE = '\\fsapathsize'
    FSA_TRANS_SIZE = '\\fsatranssize'
    FSA_BOTH_SIZE = '\\fsabothsize'


def print_var_value(var, value, fd):
    print('\\newcommand{{{0}}}{{{1:,}}}'.format(var, value), file=fd)


def optimal_bloom_size(num_names, fp_rate):
    return -num_names * math.log(fp_rate) / (math.log(2) ** 2)


def process_input_file(input_file, fd):
    line_count = 0
    byte_count = 0
    for line in fileinput.input(input_file):
        line_count += 1
        byte_count += len(line)
    print_var_value(VarNames.NUM_NAMES.value, line_count, fd)

    byte_count = round(byte_count / 1e6)
    print_var_value(VarNames.PLAINTEXT_SIZE.value, byte_count, fd)

    bloom_large_size = round(optimal_bloom_size(line_count,
                                                LARGE_FP_RATE) / 8e6)
    print_var_value(VarNames.BLOOM_LARGE_SIZE.value, bloom_large_size, fd)

    bloom_med_size = round(optimal_bloom_size(line_count, MED_FP_RATE) / 8e6)
    print_var_value(VarNames.BLOOM_MED_SIZE.value, bloom_med_size, fd)

    bloom_small_size = round(optimal_bloom_size(line_count,
                                                SMALL_FP_RATE) / 8e6)
    print_var_value(VarNames.BLOOM_SMALL_SIZE.value, bloom_small_size, fd)


def run_util_test(command_format, output_format, input, param, ext):
    command_kwargs = {
        'input': input,
        'param': param,
        'ext': ext
    }
    subprocess.run(command_format.format(**command_kwargs).split())
    output_file = output_format.format(**command_kwargs)
    size = round(os.path.getsize(output_file) / 8e6)
    os.remove(output_file)
    return size


def run_test(command_format, output_format, input, param, ext, var_name, fd):
    size = run_util_test(command_format, output_format, input, param, ext)
    print_var_value(var_name, size, fd)


def test_bzip(input_file, fd):
    run_test(BZIP_PARAMS, BZIP_OUT_FILE, input_file, 1, BZIP_EXT,
             VarNames.BZ_LARGE_SIZE.value, fd)
    run_test(BZIP_PARAMS, BZIP_OUT_FILE, input_file, 5, BZIP_EXT,
             VarNames.BZ_MED_SIZE.value, fd)
    run_test(BZIP_PARAMS, BZIP_OUT_FILE, input_file, 9, BZIP_EXT,
             VarNames.BZ_SMALL_SIZE.value, fd)

def test_zpaq(input_file, fd):
    run_test(ZPAQ_PARAMS, ZPAQ_OUT_FILE, input_file, 1, ZPAQ_EXT,
             VarNames.ZPAQ_LARGE_SIZE.value, fd)
    run_test(ZPAQ_PARAMS, ZPAQ_OUT_FILE, input_file, 5, ZPAQ_EXT,
             VarNames.ZPAQ_MED_SIZE.value, fd)
    run_test(ZPAQ_PARAMS, ZPAQ_OUT_FILE, input_file, 511, ZPAQ_EXT,
             VarNames.ZPAQ_SMALL_SIZE.value, fd)


def parse(args=sys.argv):
    parser = argparse.ArgumentParser(args)
    parser.add_argument('-i', '--input-file', default='-',
                        help='input file name (defaults to stdin)')
    parser.add_argument('-o', '--output-file',
                        help='input file name (defaults to stdout)')
    return parser.parse_args()


def process(args):
    with open(args.output_file, 'w') if args.output_file is not None \
            else sys.stdout as fd:
        process_input_file(args.input_file, fd)
        test_bzip(args.input_file, fd)
        test_zpaq(args.input_file, fd)
    # Print DAFSA numbers


def main():
    args = parse()
    process(args)


if __name__ == '__main__':
    main()