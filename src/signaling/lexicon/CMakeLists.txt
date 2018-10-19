# CAPS Lexicon Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

add_library(lexicon lexicon.h lexicon.cc)

add_library(set_lexicon set_lexicon.h set_lexicon.cc)
target_link_libraries(set_lexicon INTERFACE lexicon)

add_library(fsa_lexicon fsa_lexicon.h fsa_lexicon.cc)
target_link_libraries(fsa_lexicon
    PUBLIC node graph visitor
    INTERFACE lexicon
    PRIVATE contains powerset connected_component graph_search
)