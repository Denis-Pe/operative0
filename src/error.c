#include "error.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdnoreturn.h>
#include "string/string.h"

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

void report_err(const Error err) {
    if (err.location.len > 0) {
        size_t line_idx = 0;
        size_t i = 0;
        const size_t srclen = strv_len(err.location.source);
        while (i < srclen) {
            const uint32_t c = strv_char_at(err.location.source, i);
            if (c == '\n') line_idx = i + 1;
            if (i >= err.location.src_idx) {
                break;
            }
            i++;
        }
        const size_t start = line_idx > 0 ? err.location.src_idx - line_idx - 1 : err.location.src_idx;
        i = 0;
        while (true) {
            const uint32_t c = strv_char_at(err.location.source, line_idx + i);
            fprintf(stderr, "%c", (char) c);
            if (c == '\n') {
                break;
            }
            if (line_idx + i + 1 >= srclen) {
                fprintf(stderr, "\n");
                break;
            }
            i++;
        }

        for (i = 0; i < start; i++) {
            fprintf(stderr, " ");
        }
        for (i = 0; i < err.location.len; i++) {
            fprintf(stderr, "^");
        }
        fprintf(stderr, "\n");
    }
    fprintgstrv(stderr, err.message);
}
