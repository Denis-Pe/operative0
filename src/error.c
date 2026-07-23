#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void panic_cstr(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    abort();
}

void panic_errno() {
    fprintf(stderr, "Panic on Errno: %s\n", strerror(errno));
    abort();
}

void panic() {
    fprintf(stderr, "Panic: Unrecoverable error\n");
    abort();
}
