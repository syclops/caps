import math
def testing():
    print("hello this is a test")

def calculate_entropy(hist, occ_cuttoff = 0):
    #calculates the entropy in a given histogram of symbols and frequencies
    #input: histogram formatted {label: occurrences}
    #optional: an occurence cutoff, where if a label has less than that number of occurences it is not included in the calculation
    #output: entropy
    entropy = 0
    frequencies = []
    for f in hist.values():
        if f < occ_cuttoff:
            continue
        frequencies.append(f)
    total_counts = sum(frequencies)
    for i in range(len(frequencies)):
        frequencies[i] /= total_counts
    for f in frequencies:
        entropy -= f * math.log(f, 2)
    return entropy

def calculate_codebook_size(hist):
    #calculates the codebook size in a given histogram of symbols and frequencies
    #input: histogram formatted {label: occurrences}
    #output: codebook size in bits
    labels = hist.keys()
    label_bits = 0
    for l in labels:
        label_bits += 7 * (len(l) + 1)
    int_size = math.ceil(math.log(len(labels), 2))
    int_size = math.ceil(math.log(int_size + 1, 2))
    bit_length_storage =  int_size * math.ceil(math.log(len(labels), 2))
    return bit_length_storage + label_bits

def total_storage(hist, edges, occ_cuttoff = 0):
    bits_per_edge = 3 #based on what steve said, might be worth changing later?
    total_bits_below_cutoff = 0
    for symbol, occ in hist.items():
        if occ < occ_cuttoff:
            total_bits_below_cutoff += 7 * (len(symbol) + 1)
    codebook_size = calculate_codebook_size(hist)
    entropy = calculate_entropy(hist, occ_cuttoff)
    bits_above_cutoff = entropy * edges * bits_per_edge
    print(total_bits_below_cutoff)
    print(bits_above_cutoff)
    return total_bits_below_cutoff + bits_above_cutoff + codebook_size
