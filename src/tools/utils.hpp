#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdio>

#define eprintf( ... ) fprintf(stderr, __VA_ARGS__)
#define eputs(str) fputs(str"\n", stderr)

#endif