#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "root_forms.h"
#include "string_utils.h"
#include "forms.h"

typedef enum {
    PARSING_NONE,
    PARSING_WORD,
    PARSING_LONG,
} ParsingState;

typedef struct {
    ParsingState type;
    bool isneg;
} Parsm; // parsing state machine

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
                    root_forms_push(&root_forms, &form);
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
                    root_forms_push(&root_forms, &form);
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
        root_forms_push(&root_forms, &form);
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
