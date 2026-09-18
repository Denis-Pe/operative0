#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>

noreturn void panicf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    abort();
}

noreturn void panic_errno(void) {
    fprintf(stderr, "Panic on Errno: %s\n", strerror(errno));
    abort();
}

noreturn void panic(void) {
    fprintf(stderr, "Panic: Unrecoverable error\n");
    abort();
}
