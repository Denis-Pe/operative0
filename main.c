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

void free_form(const Form form) {
    if (form.type == FORM_WORD) {
        free_str(form.as_word);
    }
}

typedef enum {
    PARSING_NONE,
    PARSING_WORD,
    PARSING_LONG,
} ParsingState;

typedef struct {
    ParsingState type;
    bool isneg;
} Parsm; // parsing state machine

typedef struct {
    Form* ptr;
    size_t len;
    size_t cap;
} RootForms;

RootForms alloc_root_forms_cap(const size_t cap) {
    Form* ptr = malloc(sizeof(Form) * cap);
    const RootForms forms = {ptr, 0, cap};
    return forms;
}

RootForms alloc_root_forms() {
    return alloc_root_forms_cap(16);
}

void root_forms_push(RootForms *root_forms, const Form form) {
    if (root_forms->len == root_forms->cap) {
        root_forms->ptr = realloc(root_forms->ptr, root_forms->cap * 2);
        root_forms->cap *= 2;
    }
    root_forms->ptr[root_forms->len++] = form;
}

void free_root_forms(const RootForms forms) {
    for (size_t i = 0; i < forms.len; i++) {
        free_form(forms.ptr[i]);
    }
    free(forms.ptr);
}

RootForms parse_forms(const StringView src) {
    Parsm parsm = {PARSING_NONE, false};
    RootForms root_forms = alloc_root_forms();
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
                    fprintf(stderr, "Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
            case PARSING_WORD:
                if (isalnum(c) || c == '-') {
                    str_pushc(&form.as_word, c);
                } else if (isspace(c)) {
                    parsm.type = PARSING_NONE;
                    root_forms_push(&root_forms, form);
                } else {
                    fprintf(stderr, "Unsupported character: '%c'\nExiting now...\n", c);
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
                    root_forms_push(&root_forms, form);
                } else {
                    fprintf(stderr, "Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
        }
    }

    if (parsm.type == PARSING_LONG && parsm.isneg) {
        form.as_long *= -1;
    }
    if (parsm.type != PARSING_NONE) {
        root_forms_push(&root_forms, form);
    }

    return root_forms;
}

int main(void) {
    StringView sample_source = strv_fromtstr("1 2 3 4 5 6 7 8 9 10");

    RootForms forms = parse_forms(sample_source);
    // printf("%i %.*s\n", f.type, f.as_word.len, f.as_word.ptr);
    for (size_t i = 0; i < forms.len; i++) {
        const Form f = forms.ptr[i];
        printf("%i %li\n", f.type, f.as_long);
    }

    return 0;
}
