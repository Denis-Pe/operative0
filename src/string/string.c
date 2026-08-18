//
// Created by denis on 7/23/26.
//

#include "string/string.h"

#include <assert.h>
#include <limits.h>
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
    assert(*string != NULL);
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
    assert(*string != NULL);
    assert(cstr != NULL);

    char *ptr;
    uint8_t c;
    for (ptr = (char *) cstr, c = *ptr; c != '\0'; ++ptr, c = *ptr) {
        assert(c <= 127);
        str_push(string, c);
    }
}

// TODO optimize by directly allocating enough room + memcpy rather than pushing 1 by 1
void str_pushstr(String **string, const String *new_data) {
    assert(string != NULL);
    assert(*string != NULL);
    assert(new_data != NULL);

    for (size_t i = 0; i < new_data->len; i++) {
        const uint8_t c = new_data->ptr[i];
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

int str_comprcstr(const String *str, const char *cstr) {
    assert(str != NULL);
    assert(cstr != NULL);

    size_t i = 0;
    for (; i < str->len && cstr[i] != '\0'; i++) {
        assert(str->ptr[i] <= 127);
        assert(cstr[i] <= 127);
        const int diff = str->ptr[i] - cstr[i];
        if (diff) {
            return diff;
        }
    }

    // length differences
    if (i == str->len && cstr[i] != '\0') {
        return cstr[i];
    } else if (i < str->len && cstr[i] == '\0') {
        return str->ptr[i];
    } else {
        return 0;
    }
}

void fprintstr(FILE *stream, const String *str) {
    assert(str->len <= INT_MAX);
    fprintf(stream, "%.*s", str->len, (char *) str->ptr);
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
        .impl_ptr = (uint8_t *) cstr,
        .impl_len = len
    };
    return strv;
}

StringView strv_fromstr(const String *str) {
    assert(str != NULL);
    return (StringView){
        .impl_ptr = str->ptr,
        .impl_len = str->len
    };
}

size_t strv_len(const StringView strv) {
    return strv.impl_len;
}

uint32_t strv_char_at(const StringView strv, const size_t index) {
    if (index >= strv.impl_len) {
        panicf("String access error: length %zu index %zu", strv.impl_len, index);
    }

    return strv.impl_ptr[index];
}
