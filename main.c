#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_STRING_CAPACITY 8

/// Owned string, not null-terminated
typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} String;

/// buffer is set to NULL if capacity is 0
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

void free_str(const String s) {
    free(s.ptr);
}

/// Borrowed string, not null-terminated
typedef struct {
    const char *ptr;
    size_t len;
} StringView;

StringView strv_fromtstr(const char *tstr) {
    StringView strv = {tstr, 0};
    for (const char *tstrptr = tstr; *tstrptr != '\0'; tstrptr++) {
        strv.len++;
    }
    return strv;
}

typedef enum {
    FORM_WORD,
    FORM_LONG,
} FormType;

typedef struct {
    union {
        String as_word;
        long as_long;
    };

    FormType type;
} Form;

typedef enum {
    PARSING_NONE,
    PARSING_WORD,
    PARSING_LONG,
} ParsingState;

typedef struct {
    ParsingState type;
    bool isneg;
} Parsm; // parsing state machine

Form parse_form(const StringView src) {
    Parsm parsm = {PARSING_NONE, false};
    Form form;

    for (size_t i = 0; i < src.len; ++i) {
        const char c = src.ptr[i];
        switch (parsm.type) {
            case PARSING_NONE:
                if (isalpha(c)) {
                    parsm.type = PARSING_WORD;
                    form.type = FORM_WORD;
                    form.as_word = alloc_str();
                    str_pushc(&form.as_word, c);
                } else if (c == '-') {
                    parsm.type = PARSING_LONG;
                    parsm.isneg = true;
                    form.type = FORM_LONG;
                    form.as_long = 0;
                } else if (isdigit(c)) {
                    parsm.type = PARSING_LONG;
                    form.type = FORM_LONG;
                    form.as_long = c - 48;
                } else if (isspace(c)) {
                    continue;
                } else {
                    printf("Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
            case PARSING_WORD:
                if (isalnum(c) || c == '-') {
                    str_pushc(&form.as_word, c);
                } else if (isspace(c)) {
                    parsm.type = PARSING_NONE;
                } else {
                    printf("Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
            case PARSING_LONG:
                if (isdigit(c)) {
                    form.as_long *= 10;
                    form.as_long += c - 48;
                } else if (isspace(c)) {
                    parsm.type = PARSING_NONE;
                    if (parsm.isneg) {
                        form.as_long *= -1;
                        parsm.isneg = false;
                    }
                } else {
                    printf("Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
        }
    }

    if (parsm.type == PARSING_LONG && parsm.isneg) {
        form.as_long *= -1;
    }

    return form;
}

int main(void) {
    StringView sample_source = strv_fromtstr("hello-world");

    printf("Hello, World!\n");

    Form f = parse_form(sample_source);
    printf("%i %.*s\n", f.type, f.as_word.len, f.as_word.ptr);
    // printf("%i %li", f.type, f.as_long);

    return 0;
}
