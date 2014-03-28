#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>
#include <time.h>

static inline void initRandom()
{
    srand(time(NULL));
}

static inline float random(float fmin, float fmax)
{
    return (float(rand()) / float(RAND_MAX)) * (fmax - fmin) + fmin;
}

#endif//RANDOM_H
