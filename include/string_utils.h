#ifndef OPERATIVE_STRING_UTILS_H
#define OPERATIVE_STRING_UTILS_H

#define DEFAULT_STRING_CAPACITY 8
#include <stddef.h>
#include "seq.h"
#include "error.h"

DECLARE_SEQ(String, str, char);

void str_pushcstr(String *string, const char *tstr);

String alloc_str_fromcstr(const char *tstr);

String alloc_str_clone(String from);

void free_str(String s);

/// Borrowed string, not null-terminated
typedef struct {
    const char *ptr;
    size_t len;
} StringView;

StringView strv_fromtstr(const char *tstr);

#endif //OPERATIVE_STRING_UTILS_H
