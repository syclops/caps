"""
Validate and filter a set of already-processed domain names.
"""

import argparse
import fileinput
import tlds


def validate_name(name, reverse):
    """
    Ensure that a name is a properly formatted DNS name.

    Specifically, for a name to be considered properly formatted, it must meet
    the following criteria:
    * It has more than one label, i.e., a `.` character appears in the name.
    * Its top-level label is in ICANN's list of Internet TLDs.
    * If a wildcard (`*`) symbol appears, it must appear in the
    :param name:
    :return:
    """
    if '.' not in name or '/' in name or '@' in name:
        return False
    if reverse:
        return name.split('.')[0] in tlds.TLDS and '*' not in name[:-1]
    else:
        return name.split('.')[-1] in tlds.TLDS and '*' not in name[1:]


def process_file(infile, reverse):
    for line in fileinput.input(infile):
        try:
            name = line.strip().encode('idna').decode('utf-8').lower()
            if validate_name(name, reverse):
                if reverse:
                    print(name)
                else:
                    print('.'.join(name.split('.')[::-1]))
        except UnicodeError:
            continue
        except:
            raise


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', type=str, default='-',
                        help='Input file name (defaults to standard input)')
    parser.add_argument('--reverse', action='store_true', help='Reverse ')
    args = parser.parse_args()
    process_file(args.infile, args.reverse)

if __name__ == '__main__':
    main()
