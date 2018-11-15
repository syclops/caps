# CAPS Lexicon Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(lexicon lexicon.h lexicon.cc)

add_library(set_lexicon set_lexicon.h set_lexicon.cc)
target_link_libraries(set_lexicon INTERFACE lexicon)

add_subdirectory(fsa_lexicon)

