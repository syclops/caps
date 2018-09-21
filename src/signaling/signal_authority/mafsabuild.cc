/**
 * MAFSA construction utility
 * Author: Steve Matsumoto
 */

// Include C standard libraries.
#include <ctime>

// Include C++ standard libraries.
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

// Include header files from this project.
#include "../../../test/src/mafsadebug.h"

// Include header files from other projects.
#include <boost/program_options.hpp>

void usage()
{
  std::cerr << "Usage: ./mafsabuild <infile>" << std::endl;
}

DAFSADebug load_mafsa(bool load_plaintext, const std::string& infile_name)
{
  namespace chr = std::chrono;
  chr::high_resolution_clock::time_point counts_start, counts_end;
  DAFSADebug mafsa;
  if (infile_name.empty()) {
    if (load_plaintext) {
      std::cerr << "Building MAFSA from its representation on standard input..."
                << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.load_txt(std::cin);
      counts_end = chr::high_resolution_clock::now();
    } else {
      std::cerr << "Building MAFSA from strings over standard input..."
                << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.read_file(std::cin);
      counts_end = chr::high_resolution_clock::now();
    }
  } else {
    if (load_plaintext) {
      std::cerr << "Loading MAFSA representation from file " << infile_name
                << "..." << std::flush;
      std::ifstream infile(infile_name);
      counts_start = chr::high_resolution_clock::now();
      mafsa.load_txt(infile);
      counts_end = chr::high_resolution_clock::now();
    } else {
      std::cerr << "Building MAFSA from plaintext file " << infile_name << "..."
                << std::flush;
      std::ifstream infile(infile_name);
      counts_start = chr::high_resolution_clock::now();
      mafsa.read_file(infile);
      counts_end = chr::high_resolution_clock::now();
    }
  }
  chr::duration<double> counts_duration
    = chr::duration_cast<chr::duration<double>>(
      counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
  return mafsa;
}

void compact_edges(DAFSADebug& mafsa)
{
  namespace chr = std::chrono;
  chr::high_resolution_clock::time_point counts_start, counts_end;
  chr::duration<double> counts_duration;
  std::cerr << "Compacting edges..." << std::flush;
  counts_start = chr::high_resolution_clock::now();
  mafsa.compact_edges();
  counts_end = chr::high_resolution_clock::now();
  counts_duration = chr::duration_cast<chr::duration<double>>(
    counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
}

void compact_nodes(DAFSADebug& mafsa, const size_t compaction_level)
{
  namespace chr = std::chrono;
  chr::high_resolution_clock::time_point counts_start, counts_end;
  chr::duration<double> counts_duration;
  std::cerr << "Compacting nodes that are up to 1-to-" << compaction_level
            << "..." << std::flush;
  counts_start = chr::high_resolution_clock::now();
  mafsa.compact_nodes(compaction_level);
  counts_end = chr::high_resolution_clock::now();
  counts_duration += chr::duration_cast<chr::duration<double>>(
    counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
}

void write_huffman(const DAFSADebug& mafsa, const std::string& outfile_name)
{
  namespace chr = std::chrono;
  chr::high_resolution_clock::time_point counts_start, counts_end;
  chr::duration<double> counts_duration;
  std::cerr << "Creating Huffman codebook..." << std::flush;
  counts_start = chr::high_resolution_clock::now();
  HuffmanCoder codebook(mafsa.get_label_counts());
  counts_end = chr::high_resolution_clock::now();
  counts_duration = chr::duration_cast<chr::duration<double>>(
    counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
  counts_end = chr::high_resolution_clock::now();
  if (outfile_name.empty()) {
    std::cerr << "Dumping Huffman codebook to stdout..." << std::flush;
    counts_start = chr::high_resolution_clock::now();
    codebook.dump_codebook_ascii(std::cout);
    counts_end = chr::high_resolution_clock::now();
  } else {
    std::ofstream outfile(outfile_name);
    std::cerr << "Dumping Huffman codebook to " << outfile_name << "..."
              << std::flush;
    counts_start = chr::high_resolution_clock::now();
    codebook.dump_codebook_ascii(outfile);
    counts_end = chr::high_resolution_clock::now();
  }
  counts_duration = chr::duration_cast<chr::duration<double>>(
    counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
}

void write_mafsa(const DAFSADebug& mafsa, bool write_binary,
                 const std::string& outfile_name, const std::string& suffix)
{
  namespace chr = std::chrono;
  chr::high_resolution_clock::time_point counts_start, counts_end;
  chr::duration<double> counts_duration;
  if (outfile_name.empty()) {
    if (write_binary) {
      std::cerr << "Writing binary representation to stdout..." << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.dump_bin(std::cout);
      counts_end = chr::high_resolution_clock::now();
    } else {
      std::cerr << "Writing plaintext representation to stdout..."
                << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.dump_txt(std::cout);
      counts_end = chr::high_resolution_clock::now();
    }
  } else {
    std::ofstream outfile(outfile_name + suffix);
    if (write_binary) {
      std::cerr << "Writing binary representation to " << outfile_name << "..."
                << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.dump_bin(outfile);
      counts_end = chr::high_resolution_clock::now();
    } else {
      std::cerr << "Writing plaintext representation to " << outfile_name
                << "..." << std::flush;
      counts_start = chr::high_resolution_clock::now();
      mafsa.dump_txt(outfile);
      counts_end = chr::high_resolution_clock::now();
    }
  }
  counts_duration = chr::duration_cast<chr::duration<double>>(
    counts_end - counts_start);
  std::cerr << "done! (took " << counts_duration.count() << " seconds)"
            << std::endl;
}

int main(int argc, char* argv[])
{
  try {
    namespace po = boost::program_options;
    po::options_description desc("Options");
    std::string infile_name, outfile_name;
    size_t compaction_level;
    desc.add_options()
      ("write-binary,b", "Write compressed MAFSA in binary format")
      ("compression-level,c", po::value<size_t>(&compaction_level),
       "Level of compression to use (default 0)")
      ("help,h", "Print this help message")
      ("infile,i", po::value<std::string>(&infile_name), "Input file")
      ("load-representation,l",
       "Load a plaintext MAFSA representation instead of creating a new MAFSA")
      ("outfile,o", po::value<std::string>(&outfile_name), "Output file")
      ("huffman,H", "write the canonical Huffman codebook instead")
      ;
    po::variables_map option_map;
    try {
      po::store(po::parse_command_line(argc, argv, desc), option_map);
      po::notify(option_map);
      if (option_map.count("help")) {
        usage();
        return 0;
      }
      if (!option_map.count("infile")) {
        infile_name = "";
      }
      bool load_plaintext = option_map.count("load-representation");
      DAFSADebug mafsa = load_mafsa(load_plaintext, infile_name);
      if (!option_map.count("outfile")) {
        outfile_name = "";
      }
      if (option_map.count("huffman")) {
        write_huffman(mafsa, outfile_name);
        return 0;
      }
      if (!option_map.count("compression-level")) {
        compaction_level = 0;
      }
      bool write_binary = option_map.count("write-binary");
      write_mafsa(mafsa, write_binary, outfile_name, "");
      if (compaction_level > 0) {
        compact_edges(mafsa);
        write_mafsa(mafsa, write_binary, outfile_name, "edge");
      }
      if (compaction_level > 1) {
        for (size_t n = 2; n <= compaction_level; ++n) {
          compact_nodes(mafsa, n);
          write_mafsa(mafsa, write_binary, outfile_name, std::to_string(n));
        }
      }
    } catch (po::error& e) {
      std::cerr << "ERROR: " << e.what() << std::endl << std::endl << desc
                << std::endl;
      return 1;
    }
  } catch (std::exception& e) {
    std::cerr << "Unhandled exception reached the top of main:" << e.what()
              << ", exiting" << std::endl;
    return 2;
  }
  return 0;
}
