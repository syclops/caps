#include <ctime>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

#include "mafsadebug.h"

void usage(char* scriptname)
{
  std::cerr << "Usage: ./" << scriptname << " [node_compaction_level] <infile>"
            << std::endl;
}

int main(int argc, char* argv[])
{
  // Build the FSA from standard input or from a file.
  std::string infile_name;
  std::chrono::high_resolution_clock::time_point counts_start, counts_end;
  DAFSADebug mafsa;
  if (argc == 2) {
    std::cout << "Building FSA from standard input..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    mafsa.load_txt(std::cin);
    counts_end = std::chrono::high_resolution_clock::now();
    infile_name = "stdin";
  } else if (argc == 3) {
    std::ifstream infile(argv[2]);
    if (!infile) {
      std::cerr << "Input file error" << std::endl;
      return 1;
    }
    std::cout << "Building FSA from " << argv[2] << "..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    mafsa.load_txt(infile);
    counts_end = std::chrono::high_resolution_clock::now();
    infile.close();
    infile_name = argv[2];
  } else {
    usage(argv[0]);
    return 0;
  }
  std::chrono::duration<double> counts_duration
    = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
  std::cout << "Estimating size..." << std::flush;
  counts_start = std::chrono::high_resolution_clock::now();
  size_t estimated_size = mafsa.size_estimate();
  counts_end = std::chrono::high_resolution_clock::now();
  counts_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
    counts_end - counts_start);
  std::cout << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
  std::cout << "Estimated size before edge compaction: "
            << mafsa.get_num_nodes() << " " << mafsa.get_num_edges() << " "
            << estimated_size << std::endl;

  // Compact nodes and estimate size.
  size_t compaction_level = (size_t) std::stoi(argv[1]);
  for (size_t i = 2; i < compaction_level + 1; ++i) {
    std::cout << "Compacting all 1-to-" << i << " and " << i << "-to-1 nodes..."
              << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    mafsa.compact_nodes(i);
    counts_end = std::chrono::high_resolution_clock::now();
    counts_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
    std::cout << "done! (took " << counts_duration.count() << " seconds)"
              << std::endl;
    std::cout << "Estimating size..." << std::flush;
    counts_start = std::chrono::high_resolution_clock::now();
    estimated_size = mafsa.size_estimate();
    counts_end = std::chrono::high_resolution_clock::now();
    counts_duration = std::chrono::duration_cast<std::chrono::duration<double>>(
      counts_end - counts_start);
    std::cout << "done! (took " << counts_duration.count() << " seconds)"
              << std::endl;
    std::cout << "Estimated size after level " << i << " node compaction: "
              << mafsa.get_num_nodes() << " " << mafsa.get_num_edges() << " "
              << estimated_size << std::endl;
  }
  mafsa.dump_bin(std::cout);
  return 0;
}
