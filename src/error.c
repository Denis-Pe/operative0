#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void panic_cstr(const char *msg) {
    fprintf(stderr, "%s\n", msg);
    abort();
}

void panic_errno() {
    fprintf(stderr, "Panic on Errno: %s\n", strerror(errno));
    abort();
}

void panic() {
    abort();
}
