#include "string_utils.h"

#include <stdlib.h>
#include <string.h>
#include <seq.h>

void str_pushcstr(String *string, const char *tstr) {
    for (const char *tstrptr = tstr; *tstrptr != '\0'; tstrptr++) {
        str_push(string, tstrptr);
    }
}

String alloc_str_fromcstr(const char *tstr) {
    String s = alloc_str();
    str_pushcstr(&s, tstr);
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