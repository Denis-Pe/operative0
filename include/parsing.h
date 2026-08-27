//
// Created by denis on 8/27/26.
//

#ifndef OPERATIVE_PARSING_H
#define OPERATIVE_PARSING_H

#include <stddef.h>

#include "string/string.h"
#include "seq.h"
#include "string/pred.h"

#define ASCII_0 48
#define ASTPRINT_INDENT_WIDTH 4

enum TokenType {
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
        StringView as_word;
        BracketToken as_bracket;
        StringView as_whitespace;
        StringView as_number;
    };
} Token;

void free_token(Token token);

DECLARE_SEQ(Tokens, tokens, Token)

void free_tokens(Tokens tokens);

bool is_valid_word_tok_first(uint32_t c);

bool is_valid_word_tok_rest(uint32_t c);

void emit_token(Tokens *tokens, const Token *tok, bool *has_token);

Tokens tokenize(StringView src);

enum ASTNodeType {
    AST_WORD,
    AST_INTEGER,
    AST_DOUBLE,
    AST_BLOCK
};

typedef struct ASTNode ASTNode;

DECLARE_SEQ(ASTBlock, block, ASTNode)

struct ASTNode {
    size_t src_idx;
    size_t src_len;
    enum ASTNodeType type;

    union {
        StringView as_word;
        int64_t as_integer;
        double as_double;
        ASTBlock as_block;
    };
};

void free_block(ASTBlock block);

void free_node(ASTNode node);

typedef struct {
    Token *ptr;
    size_t len;
} TokensSlice;

void emit_node(ASTBlock *root, const ASTNode *node, bool *has_node);

ASTNode parse_number(Token token);

ASTBlock parse_tokens(TokensSlice tokens, size_t *i);

void printatom(ASTNode node);

void printast(ASTBlock root, size_t indent);

#endif //OPERATIVE_PARSING_H
