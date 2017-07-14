"""
Hostname library.
"""

import re
import string

WILDCARD = '*'
DELIMITER = '.'
MAX_LABEL_LEN = 63
MAX_NAME_LEN = 253
ALLOWED_CHARS = (string.ascii_lowercase + string.digits + WILDCARD + DELIMITER
                 + '-')


def valid_label(label, strict=False):
    if strict:
        regex = re.compile('^(?![0-9]+$)(?!-)[a-zA-Z0-9-]{,63}(?<!-)$')
    else:
        regex = re.compile('[a-zA-Z0-9_-]{1,63}')
    return regex.fullmatch(label) is not None


def valid(name):
    if len(name) > MAX_NAME_LEN:
        return False
    for i, label in enumerate(name.split(DELIMITER)):
        if not valid_label(label) and (i != 0 or label != WILDCARD):
            return False
    return True


def parse_name(name, reverse=False):
    if not valid(name):
        raise ValueError('Argument {} is not a valid DNS name'.format(name))
    if reverse:
        return name.split(DELIMITER)[::-1]
    return name.split(DELIMITER)


def reverse(name):
    return DELIMITER.join(parse_name(name, reverse=True))
