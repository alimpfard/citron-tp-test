#ifndef WORD_TREE_H
#define WORD_TREE_H

#include <stdlib.h>
#include "utf8/utf8proc.h"

#define WIDE_CHAR_T utf8proc_int32_t
#define TREE_RD_MODE_BINARY 0
#define TREE_RD_MODE_PLAIN  1

#define MIN_ACCEPTABLE_PROB 0.5

#define LEVENSORT

struct word_tree_node {
    //void* metadata;
    struct filter_data {
        int enabled;           ///< Is this node (and all children) enabled?
        float proj_prob;       ///< if (enabled), set to the value of its probability P_c
        char const* data_ptr;  ///< holds a pointer to a data array element containing the value if it is a leaf node, unspecified otherwise
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

typedef int (*encoder_fn_ptr_t)(WIDE_CHAR_T,struct word_tree*); ///< encodes a wide char to an int in range [0, alphabet_count)
typedef WIDE_CHAR_T (*decoder_fn_ptr_t)(int,struct word_tree*); ///< decodes an int in range [0, alphabet_count) to a wide char

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

/**
 * Read a tree from a file
 *
 * @param path - path to the data file
 * @param format - tree format specifier:
 *     a<int> - alphabet count (default = 26)
 *     f<code point> - alphabet's first character (default = L'a')
 *     s<int> - initial tree size (default = 16)
 * @param rd_mode - read mode (set to `TREE_RD_MODE_PLAIN`)
 *
 * @return - the created tree
 */
word_tree_t tree_freadf(const char *path, const char *format, int rd_mode);

/**
 * Frees the given tree
 *
 * @param tree - tree to free
 */
void tree_free(word_tree_t tree);

/**
 * Insert word into tree if not exists
 *
 * @param tree - the tree to insert into
 * @param str  - the string to insert
 *
 * @return whether the string was inserted
 */
int tree_insert(word_tree_t tree, char *str);

/**
 * check if string exists inside the tree
 *
 * @param tree - the tree
 * @param str  - the string to check for
 *
 * @return whether the string exists
 */
int tree_exists(word_tree_t tree, char *str);

/**
 * Check tree or filter the tree into the given vector space
 *
 * @param tree - the tree to filter
 * @param str  - the string to check
 * @param filtered_v - a vector of size >= `fvsize` for the results
 * @param fvsize - the number of desired filter results
 *
 * @return
 *     `-1`: if the string exists
 *     anything else: the number of filtered results stored into filtered_v
 *
 * @notes:
 * do not free nor modify the strings returned in `filtered_v`.
 */
int tree_filter_if_not_exist(word_tree_t tree, char *str, char const**filtered_v,
                             int fvsize);
#ifdef LEVENSORT
 /**
  * Check tree or filter the tree into the given vector space, returns a sorted vector
  *
  * @param tree - the tree to filter
  * @param str  - the string to check
  * @param filtered_v - a vector of size >= `fvsize` for the results
  * @param fvsize - the number of desired filter results
  *
  * @return
  *     `-1`: if the string exists
  *     anything else: the number of filtered results stored into filtered_v
  *
  * @notes:
  * do not free nor modify the strings returned in `filtered_v`.
  */
 int tree_filter_if_not_exist_sorted(word_tree_t tree, char *str, char const**filtered_v,
                              int fvsize);
#endif

// utility functions
size_t levenshtein(const char *a, const char *b);

#endif
