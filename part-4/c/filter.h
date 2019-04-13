#ifndef FILTER_H
#define FILTER_H

struct mapv {
    int value;
    float prob;
};

struct tree_init_data {
  char const* format;
};

typedef struct mapv mapv;
typedef struct tree_init_data tree_init_data;

mapv* const* filter_get(int);
const tree_init_data tree_data;

#endif
