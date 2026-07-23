//
// Created by denis on 7/23/26.
//

#include "string/string.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"

#define DEFAULT_STRING_SIZE 8

struct String {
    size_t len;
    size_t cap;
    uint8_t ptr[];
};

String *alloc_str_cap(const size_t cap) {
    String *str = malloc(sizeof(*str) + cap);
    if (str == NULL) panic_errno();

    str->len = 0;
    str->cap = cap;
    return str;
}

String *alloc_str(void) {
    return alloc_str_cap(DEFAULT_STRING_SIZE);
}

void str_push(String **string, const uint32_t character) {
    assert(string != NULL);
    assert(character <= 127);

    if ((*string)->len == (*string)->cap) {
        const size_t new_cap = (*string)->cap ? (*string)->cap * 2 : DEFAULT_STRING_SIZE;
        String *new_ptr = realloc(*string, sizeof(String) + new_cap * sizeof(*(*string)->ptr));
        if (new_ptr == NULL) panic_errno();
        *string = new_ptr;
        (*string)->cap = new_cap;
    }
    (*string)->ptr[(*string)->len++] = character;
}

void str_pushcstr(String **string, const char *cstr) {
    assert(string != NULL);
    assert(cstr != NULL);

    char *ptr;
    uint8_t c;
    for (ptr = (char *) cstr, c = *ptr; c != '\0'; ++ptr, c = *ptr) {
        assert(c <= 127);
        str_push(string, c);
    }
}

String *alloc_str_fromcstr(const char *cstr) {
    assert(cstr != NULL);
    String *s = alloc_str();
    str_pushcstr(&s, cstr);
    return s;
}

String *alloc_str_clone(const String *from) {
    assert(from != NULL);
    String *to = alloc_str_cap(from->cap);
    to->len = from->len;
    memcpy(to->ptr, from->ptr, from->len);
    return to;
}

uint32_t str_char_at(const String *str, const size_t index) {
    assert(str != NULL);
    if (index >= str->len) {
        panicf("String access error: length %zu index %zu", str->len, index);
    }

    return str->ptr[index];
}

size_t str_len(const String *str) {
    assert(str != NULL);
    return str->len;
}

void free_str(String *s) {
    free(s);
}

StringView strv_fromcstr(const char *cstr) {
    assert(cstr != NULL);

    size_t len = 0;
    char *ptr;
    uint8_t c;
    for (ptr = (char *) cstr, c = *ptr; c != '\0'; ++ptr, c = *ptr) {
        assert(c <= 127);
        len++;
    }
    const StringView strv = (StringView){
        ._ptr = (uint8_t *) cstr,
        ._len = len
    };
    return strv;
}

StringView strv_fromstr(const String *str) {
    assert(str != NULL);
    return (StringView){
        ._ptr = str->ptr,
        ._len = str->len
    };
}

size_t strv_len(const StringView strv) {
    return strv._len;
}

uint32_t strv_char_at(const StringView strv, const size_t index) {
    if (index >= strv._len) {
        panicf("String access error: capacity %zu index %zu", strv._len, index);
    }

    return strv._ptr[index];
}
