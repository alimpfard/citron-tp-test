#include "word-tree.h"
#include "filter.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#if defined(LEVENSORT) | defined(TEST)
#define IS_TIMSORT_R
#include "timsort.c"
#endif

int enc_simple(WIDE_CHAR_T c, word_tree_t tree) {
  return (int)(c - tree->alphabet_first + 1);
}

WIDE_CHAR_T dec_simple(int e, word_tree_t tree) {
  return (WIDE_CHAR_T)(e + tree->alphabet_first - 1);
}

void *allocate_empty_node(int count) {
  struct word_tree_node *root = malloc(sizeof *root);
  root->filter_data.enabled = 1;
  root->filter_data.proj_prob = 1;
  root->cells = calloc(count, sizeof(root));
  return root;
}

#define LOAD_FACTOR 84
#define INCREASE_FACTOR 2
static inline char const *dynarray_insert(word_tree_t tree, char *str) {
  if (tree->filled * 100 / tree->allocd > LOAD_FACTOR) {
    tree->allocd *= INCREASE_FACTOR;
    tree->data = realloc(tree->data, tree->allocd * sizeof(char *));
  }
  char **p = tree->data + (tree->filled++);
  *p = str;
  return str;
}

static inline void dynarray_free(word_tree_t tree) {
  if(!tree->allocd) return;
  free(tree->data);
  tree->allocd = 0;
}

int tree_insert(word_tree_t tree, char *str) {
  WIDE_CHAR_T c;
  int e;
  struct word_tree_node *node = tree->root;
  char *ss = str;
  ssize_t it;
  ssize_t size = strlen(str);
  while (*str && ((it = utf8proc_iterate((utf8proc_uint8_t*)str, size, &c)) > 0)) {
    str += it;
    size -= it;
    e = tree->encoder(c, tree);
    struct word_tree_node **cell = node->cells + e;
    if (!*cell)
      *cell = allocate_empty_node(tree->alphabet_count);
    node = *cell;
  }
  e = 0; // EOW
  struct word_tree_node **cell = node->cells + e;
  if (*cell) {
    // this word exists
    // printf("Word '%s' already exists\n", ss);
    return 1;
  } else {
    node = *cell = allocate_empty_node(tree->alphabet_count);
    node->filter_data.enabled = 1;
    node->filter_data.proj_prob = 1;
    node->filter_data.data_ptr = dynarray_insert(tree, strdup(ss));
    // printf("Node %p has data ptr %p which is string '%s'\n", node,
    // node->filter_data.data_ptr, ss);
  }
  return 0;
}

static inline void free_nodes(struct word_tree_node* node, int count) {
  int i;
  struct word_tree_node** snode;
  for (snode = node->cells, i = 0; i<count; snode++,i++)
    if (*snode)
      free_nodes(*snode, count);
  free(node);
}

void tree_free(word_tree_t tree) {
  if (!tree) return;
  // free the data
  dynarray_free(tree);
  // free the nodes
  free_nodes(tree->root, tree->alphabet_count);
  // free the tree
  free(tree);
}

word_tree_t tree_freadf(const char *path, const char *format, int rd_mode) {
  int alphabet_count = 26;
  int initial_size = 16;
  decoder_fn_ptr_t decoder = &dec_simple;
  encoder_fn_ptr_t encoder = &enc_simple;
  WIDE_CHAR_T alphabet_first = L'a';

  // parse format spec
  // a<int>  -- set [a]lphabet count
  // f<char> -- set [f]irst character in alphabet
  // s<int>  -- set initial data [s]ize
  // --
  if (format) {
    WIDE_CHAR_T f;
    ssize_t it;
    ssize_t size = strlen(format);
    int n;
    while (*format && ((it = utf8proc_iterate((utf8proc_uint8_t*)format, size, &f)) > 0)) {
      format += it;
      size -= it;

      switch (f) {
      case 'a':
        sscanf(format, "%i%n", &alphabet_count, &n);
        format += n;
        break;
      case 's':
        sscanf(format, "%i%n", &initial_size, &n);
        format += n;
        break;
      case 'f':
        format += utf8proc_iterate((utf8proc_uint8_t*)format, -1, &f);
        alphabet_first = f;
        break;
        // ...
      default:
        fprintf(stderr, "%lc is not a valid format specifier\n", f);
      }
    }
  } // allocate and init tree
  word_tree_t tree = malloc(sizeof *tree);
  tree->root = allocate_empty_node(alphabet_count + 1);
  tree->data = malloc(sizeof(char *) * initial_size);
  tree->filled = 0;
  tree->allocd = initial_size;
  tree->max_height = 0;
  tree->alphabet_count = alphabet_count + 1; // plus one for <EOW>
  tree->alphabet_first = alphabet_first;
  tree->decoder = decoder;
  tree->encoder = encoder;

  // read data
  switch (rd_mode) {
  case TREE_RD_MODE_BINARY:
    fputs("BINARY read mode not implemented", stderr);
    abort();
    break;
  case TREE_RD_MODE_PLAIN: {
    char fbuf[512]; // max word length 512 O_o overkill much
    FILE *fp = fopen(path, "r");
    while (fscanf(fp, "%511s", fbuf) == 1) {
      // printf("Read %s\n", fbuf);
      tree_insert(tree, fbuf);
    }
    fclose(fp);
  } break;
  }
  return tree;
}

