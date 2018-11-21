# CAPS FSA Lexicon Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# TODO: add visitors here: accept string

find_package(Boost
        REQUIRED
)

add_library(node_right_language node_right_language.h node_right_language.cc)
target_link_libraries(node_right_language
        PUBLIC node
        PRIVATE Boost::boost
)

add_library(accept_string_visitor accept_string_visitor.h
    accept_string_visitor.cc
)
target_link_libraries(accept_string_visitor
    PUBLIC node
    INTERFACE visitor
)

add_library(fsa_lexicon fsa_lexicon.h fsa_lexicon.cc)
target_link_libraries(fsa_lexicon
    PUBLIC lexicon node_right_language
    INTERFACE node visitor graph
    PRIVATE powerset bitvector connected_component connected_component_utils
            graph_search accept_string_visitor

)
