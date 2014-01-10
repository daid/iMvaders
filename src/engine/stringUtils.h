#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <stdlib.h>
#include <stdio.h>

/* Utility function in assisting to convert numbers to strings */
static inline std::string to_string(int number)
{
    char buf[16];
    sprintf(buf, "%i", number);
    return std::string(buf);
}

static inline std::string to_string(unsigned int number)
{
    char buf[16];
    sprintf(buf, "%d", number);
    return std::string(buf);
}

static inline std::string to_string(double number)
{
    char buf[16];
    sprintf(buf, "%.2f", number);
    return std::string(buf);
}

#endif//STRING_UTILS_H
