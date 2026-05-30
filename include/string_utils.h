#ifndef OPERATIVE_STRING_UTILS_H
#define OPERATIVE_STRING_UTILS_H

#define DEFAULT_STRING_CAPACITY 8
#include <stddef.h>

/// Owned string, not null-terminated
typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} String;

/// buffer is set to NULL if capacity is 0
String alloc_str_cap(size_t capacity);

String alloc_str();

void str_pushc(String *s, char c);

void str_pushtstr(String *string, const char *tstr);

String alloc_str_fromtstr(const char *tstr);

String alloc_str_clone(String from);

void free_str(String s);

/// Borrowed string, not null-terminated
typedef struct {
    const char *ptr;
    size_t len;
} StringView;

StringView strv_fromtstr(const char *tstr);

#endif //OPERATIVE_STRING_UTILS_H
