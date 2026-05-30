#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "root_forms.h"
#include "string_utils.h"
#include "forms.h"

#define ASCII_0 48

typedef enum {
    PARSING_NONE,
    PARSING_WORD,
    PARSING_LONG,
} ParsingState;

typedef struct {
    ParsingState type;
    bool isneg;
    size_t src_idx;
    StringView src;
} Parst; // parsing state

RootForms parse_forms(Parst *parst) {
    RootForms root_forms = alloc_root_forms();
    Form form;

    for (size_t i = parst->src_idx; i < parst->src.len; parst->src_idx = ++i) {
        const char c = parst->src.ptr[i];
        switch (parst->type) {
            case PARSING_NONE:
                if (isalpha(c)) {
                    parst->type = PARSING_WORD;
                    form.type = FORM_WORD;
                    form.as_word = alloc_str();
                    str_pushc(&form.as_word, c);
                } else if (c == '-') {
                    parst->type = PARSING_LONG;
                    parst->isneg = true;
                    form.type = FORM_LONG;
                    form.as_long = 0;
                } else if (isdigit(c)) {
                    parst->type = PARSING_LONG;
                    form.type = FORM_LONG;
                    form.as_long = c - ASCII_0;
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
                    parst->type = PARSING_NONE;
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
                    parst->type = PARSING_NONE;
                    if (parst->isneg) {
                        form.as_long *= -1;
                        parst->isneg = false;
                    }
                    root_forms_push(&root_forms, &form);
                } else {
                    fprintf(stderr, "Unsupported character: '%c'\nExiting now...\n", c);
                    exit(-1);
                }
                break;
        }
    }

    if (parst->type == PARSING_LONG && parst->isneg) {
        form.as_long *= -1;
    }
    if (parst->type != PARSING_NONE) {
        root_forms_push(&root_forms, &form);
    }

    return root_forms;
}

int main(void) {
    StringView sample_source = strv_fromtstr("               1 2 3 4 5 6 7 8 9 10           ");
    Parst parst = {
        PARSING_NONE,
        false,
        0,
        sample_source
    };

    RootForms forms = parse_forms(&parst);
    // printf("%i %.*s\n", f.type, f.as_word.len, f.as_word.ptr);
    for (size_t i = 0; i < forms.len; i++) {
        const Form f = forms.ptr[i];
        printf("%i %li\n", f.type, f.as_long);
    }

    return 0;
}
