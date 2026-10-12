#ifndef OPERATIVE_ERROR_H
#define OPERATIVE_ERROR_H
#include <stddef.h>

#include "string/string.h"

typedef struct {
    StringView source;
    size_t src_idx;
    size_t len;
    StringView message;
} SourceError;

void panicf(const char *format, ...);

void panic_errno(void);

void panic(void);

/// Assuming the error is contained in a single line
void report_srcerr(SourceError srce);

#define panic_switch() panicf("Error: switch statement is inexhaustive in %s %d\n", __FILE__, __LINE__)

#endif //OPERATIVE_ERROR_H
