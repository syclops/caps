"""
Randomly sample lines from an input file.
"""
import argparse
import fileinput
import random
import signal


def sample_lines(infile, fraction, seed=None):
    """
    Print a random sample of lines from a file.

    Given an input file, a fraction (between 0 and 1), and an optional seed for
    the random number generator, print a random sample of the lines in the
    input file. This function does not guarantee that exactly `fraction` of the
    lines in the input file `infile` will be printed because it uses a random
    number generator to determine whether to print each line independently.

    Arguments:
        `infile` (str): the path to the input file.
        `fraction` (float): the probability with which to print each line.

    Returns:
        None.

    Raises:
        ValueError: the given fraction was not between 0 and 1.
    """
    if fraction < 0.0 or fraction > 1.0:
        raise ValueError('Sampling fraction must be between 0 and 1')
    random.seed(seed)
    signal.signal(signal.SIGPIPE, signal.SIG_DFL)
    for line in fileinput.input(infile):
        if random.random() < fraction:
            print(line.strip())


def main():
    """
    Parse arguments and call the sampling function.
    """
    parser = argparse.ArgumentParser()
    parser.add_argument('infile', default='-', help='input file')
    parser.add_argument('fraction', default=0.5, type=float,
                        help='fraction of lines to select')
    parser.add_argument('--seed', default=None, type=int,
                        help='seed for random number generator')
    args = parser.parse_args()
    sample_lines(args.infile, args.fraction, args.seed)


if __name__ == '__main__':
    main()
