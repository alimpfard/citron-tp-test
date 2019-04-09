#ifndef FILTER_H
#define FILTER_H

struct mapv {
    int value;
    float prob;
};

typedef struct mapv mapv;

mapv* const* filter_get(int);

#endif
