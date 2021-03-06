//
// Created by smaptas on 07.11.17.
//

#include <chrono>
#include <iostream>
#include <fstream>
#include <utility>
#include <map>

#include "lexicon/fsa_lexicon/fsa_lexicon.h"
#include "common/io_option.h"
#include "common/measure_time.h"
#include "encoding/fsa_encoder/fsa_encoder.h"
#include "encoding/fsa_encoder/fsa_char_encoder.h"
#include "encoding/fsa_encoder/fsa_huffman_encoder.h"
#include "encoding/fsa_encoder/fsa_mixed_huffman_encoder.h"
#include "encoding/fsa_encoder/fsa_partial_huffman_encoder.h"
#include "encoding/fsa_encoder/fsa_char_huffman_encoder.h"
#include "encoding/bitvector_io.h"

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

void write_lexicon(std::ostream& out_stream,
                   FSAEncoder<BitVector<>>* encoder)
{
  out_stream << encoder->encode();
}

void print_lexicon_info(const FSALexicon& lexicon)
{
  std::cout << "Lexicon has " << lexicon.get_graph().get_num_nodes()
            << " nodes and " << lexicon.get_graph().get_num_edges() << " edges"
            << std::endl;
/*  std::map<int, int> mp{};
  for(auto& [label, count]: lexicon.get_graph().get_label_counts()){
    if (mp.find(count)!=mp.end()){
      mp.at(count)++;
    }else{
      mp.emplace(count, 1);
    }
  }
  for(auto& [length, count]: mp){
    std::cout << length << " : " << count << std::endl;
  }*/
}

struct option_string
{
  option_string(std::string short_option, std::string long_option)
    : short_option{std::move(short_option)}, long_option{std::move(long_option)}
  {
    // Nothing to do here.
  }

  std::string full_option() const
  {
    return short_option + "," + long_option;
  }

  std::string short_option;
  std::string long_option;
};

enum class Option {
  debug,
  help,
  infile,
  load,
  outfile,
  path_compaction,
  transition_compaction
};

struct tool_options
{
  tool_options(const cxxopts::ParseResult& result,
               const std::unordered_map<Option, option_string>& option_map)
    : in_file{result[option_map.at(
        Option::infile).long_option].as<std::string>()},
      out_file{result[option_map.at(
        Option::outfile).long_option].as<std::string>()},
      help{result.count(option_map.at(Option::help).long_option) > 0},
      load{result.count(option_map.at(Option::load).long_option) > 0},
      path_compact{result.count(option_map.at(
        Option::path_compaction).long_option) > 0},
      transition_compact{result.count(option_map.at(
        Option::transition_compaction).long_option) > 0}
  {
    // Nothing to do here.
  }

  std::string in_file;
  std::string out_file;
  bool help;
  bool load;
  bool path_compact;
  bool transition_compact;
};

const std::unordered_map<Option, option_string> OPTION_MAP{
  {Option::debug, {"d", "debug"}},
  {Option::infile, {"i", "infile"}},
  {Option::load, {"l", "load"}},
  {Option::help, {"h", "help"}},
  {Option::outfile, {"o", "outfile"}},
  {Option::path_compaction, {"p", "path-compaction"}},
  {Option::transition_compaction, {"t", "transition-compaction"}},
};

/**
 * Make the Options instance used in argument parsing.
 *
 * @param program_name
 * @return
 */
cxxopts::Options make_options(char* program_name)
{
  cxxopts::Options options(program_name, "FSA-based lexicon utility");
  auto get_full_option = [](Option o){
    return OPTION_MAP.at(o).full_option();
  };
  options.add_options()
           (get_full_option(Option::infile), "Input file",
            cxxopts::value<std::string>()->default_value(""))
           (get_full_option(Option::load), "Load a representation")
           (get_full_option(Option::help), "Display this help message")
           (get_full_option(Option::outfile), "Output file",
            cxxopts::value<std::string>()->default_value(""))
           (get_full_option(Option::path_compaction), "Use path compaction")
           (get_full_option(Option::transition_compaction),
            "Use transition compaction");
  return options;
}

/**
 * NOTE: the options argument should be marked const, except that the cxxopts
 *       library does not mark its parse function const (even though its
 *       functionality does not change the calling Option instance).
 *
 * @param options
 * @param argc
 * @param argv
 * @return
 */
