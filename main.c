#include <ctype.h>
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

/// initialized to \0
/// buffer is set to NULL if capacity is 0
String str_make_capacity(const size_t capacity) {
    char *buf = NULL;
    if (capacity > 0) {
        buf = malloc(capacity);
    }
    const String s = {buf, 0, capacity};
    return s;
}

String str_make() {
    return str_make_capacity(DEFAULT_STRING_CAPACITY);
}

void str_pushc(String *s, const char c) {
    if (s->len == s->cap) {
        s->ptr = realloc(s->ptr, s->cap * 2);
    }
    s->ptr[s->len] = c;
    s->len++;
}

void str_pushtstr(String *string, const char *tstr) {
    for (const char *tstrptr = tstr; *tstrptr != '\0'; tstrptr++) {
        str_pushc(string, *tstrptr);
    }
}

String str_fromtstr(const char *tstr) {
    String s = str_make();
    str_pushtstr(&s, tstr);
    return s;
}

String str_clone(const String from) {
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

Form parse_form(const StringView src) {
    ParsingState state = PARSING_NONE;
    Form form;

    for (size_t i = 0; i < src.len; ++i) {
        const char c = src.ptr[i];
        switch (state) {
            case PARSING_NONE:
                if (isalpha(c)) {
                    state = PARSING_WORD;
                    form.type = FORM_WORD;
                    form.as_word = str_make();
                    str_pushc(&form.as_word, c);
                } else if (isdigit(c)) {
                    state = PARSING_LONG;
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
                if (isalnum(c)) {
                    str_pushc(&form.as_word, c);
                } else if (isspace(c)) {
                    state = PARSING_NONE;
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
                    state = PARSING_NONE;
                } else {
                    printf("Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
        }
    }
    return form;
}

int main(void) {
    StringView sample_source = strv_fromtstr("function-");

    printf("Hello, World!\n");

    Form f = parse_form(sample_source);
    printf("%i %.*s\n", f.type, f.as_word.len, f.as_word.ptr);

    return 0;
}
