#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "string_utils.h"

#define ASCII_0 48

enum OpTokenType {
    TOKEN_WORD,
    TOKEN_OPEN_BRACKET,
    TOKEN_CLOSE_BRACKET,
    TOKEN_WHITESPACE,
    TOKEN_INTEGER
};

typedef struct {
    enum OpTokenType type;
    size_t src_idx;

    union {
        String as_word;
        char as_open_bracket;
        char as_close_bracket;
        String as_whitespace;
        long as_integer;
    };
} OpToken;

void free_token(const OpToken token) {
    switch (token.type) {
        case TOKEN_WORD:
            free_str(token.as_word);
            break;
        case TOKEN_WHITESPACE:
            free_str(token.as_whitespace);
            break;
        default: ;
    };
}

typedef struct {
    OpToken *ptr;
    size_t len;
    size_t cap;
} OpTokensArr;

OpTokensArr alloc_tokens_cap(const size_t cap) {
    OpToken *ptr = malloc(sizeof(OpToken) * cap);
    const OpTokensArr tokens = {ptr, 0, cap};
    return tokens;
}

OpTokensArr alloc_tokens() {
    return alloc_tokens_cap(16);
}

void tokens_push(OpTokensArr *root_forms, const OpToken *token) {
    if (root_forms->len == root_forms->cap) {
        root_forms->ptr = realloc(root_forms->ptr, root_forms->cap * 2);
        root_forms->cap *= 2;
    }
    root_forms->ptr[root_forms->len++] = *token;
}

void free_tokens(const OpTokensArr forms) {
    for (size_t i = 0; i < forms.len; i++) {
        free_token(forms.ptr[i]);
    }
    free(forms.ptr);
}

OpTokensArr op_tokenize(const StringView src) {
    OpTokensArr tokens = alloc_tokens();

    if (src.len == 0) {
        return tokens;
    }

    OpToken tok;
    char c = src.ptr[0];
    bool turn_negative = false;
    if (c == '-') {
        turn_negative = true;
        tok.type = TOKEN_INTEGER;
        tok.as_integer = 0;
    } else if (isdigit(c)) {
        tok.type = TOKEN_INTEGER;
        tok.as_integer = c - ASCII_0;
    } else if (isalpha(c)) {
        tok.type = TOKEN_WORD;
        tok.as_word = alloc_str();
        str_pushc(&tok.as_word, c);
    } else if (isspace(c)) {
        tok.type = TOKEN_WHITESPACE;
        tok.as_whitespace = alloc_str();
        str_pushc(&tok.as_whitespace, c);
    } else if (c == '[') {
        tok.type = TOKEN_OPEN_BRACKET;
        tok.as_open_bracket = '[';
        tokens_push(&tokens, &tok);
    } else if (c == ']') {
        tok.type = TOKEN_CLOSE_BRACKET;
        tok.as_close_bracket = ']';
        tokens_push(&tokens, &tok);
    } else {
        goto unexpected_char;
    }

    for (size_t i = 1; i < src.len; i++) {
        c = src.ptr[i];
    }

    return tokens;

unexpected_char:
    printf("Parsing error: Unexpected character found: %c\n", c);
    exit(EXIT_FAILURE);
}

int main(void) {
    const StringView sample_source = strv_fromtstr(
        "               1 2 3 4 5 6 7 8 9 10           ");

    OpTokensArr tokens = op_tokenize(sample_source);


    return 0;
}
