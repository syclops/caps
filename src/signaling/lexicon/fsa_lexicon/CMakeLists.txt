# CAPS FSA Lexicon Configuration
# Author: Steve Matsumoto <stephanos.matsumoto@sporic.me>

# TODO: add visitors here: accept string

add_library(accept_string_visitor accept_string_visitor.h
    accept_string_visitor.cc
)
target_link_libraries(accept_string_visitor
    PUBLIC node visitor
)

add_library(node_right_language node_right_language.h node_right_language.cc)
target_link_libraries(node_right_language
    PUBLIC node
)

add_library(fsa_lexicon fsa_lexicon.h fsa_lexicon.cc)
target_link_libraries(fsa_lexicon
    PUBLIC node graph visitor accept_string_visitor lexicon node_right_language
    PRIVATE contains powerset connected_component
            connected_component_utils graph_search
)
