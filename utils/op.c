// Place for random functions that don't fit anywhere else
#include <stdarg.h>
#include <stdio.h>

#include "op.h"

void verbose_log(const char *format, ...) {
    if (verbose) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}