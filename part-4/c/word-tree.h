#ifndef WORD_TREE_H
#define WORD_TREE_H

#include <stdlib.h>
#include "utf8/utf8proc.h"

#define WIDE_CHAR_T utf8proc_int32_t
#define TREE_RD_MODE_BINARY 0
#define TREE_RD_MODE_PLAIN  1

static const char end_of_word_marker_c = 0;
static const char* EOW = &end_of_word_marker_c;

struct word_tree_node {
    //void* metadata;
    struct filter_data {
        int enabled;     ///< Is this node (and all children) enabled?
        float proj_prob; ///< if (enabled), set to the value of its probability P_c
        char const* data_ptr; ///< holds a pointer to a data array element containing the value if it is a leaf node, unspecified otherwise
    } filter_data;
//#ifdef ALPHABET_SIZE
//    struct word_tree_node cells[ALPHABET_SIZE];
//#else
    struct word_tree_node* *cells; // allocated dynamically
//#endif
};

struct word_tree_path {
    int max_length;
    int length;
    int *path;
};

typedef struct word_tree word_tree;

typedef int (*encoder_fn_ptr_t)(WIDE_CHAR_T,struct word_tree*); // encodes a wide char to an int in range [0, alphabet_count)
typedef WIDE_CHAR_T (*decoder_fn_ptr_t)(int,struct word_tree*); // decodes an int in range [0, alphabet_count) to a wide char

struct word_tree {
    struct word_tree_node* root;
    char** data;
    // idea: array that keeps "enabled"
    size_t filled;
    size_t allocd;
    size_t max_height;
    int alphabet_count;
    WIDE_CHAR_T alphabet_first;
    encoder_fn_ptr_t encoder;
    decoder_fn_ptr_t decoder;
};

typedef word_tree* word_tree_t;


#endif
