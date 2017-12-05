import argparse
import datetime
import fileinput
import matplotlib.pyplot as plt
import numpy as np


def to_int(date_string):
    '''
    Given a date string, return the number of days since January 1, 1 AD.
    :param date_string: a date string of the form `YYYYMMDD`.
    :return: a number of days since January 1, 1 AD.
    '''
    return datetime.date(int(date_string[:4]), int(date_string[4:6]),
                         int(date_string[6:])).toordinal()

def process_input(infile, date):
    if date:
        start_int = to_int(date)
    else:
        start_int = None
    series = np.empty((0, 2), int)
    for line in fileinput.input(infile):
        [date_string, line_count] = line.strip().split()
        date_int = to_int(date_string)
        if start_int is None:
            start_int = date_int
        series = np.append(series, np.array([[date_int - start_int,
                                              int(line_count)]]), axis=0)
    return series


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-d', '--date',
                        help='Start date of series (defaults to first date)')
    parser.add_argument('-i', '--infile', default ='-',
                        help='Input file (defaults to stdin)')
    parser.add_argument('-o', '--outfile',
                        help='Output file (defaults to stdout)')
    args = parser.parse_args()
    series = process_input(args.infile, args.date)
    plt.plot(series[:, 0], series[:, 1], '.')
    plt.show()


if __name__ == '__main__':
    main()