#include "filter.h"

static mapv* const* filter[] =
  #define FILTER_DATA
  #undef  TREE_DATA
  #include "../data.c"
;

const tree_init_data tree_data =
  #undef  FILTER_DATA
  #define TREE_DATA
  #include "../data.c"
;

mapv* const* filter_get(int c) {
  return filter[c];
}
