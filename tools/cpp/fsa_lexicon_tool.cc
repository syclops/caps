//
// Created by smaptas on 07.11.17.
//

#include <chrono>
#include <iostream>
#include <fstream>
#include <utility>

#include "../../src/cpp/lexicon/fsa_lexicon.h"
#include "../../src/cpp/common/io_option.h"
#include "../../src/cpp/common/measure_time.h"

#include <cxxopts.hpp>

FSALexicon make_lexicon(std::istream& in_stream)
{
  FSALexicon lexicon;
  lexicon.add_file(in_stream);
  return lexicon;
}

FSALexicon load_lexicon(std::istream& in_stream)
{
  FSALexicon lexicon;
  lexicon.load(in_stream);
  return lexicon;
}

void write_lexicon(std::ostream& out_stream, const FSALexicon& lexicon)
{
//  lexicon.dump(out_stream);
  lexicon.dump_label_huffman(out_stream);
}

int main(int argc, char* argv[])
{
  try {
    cxxopts::Options options(argv[0], "FSA-based lexicon utility");
    bool load = false;
    options.add_options()
      ("i,infile", "Input file",
       cxxopts::value<std::string>()->default_value(""))
      ("l,load", "Load a representation", cxxopts::value<bool>(load))
      ("h,help", "Display this help message")
      ("o,outfile", "Output file",
       cxxopts::value<std::string>()->default_value(""));
    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
      std::cout << options.help({""}) << std::endl;
      return 0;
    }

    const auto infile = result["infile"].as<std::string>();
    const auto outfile = result["outfile"].as<std::string>();

    // Make or load an FSALexicon from standard input or an input file.
    FunctionTimer<FSALexicon, std::string, bool> in_timer([](std::string infile,
                                                             bool load) {
      return input_option(load ? load_lexicon: make_lexicon, infile);
    });
    std::cout << (load ? "Loading" : "Making") << " lexicon from "
              << (infile.empty() ? "standard input" : infile)
              << "..." << std::flush;
    auto lexicon = in_timer.run(infile, load);
    std::cout << "done! (took " << in_timer.time() << " seconds)" << std::endl;

    // Compact long edges.
    FunctionTimer<void, FSALexicon&> compact_timer([](FSALexicon& lexicon) {
      lexicon.compact_long_edges();
    });
    std::cout << "Compacting lexicon..." << std::flush;
    compact_timer.run(lexicon);
    std::cout << "done! (took " << compact_timer.time() << " seconds)" << std::endl;

    // Write the compacted lexicon to file.
    FunctionTimer<void, std::string, FSALexicon&> out_timer([](
      std::string outfile, FSALexicon& lexicon) {
      output_option(write_lexicon, outfile, lexicon);
    });
    std::cout << "Writing lexicon to "
              << (outfile.empty() ? "standard output" : outfile)
              << "..." << std::flush;
    out_timer.run(outfile, lexicon);
    std::cout << "done! (took " << compact_timer.time() << " seconds)" << std::endl;

//    std::cout << "Lexicon has " << lexicon.get_graph().get_num_nodes()
//              << " nodes, " << lexicon.get_graph().get_num_edges()
//              << " edges, and " << lexicon.get_graph().get_num_accept()
//              << " accepting states" << std::endl;
//    std::cout << "Debug: " << lexicon.debug() << std::endl;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
  return 0;
}