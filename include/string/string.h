//
// Created by denis on 7/23/26.
//

#ifndef OPERATIVE_STRING_H
#define OPERATIVE_STRING_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MAX_STRCHAIN_SIZE 10

enum GenericSTRVType {
    GENERICSTRV_VIEW,
    GENERICSTRV_CHAIN,
};

typedef struct String String;

/// Borrowed string, not null-terminated
typedef struct {
    /// Pointer to data. Direct use reserved for string/ files only
    const uint8_t *impl_ptr;
    /// Length of valid bytes in data. Direct use reserved for string/ files only
    size_t impl_len;
} StringView;

typedef struct {
    StringView parts[MAX_STRCHAIN_SIZE];
} StringVChain;

typedef struct {
    enum GenericSTRVType type;

    union {
        StringView as_view;
        StringVChain as_chain;
    };
} GenericStringView;

String *alloc_str_cap(size_t cap);

String *alloc_str(void);

String *alloc_str_fromcstr(const char *cstr);

String *alloc_str_clone(const String *from);

StringView strv_fromcstr(const char *cstr);

StringView strv_fromstr(const String *str);

void str_accomodate(String **string, size_t ideal_cap);

/// Does not shrink the allocated memory
void str_popall(String *string);

void str_push(String **string, uint32_t character);

void str_pushcstr(String **string, const char *cstr);

void str_pushstr(String **string, const String *new_data);

/// Starting from the current stream position, up to bytes or until EOF. Returns false on ferror
bool str_pushfile(String **string, FILE *, size_t bytes);

uint32_t str_char_at(const String *str, size_t index);

uint32_t strv_char_at(StringView strv, size_t index);

size_t str_len(const String *str);

size_t strv_len(StringView strv);

int str_cmp(const String *a, const String *b);

int str_cmpcstr(const String *str, const char *cstr);

int strv_cmpcstr(StringView strv, const char *cstr);

int strv_cmp(StringView a, StringView b);

bool str_contains(const String *str, uint32_t character);

bool strv_contains(StringView strv, uint32_t character);

StringView strv_slice(StringView strv, size_t begin, size_t length);

void fprintstr(FILE *stream, const String *str);

void fprintstrv(FILE *stream, StringView strv);

void fprintstrvc(FILE *stream, StringVChain strvc);

void fprintgstrv(FILE *stream, GenericStringView gstrv);

void free_str(String *s);

#endif //OPERATIVE_STRING_H
