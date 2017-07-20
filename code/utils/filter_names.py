"""
Filtering utilities for DNS names that match or do not match a particular
pattern.
"""
import argparse
import collections
import fileinput


def recursive_defaultdict():
    """
    Generate a recursive default dictionary.
    """
    return collections.defaultdict(recursive_defaultdict)


class DNSSuffixMatcher(object):
    """
    Matcher for allowed DNS suffixes.
    """

    def __init__(self, suffix_file=None):
        self.num_suffixes = 0
        self.suffix_file = suffix_file
        self.suffix_dict = recursive_defaultdict()
        if self.suffix_file is not None:
            self.load_file(suffix_file)

    def __repr__(self):
        return '{}: {}'.format(self.suffix_file, self.suffix_dict)

    def load_file(self, infile):
        """
        Load a set of allowed DNS suffixes from a file.
        """
        for line in fileinput.input(infile):
            if len(line.strip()) and line[:2] != '//':
                self.add_suffix(line.strip())

    def add_suffix(self, suffix):
        """
        Add a given allowed or disallowed suffix to the matcher.
        """
        current_dict = self.suffix_dict
        tokens = suffix.lower().encode('idna').decode('ascii').split('.')[::-1]
        for label in tokens:
            if label[0] == '!':
                label = label[1:]
                current_dict[label] = '!'
            current_dict = current_dict[label]
        self.num_suffixes += 1

    def match_name(self, name):
        """
        Match a DNS name to its suffix.
        """
        current_dict = self.suffix_dict
        reverse_suffix_list = []
        tokens = name.lower().encode('idna').decode('ascii').split('.')[::-1]
        for i, label in enumerate(tokens):
            if not label:
                reverse_suffix_list = []
                break
            elif label in current_dict:
                if current_dict[label] == '!':
                    break
                else:
                    reverse_suffix_list.append(label)
                    current_dict = current_dict[label]
            elif '*' in current_dict:
                if i == len(tokens) - 1:
                    reverse_suffix_list.append(label)
                else:
                    reverse_suffix_list = []
                break
            else:
                break
        return '.'.join(reverse_suffix_list[::-1])


def filter_names(suffix_matcher, infile):
    for line in fileinput.input(infile):
        if suffix_matcher.match_name(line.strip()):
            print(line.strip().lower())


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('suffixes', help='list of valid suffixes')
    parser.add_argument('infile', nargs='?', default='-',
                        help='input file (defaults to stdin)')
    args = parser.parse_args()
    suffix_matcher = DNSSuffixMatcher(args.suffixes)
    filter_names(suffix_matcher, args.infile)


if __name__ == '__main__':
    main()
