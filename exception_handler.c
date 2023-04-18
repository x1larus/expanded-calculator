#include "exception_handler.h"

#include <stdio.h>
#include <stdlib.h>

void exception(const char comment[], const char* func, const char *file, int line)
{
    fprintf(stderr, "In function %s() (%s:%d): %s\n", func, file, line, comment);
    exit(EXIT_FAILURE);
}