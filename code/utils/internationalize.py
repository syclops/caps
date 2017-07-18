"""
Convert all occurrences of international domain names in the input file to
their Punycode encoding.
"""

import fileinput


for line in fileinput.input():
    print(line.strip().encode('idna').decode('ascii'))
