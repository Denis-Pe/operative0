#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "string/string.h"
#include "string/pred.h"
#include "seq.h"
#include "error.h"

#define ASCII_0 48

enum TokenType {
    TOKEN_WORD,
    TOKEN_BRACKET,
    TOKEN_DASH,
    TOKEN_WHITESPACE,
    TOKEN_INTEGER
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
        String *as_word;
        BracketToken as_bracket;
        char as_dash;
        String *as_whitespace;
        long as_integer;
    };
} Token;

void free_token(const Token token) {
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

DECLARE_SEQ(Tokens, tokens, Token)

DEFINE_SEQ(Tokens, tokens, Token)

void free_tokens(const Tokens tokens) {
    for (size_t i = 0; i < tokens.len; i++) {
        free_token(tokens.ptr[i]);
    }
    free(tokens.ptr);
}

bool is_valid_word_tok_first(const uint32_t c) {
    return c_isletter(c)
           || c == '_';
}

bool is_valid_word_tok_rest(const uint32_t c) {
    return is_valid_word_tok_first(c) || c_isdigit(c);
}

void emit_token(Tokens *tokens, const Token *tok, bool *has_token) {
    if (*has_token) {
        tokens_push(tokens, tok);
        *has_token = false;
    }
}

Tokens tokenize(const StringView src) {
    Tokens tokens = alloc_tokens();

    const size_t src_len = strv_len(src);
    if (src_len == 0) {
        return tokens;
    }

    uint32_t c;
    bool has_token = false;
    Token tok = {0};
    size_t depth = 0;
    for (size_t i = 0; i < src_len; i++) {
        c = strv_char_at(src, i);

        if (c == '[') {
            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_BRACKET;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_bracket.bracket = '[';
            tok.as_bracket.depth = ++depth;
            tokens_push(&tokens, &tok);
        } else if (c == ']') {
            if (depth == 0) {
                panicf("Source has unmatched brackets: extra closing bracket: index %zu\n", i);
            }

            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_BRACKET;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_bracket.bracket = ']';
            tok.as_bracket.depth = depth--;
            tokens_push(&tokens, &tok);
        } else if (c_iswhitespace(c)) {
            if (!has_token || tok.type != TOKEN_WHITESPACE) {
                emit_token(&tokens, &tok, &has_token);

                tok = (Token){0};
                tok.type = TOKEN_WHITESPACE;
                tok.src_idx = i;
                tok.as_whitespace = alloc_str();
                has_token = true;
            }

            str_push(&tok.as_whitespace, c);
            tok.len++;
        } else if (c == '-') {
            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_DASH;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_dash = '-';
            tokens_push(&tokens, &tok);
        } else if (has_token && tok.type == TOKEN_WORD && is_valid_word_tok_rest(c)) {
            str_push(&tok.as_word, c);
            tok.len++;
        } else if (is_valid_word_tok_first(c)) {
            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_WORD;
            tok.src_idx = i;
            tok.as_word = alloc_str();
            str_push(&tok.as_word, c);
            tok.len = 1;
            has_token = true;
        } else if (c_isdigit(c)) {
            if (has_token && tok.type == TOKEN_INTEGER) {
                tok.as_integer *= 10;
                tok.as_integer += c - ASCII_0;
                tok.len++;
            } else {
                emit_token(&tokens, &tok, &has_token);

                tok = (Token){0};
                tok.type = TOKEN_INTEGER;
                tok.src_idx = i;
                tok.as_integer = c - ASCII_0;
                tok.len = 1;
                has_token = true;
            }
        } else {
            panicf("Parsing error: Unexpected character found: Code %u Glyph '%c'\n", c, (char) c);
        }
    }

    if (depth != 0) {
        panicf("Source has unmatched brackets: too many opening brackets\n");
    }

    emit_token(&tokens, &tok, &has_token);

    return tokens;
}

enum ASTNodeType {
    AST_WORD,
    AST_INTEGER,
    AST_DOUBLE,
    AST_BLOCK
};

typedef struct ASTNode ASTNode;

DECLARE_SEQ(Block, block, ASTNode)

struct ASTNode {
    size_t src_idx;
    size_t src_len;
    enum ASTNodeType type;

    union {
        String *as_word;
        long as_integer;
        double as_double;
        Block as_block;
    };
};

DEFINE_SEQ(Block, block, ASTNode)

int main(void) {
    const StringView sample_source = strv_fromcstr(
        "         [      1 2 3 4 5 6 7 8 9 10   ]  le-word       ");

    Tokens tokens = tokenize(sample_source);

    free_tokens(tokens);

    return 0;
}
