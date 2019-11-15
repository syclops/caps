"""
Validate and filter a set of already-processed domain names.
"""

import argparse
import fileinput
import tlds


def validate_name(name, tld_list):
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
    if tld_list is None:
        return name.split('.')[-1] in tlds.TLDS and '*' not in name[1:]
    else:
        pass  # TODO


def process_file(tld_list):
    for line in fileinput.input('-'):
        try:
            name = line.strip().encode('idna').decode('ascii').lower()
            if validate_name(name, tld_list):
                print(name)
        except UnicodeError:
            continue
        except:
            raise


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--tld-list', default=None, help='List of TLDs')
    args = parser.parse_args()
    process_file(args.tld_list)

if __name__ == '__main__':
    main()
