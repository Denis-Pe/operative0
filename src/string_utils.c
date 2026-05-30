#include "string_utils.h"

#include <stdlib.h>
#include <string.h>

String alloc_str_cap(const size_t capacity) {
    char *buf = NULL;
    if (capacity > 0) {
        buf = malloc(capacity);
    }
    const String s = {buf, 0, capacity};
    return s;
}

String alloc_str() {
    return alloc_str_cap(DEFAULT_STRING_CAPACITY);
}

void str_pushc(String *s, const char c) {
    if (s->len == s->cap) {
        s->ptr = realloc(s->ptr, s->cap * 2);
        s->cap *= 2;
    }
    s->ptr[s->len++] = c;
}

void str_pushtstr(String *string, const char *tstr) {
    for (const char *tstrptr = tstr; *tstrptr != '\0'; tstrptr++) {
        str_pushc(string, *tstrptr);
    }
}

String alloc_str_fromtstr(const char *tstr) {
    String s = alloc_str();
    str_pushtstr(&s, tstr);
    return s;
}

String alloc_str_clone(const String from) {
    char *buf = malloc(from.cap);
    memcpy(buf, from.ptr, from.len);
    const String s = {buf, from.len, from.cap};
    return s;
}

StringView strv_fromtstr(const char *tstr) {
    StringView strv = {tstr, 0};
    for (const char *tstrptr = tstr; *tstrptr != '\0'; tstrptr++) {
        strv.len++;
    }
    return strv;
}

void free_str(const String s) {
    free(s.ptr);
}