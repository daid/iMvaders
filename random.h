#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

static inline float random(float fmin, float fmax)
{
    return (float(rand()) / float(RAND_MAX)) * (fmax - fmin) + fmin;
}

#endif//RANDOM_H
