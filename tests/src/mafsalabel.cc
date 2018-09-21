/**
 * Author: smaptas
 */


// Include header file.

// Include C standard libraries.
#include <cstdlib>
#include <ctime>

// Include C++ standard libraries.
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

// Include other headers from this project.
#include "mafsadebug.h"

// Include header from other projects.

void usage(const char* scriptname)
{
  std::cerr << "Usage: ./" << scriptname << " <infile>"
            << std::endl;
}

int main(int argc, char* argv[])
{
  // Build the FSA from standard input or from a file.
  std::string infile_name;
  std::chrono::high_resolution_clock::time_point counts_start, counts_end;
  DAFSADebug mafsa;
  if (argc == 1) {
    std::cout << "Loading FSA from standard input..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    mafsa.load_txt(std::cin);
    counts_end = std::chrono::high_resolution_clock::now();
    infile_name = "stdin";
  } else if (argc == 2) {
    std::ifstream infile(argv[1]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    std::cout << "Loading FSA from " << argv[1] << "..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    mafsa.load_txt(infile);
    counts_end = std::chrono::high_resolution_clock::now();
    infile.close();
    infile_name = argv[1];
  } else {
    usage(argv[0]);
    return 0;
  }
  std::chrono::duration<double> counts_duration
    = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;

  std::ostringstream file_name_stream;
  file_name_stream << time(nullptr) << ".labels.txt";
  std::ofstream graph_file(file_name_stream.str());
  std::cout << "Writing graph to " << file_name_stream.str() << "..." << std::flush;
  counts_start = std::chrono::high_resolution_clock::now();
  for (auto it: mafsa.get_label_counts()) {
    graph_file << it.first << " " << it.second << std::endl;
  }
  graph_file.close();
  counts_end = std::chrono::high_resolution_clock::now();
  counts_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
    counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
  return 0;
}