int tree_exists(word_tree_t tree, char *str) {
  WIDE_CHAR_T f;
  ssize_t it;
  ssize_t size = strlen(str);
  int e;
  struct word_tree_node *node = tree->root;
  while (*str && ((it = utf8proc_iterate((utf8proc_uint8_t*)str, size, &f)) > 0)) {
    str += it;
    size -= it;

    e = tree->encoder(f, tree);
    struct word_tree_node **cell = node->cells + e;
    if (!*cell)
      return 0;
    node = *cell;
  }
  return !!node->cells[0];
}

int tree_filter_if_not_exist_(struct word_tree_node *node, word_tree *tree,
                              char *str) {
  if (!node->filter_data.enabled)
    return 0;

  WIDE_CHAR_T c;
  ssize_t it = utf8proc_iterate((utf8proc_uint8_t*)str, strlen(str), &c);
  if (it < 0) {
    printf("Invalid multi-byte string lookup\n");
    abort();
  }
  str += it;
  int e;
  if (it == 0 || c == 0) // end of word
    e = 0;
  else
    e = tree->encoder(c, tree);
  int i;
  struct word_tree_node **cellv;
  for (i = 0, cellv = node->cells + i; i < tree->alphabet_count;
       i++, cellv = node->cells + i)
    if (*cellv)
      (*cellv)->filter_data.enabled = 0;
  for (mapv *const *p = filter_get(e); *p; p++) {
    struct word_tree_node **cell = node->cells + (*p)->value;
    if (*cell) {
      float prob = (*p)->prob * node->filter_data.proj_prob;
      (*cell)->filter_data.proj_prob = prob;
      if (prob >= 0.5) {
        (*cell)->filter_data.enabled = 1;
        tree_filter_if_not_exist_(*cell, tree, str);
      }
    }
  }
  return 0;
}

void conditional_dfs_into(struct word_tree_node *node, int sl, char const**vec,
                          int vs, int *idx) {
  if (!node)
    return;
  if (!node->filter_data.enabled)
    return;
  if (*idx == vs)
    return;
  if (node->cells[0]) {
    // printf("idx %d will have '%s'\n", *idx,
    // node->cells[0]->filter_data.data_ptr);
    vec[(*idx)++] = node->cells[0]->filter_data.data_ptr;
  }
  for (int i = 1; i < sl; i++)
    conditional_dfs_into(node->cells[i], sl, vec, vs, idx);
}

// -1 -> exists
// ~  -> how many were assigned into filtered_v
int tree_filter_if_not_exist(word_tree_t tree, char *str, char const**filtered_v,
                             int fvsize) {
  if (tree_exists(tree, str))
    return -1;
  int res = tree_filter_if_not_exist_(tree->root, tree, str);
  if (!fvsize || !filtered_v)
    return res;
  struct word_tree_node *node = tree->root;
  int idx = 0;
  for (int i = 1; i < tree->alphabet_count; i++)
    conditional_dfs_into(node->cells[i], tree->alphabet_count, filtered_v,
                         fvsize, &idx);
  return idx;
}

// Returns a size_t, depicting the difference between `a` and `b`.
size_t levenshtein_n(const char *a, const size_t length, const char *b,
                     const size_t bLength) {
  size_t *cache = calloc(length, sizeof(size_t));
  size_t index = 0;
  size_t bIndex = 0;
  size_t distance;
  size_t bDistance;
  size_t result;
  char code;

  // Shortcut optimizations / degenerate cases.
  if (a == b) {
    return 0;
  }

  if (length == 0) {
    return bLength;
  }

  if (bLength == 0) {
    return length;
  }

  // initialize the vector.
  while (index < length) {
    cache[index] = index + 1;
    index++;
  }

  // Loop.
  while (bIndex < bLength) {
    code = b[bIndex];
    result = distance = bIndex++;
    index = SIZE_MAX;

    while (++index < length) {
      bDistance = code == a[index] ? distance : distance + 1;
      distance = cache[index];

      cache[index] = result =
          distance > result ? bDistance > result ? result + 1 : bDistance
                            : bDistance > distance ? distance + 1 : bDistance;
    }
  }

  free(cache);

  return result;
}

size_t levenshtein(const char *a, const char *b) {
  const size_t length = strlen(a);
  const size_t bLength = strlen(b);
  return levenshtein_n(a, length, b, bLength);
}

int slevenshtein(const void *a, const void *b, void *sstr) {
  char *x = *(char**)a, *y = *(char**)b;
  return levenshtein(x, sstr) - levenshtein(y, sstr);
}

#ifdef LEVENSORT
int tree_filter_if_not_exist_sorted(word_tree_t tree, char *str, char const**filtered_v,
                            int fvsize) {
  int res = tree_filter_if_not_exist(tree, str, filtered_v, fvsize);
  if (res < 2)
    return res; // there's no point sorting these cases
  int val = timsort_r(filtered_v, res, sizeof(filtered_v[0]), &slevenshtein, str);
  if (val)
    fprintf (stderr, "Timesort failed with result %d\n", val);
  return res;
}
#endif

#ifdef TEST
int main(int argc, char **argv) {
  word_tree_t tree = tree_freadf("dict", "a26s64", TREE_RD_MODE_PLAIN);
  char *vec[1024];
  char *sstr = argc > 1 ? argv[1] : "twst";
  int st = tree_filter_if_not_exist(tree, sstr, vec, 1024);
  if (st == -1)
    puts("That word exists in the dict");
  else if (st == 0)
    puts("No matches found");
  else {
    printf("We got %d result(s) for '%s'\n", st, sstr);
    printf("Sorting with leven...");
    int val = timsort_r(vec, st, sizeof(vec[0]), slevenshtein, sstr);
    printf("done: %d\n", val);
    for (int i = 0; i < st; i++) {
      fputs(vec[i], stdout);
      puts("");
    }
  }
  return 0;
}
#endif
