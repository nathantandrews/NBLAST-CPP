#include "Debug.hpp"

#include <stdarg.h>
#include <stdio.h>

void debug(const char* format, ...) {
    va_list args;
    va_start(args, format);
#ifdef DEBUG
    vfprintf(stderr, format, args);
#endif
    va_end(args);
    return;
}
