//
// Created by denis on 7/23/26.
//

#ifndef OPERATIVE_STRING_H
#define OPERATIVE_STRING_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct String String;

String *alloc_str_cap(size_t cap);

String *alloc_str(void);

void str_push(String **string, uint32_t character);

void str_pushcstr(String **string, const char *cstr);

void str_pushstr(String **string, const String *new_data);

String *alloc_str_fromcstr(const char *cstr);

String *alloc_str_clone(const String *from);

uint32_t str_char_at(const String *str, size_t index);

size_t str_len(const String *str);

int str_comprcstr(const String *str, const char *cstr);

bool str_contains(const String *str, uint32_t character);

void fprintstr(FILE *stream, const String *str);

void free_str(String *s);

/// Borrowed string, not null-terminated
typedef struct {
    /// Pointer to data. Direct use reserved for string/ files only
    const uint8_t *impl_ptr;
    /// Length of valid bytes in data. Direct use reserved for string/ files only
    size_t impl_len;
} StringView;

StringView strv_fromcstr(const char *cstr);

StringView strv_fromstr(const String *str);

size_t strv_len(StringView strv);

uint32_t strv_char_at(StringView strv, size_t index);

StringView strv_slice(StringView strv, size_t begin, size_t length);

#endif //OPERATIVE_STRING_H
