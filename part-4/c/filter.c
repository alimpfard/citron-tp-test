#include "filter.h"

static mapv* const* filter[] = {
#include "../data.c"
};
mapv* const* filter_get(int c) {
  return filter[c];
}
