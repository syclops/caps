/**
 * Read a set of filenames, each of which contains a set of domain names, and
 * merge them into a file containing a binary string representing the dates on
 * which each was seen.
 */

// Include C standard library headers.

// Include C++ standard library headers.
#include <iostream>

// Include other headers from this project.
#include "../../../src/cpp/common/measure_time.h"
#include "../../../src/cpp/common/io_option.h"
#include "../../../src/cpp/common/contains.h"
#include "domain_map.h"

// Include headers from other projects.
#include <cxxopts.hpp>

std::vector<std::string> get_filenames(std::istream& in_stream)
{
  std::vector<std::string> filenames;
  std::string line;
  while (std::getline(in_stream, line)) {
    filenames.push_back(line);
  }
  return filenames;
}

void write_map(std::ostream& out_stream, DomainMap& domain_map)
{
  for (auto itr: domain_map.map()) {
    out_stream << itr.first << " ";
    for (auto b: itr.second) {
      out_stream << static_cast<int>(b);
    }
    out_stream << std::endl;
  }
}

int main(int argc, char* argv[])
{
  try {
    // Options processing.
    cxxopts::Options options(argv[0], "List when domain names appeared.");
    bool verbose = false;
    options.add_options()
      ("i,infile", "Input list of file names",
       cxxopts::value<std::string>()->default_value(""))
      ("h,help", "Display this help message")
      ("o,outfile", "Output file",
       cxxopts::value<std::string>()->default_value(""))
      ("v,verbose", "Verbose output", cxxopts::value<bool>(verbose))
      ;
    auto result = options.parse(argc, argv);

    // If help requested, display usage and stop.
    if (result.count("help") > 0) {
      std::cout << options.help({""}) << std::endl;
      return 0;
    }

    FunctionTimer<std::vector<std::string>, std::string> filename_timer(
      [](std::string in_file) {
        return input_option(get_filenames, in_file);
      }
    );
    const auto infile = result["infile"].as<std::string>();
    std::cout << "Reading filenames from "
              << (infile.empty() ? "standard input": infile)
              << "..." << std::flush;
    auto filenames = filename_timer.run(result["infile"].as<std::string>());
    std::cout << "done! (took " << filename_timer.time() << " seconds)"
              << std::endl;

    DomainMap domain_map;
    FunctionTimer<void, std::vector<std::string>&> add_timer(
      [&domain_map](std::vector<std::string>& filenames) {
        domain_map.add_files(filenames);
      }
    );
    std::cout << "Adding names into domain map..." << std::flush;
    add_timer.run(filenames);
    std::cout << "done! (took " << add_timer.time() << " seconds)"
              << std::endl;

    FunctionTimer<void, std::string> output_timer(
      [&domain_map](std::string out_file) {
        output_option(write_map, out_file, domain_map);
      }
    );
    const auto outfile = result["outfile"].as<std::string>();
    std::cout << "Writing filenames to "
              << (outfile.empty() ? "standard output": outfile)
              << "..." << std::flush;
    output_timer.run(outfile);
    std::cout << "done! (took " << output_timer.time() << " seconds)"
              << std::endl;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}