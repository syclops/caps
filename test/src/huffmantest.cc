#include <cstdlib>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include "huffmandebug.h"

void usage()
{
  std::cerr << "Usage: ./huffmanbuild <infile>" << std::endl;
}

std::unordered_map<std::string, size_t> build_counts_map(std::istream& in_file)
{
  std::unordered_map<std::string, size_t> label_counts;
  std::string line, label;
  size_t count;
  while (std::getline(in_file, line)) {
    std::stringstream ss(line);
    ss >> label >> count;
    label_counts[label] = count;
  }
  return label_counts;
}

int main(int argc, char* argv[])
{
  std::unordered_map<std::string, size_t> label_counts;
  std::chrono::high_resolution_clock::time_point counts_start, counts_end;
  if (argc < 2) {
    std::cout << "Building label counts from standard input..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    label_counts = build_counts_map(std::cin);
    counts_end = std::chrono::high_resolution_clock::now();
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    std::cout << "Reading label counts from " << argv[1] << "..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    label_counts = build_counts_map(infile);
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

  std::cout << "Building Huffman tree..." << std::flush;
  counts_start = std::chrono::high_resolution_clock::now();
  HuffmanDebug huffman(label_counts);
  counts_end = std::chrono::high_resolution_clock::now();
  counts_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;

  std::cout << "Size with strings: " << huffman.size() << std::endl;
  std::cout << "Size with hashes: " << huffman.hash_size() << std::endl;
}
