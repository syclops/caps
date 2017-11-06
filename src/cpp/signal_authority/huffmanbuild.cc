#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include "../encoding/huffman.h"

void usage()
{
  std::cerr << "Usage: ./huffmanbuild <infile>" << std::endl;
}

void build_counts_map(std::istream& in_file,
                      std::map<std::string, size_t>& counts_map)
{
  std::string line, label;
  size_t count;
  while (std::getline(in_file, line)) {
    std::stringstream ss(line);
    ss >> label >> count;
    counts_map[label] = count;
  }
}

int main(int argc, char* argv[])
{
  std::map<std::string, size_t> counts_map;
  std::chrono::high_resolution_clock::time_point counts_start, counts_end;
  if (argc < 2) {
    std::cout << "Building label counts from standard input..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    build_counts_map(std::cin, counts_map);
    counts_end = std::chrono::high_resolution_clock::now();
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    std::cout << "Reading label counts from " << argv[1] << "..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    build_counts_map(infile, counts_map);
    counts_end = std::chrono::high_resolution_clock::now();
    infile.close();
  } else {
    usage();
    return 0;
  }
  std::chrono::duration<double> counts_duration
    = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;

  std::chrono::high_resolution_clock::time_point huffman_start, huffman_end;
  std::cout << "Building Huffman tree..." << std::flush;
  huffman_start = std::chrono::high_resolution_clock::now();
  HuffmanTree huffman_tree(counts_map);
  huffman_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> huffman_duration
    = std::chrono::duration_cast<std::chrono::duration<double>>(
      huffman_end - huffman_start);
  std::cout << "done! (took " << huffman_duration.count() << " seconds)"
            << std::endl;

  std::map<std::string, size_t> huffman_map;
  std::chrono::high_resolution_clock::time_point dump_start, dump_end;
  std::cout << "Dumping Huffman codebook..." << std::flush;
  dump_start = std::chrono::high_resolution_clock::now();
  huffman_tree.get_encoding_lengths(huffman_map);
  dump_end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> dump_duration
    = std::chrono::duration_cast<std::chrono::duration<double>>(
      dump_end - dump_start);
  std::cout << "done! (took " << dump_duration.count() << " seconds)"
            << std::endl;

  size_t total_size = 0;
  for (auto it: huffman_map) {
    total_size += it.second * counts_map[it.first];
  }
  std::cout << "Total size: " << total_size << std::endl;
  return 0;
}
