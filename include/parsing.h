//
// Created by denis on 8/27/26.
//

#ifndef OPERATIVE_PARSING_H
#define OPERATIVE_PARSING_H

#include <stddef.h>

#include "runtime/value.h"
#include "seq.h"
#include "string/string.h"

#define ASCII_0 48
#define ASTPRINT_INDENT_WIDTH 4

enum TokenType {
    TOKEN_QUOTE,
    TOKEN_WORD,
    TOKEN_BRACKET,
    TOKEN_WHITESPACE,
    TOKEN_NUMBER
};

typedef struct {
    size_t depth;
    char bracket;
} BracketToken;

typedef struct {
    enum TokenType type;
    size_t src_idx;
    size_t len;

    union {
        char as_quote;
        StringView as_word;
        BracketToken as_bracket;
        StringView as_whitespace;
        StringView as_number;
    };
} Token;

DECLARE_SEQ(Tokens, tokens, Token)

typedef struct {
    Token *ptr;
    size_t len;
} TokensSlice;

typedef struct {
    bool isok;
    Tokens res;
    Error err;
} TokensResult;

TokensResult tokenize(StringView src);

BlockResult parse_tokens(TokensSlice tokens, StringView source);

void free_token(Token token);

void free_tokens(Tokens tokens);

#endif //OPERATIVE_PARSING_H