tool_options parse_arguments(int argc, char* argv[])
{
  try {
    // Parse command-line arguments.
    auto options = make_options(argv[0]);
    tool_options parsed{options.parse(argc, argv), OPTION_MAP};

    // If help flag was passed, show help.
    if (parsed.help) {
      std::cout << options.help({""}) << std::endl;
    }

    return parsed;
  } catch (const cxxopts::OptionException& e) {
    std::cout << "Error parsing options: " << e.what() << std::endl;
    exit(1);
  }
}

constexpr static char END_OF_STRING = static_cast<char>(0x80);

std::pair<LabeledGraph::LabelMap, std::unordered_map<char, size_t>> get_new_counts(const LabeledGraph::LabelMap& counts){
  LabeledGraph::LabelMap counts1;
  std::unordered_map<char, size_t> counts2;
  for (const auto& [symbol, count]: counts){
    if (/*symbol.length()!=1 && count>3 && symbol.length()<=30*/
      count!=1) counts1.emplace(symbol, count);
    else{
      for(auto c: symbol){
        if (counts2.find(c)!=counts2.end()) counts2.at(c) += count;
        else counts2.emplace(c, count);
      }
      if (counts2.find(END_OF_STRING)!=counts2.end()) counts2.at(END_OF_STRING)+=count;
      else counts2.emplace(END_OF_STRING, count);
    }
  }
  return std::make_pair(counts1, counts2);
}

const LabeledGraph::LabelMap& get_label_counts(
  const FSALexicon& lexicon)
{
  return const_cast<const LabeledGraph::LabelMap&>(lexicon.get_graph().get_label_counts());
}

int main(int argc, char* argv[])
{
  // Parse command-line arguments.
  auto parsed = parse_arguments(argc, argv);

  // If help flag was passed, exit, since parse_arguments printed the help
  // string already.
  if (parsed.help) {
    return 0;
  }

  // TODO: refactor the operations below into their own functions.

  // Make or load an FSALexicon from standard input or an input file.
  FunctionTimer<FSALexicon, std::string, bool> in_timer([](std::string infile,
                                                           bool load) {
    return input_option(load ? load_lexicon: make_lexicon, infile);
  });
  std::cout << (parsed.load ? "Loading" : "Making") << " lexicon from "
            << (parsed.in_file.empty() ? "standard input" : parsed.in_file)
            << "..." << std::flush;
  auto lexicon = in_timer.run(parsed.in_file, parsed.load);
  std::cout << "done! (took " << in_timer.time() << " seconds)" << std::endl;
  print_lexicon_info(lexicon);

  if (parsed.path_compact) {
    FunctionTimer<void, FSALexicon&> compact_timer([](FSALexicon& lexicon) {
      lexicon.compact(3);
    });
    std::cout << "Compacting lexicon..." << std::flush;
    compact_timer.run(lexicon);
    std::cout << "done! (took " << compact_timer.time() << " seconds)"
              << std::endl;
    print_lexicon_info(lexicon);
  }

  using BVType = BitVector<>;
  using EncPtrType = std::unique_ptr<FSAEncoder<BVType>>;
  auto [counts1, counts2] = get_new_counts(get_label_counts(lexicon));
  auto temp_char_coder = std::make_shared<CharHuffmanCoder<std::string,
                                                   BVType>>(counts2);
  auto temp_string_coder = std::make_shared<HuffmanCoder<std::string,
                                                   BVType>>(counts1);
/*
  size_t cnt=0, saved=0;
  for(auto [symbol, count]: counts1){
    if (temp_char_coder->value_size(symbol)<temp_string_coder->value_size(symbol)){
      cnt++;
      saved += (temp_string_coder->value_size(symbol)-temp_char_coder->value_size(symbol))*count;
    }
  }
  std::cout<<"cnt: "<<cnt<<std::endl;
  std::cout<<"saved: "<<saved<<std::endl;
*/
  EncPtrType encoder{parsed.transition_compact
                     ? new FSAMixedHuffmanEncoder<BVType>(lexicon)
                     : new FSAEncoder<BVType>(lexicon)};

//  auto encoder = parsed.transition_compact
//    ? FSAHuffmanEncoder<BitVector<>>{lexicon}
//    : FSAEncoder<BitVector<>>{lexicon};

  // Write the compacted lexicon to file.
  FunctionTimer<void, std::string, const EncPtrType&> out_timer([](
    std::string outfile, const EncPtrType& encoder) {
    output_option(write_lexicon, outfile, encoder.get());
  });
  std::cout << "Writing lexicon to "
            << (parsed.out_file.empty() ? "standard output" : parsed.out_file)
            << "..." << std::flush;
  out_timer.run(parsed.out_file, encoder);
  std::cout << "done! (took " << out_timer.time() << " seconds)" << std::endl;
  return 0;
}
