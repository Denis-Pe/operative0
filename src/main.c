#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "string/string.h"
#include "string/pred.h"
#include "seq.h"
#include "error.h"

#define ASCII_0 48
#define INDENT_WIDTH 4

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
        String *as_word;
        BracketToken as_bracket;
        String *as_whitespace;
        String *as_number;
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
        case TOKEN_NUMBER:
            free_str(token.as_number);
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
           || c == '_' || c == '-' || c == '.';
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
        } else if (c_isdigit(c)) {
            if (has_token && tok.type == TOKEN_WORD && (str_comprcstr(tok.as_word, "-") == 0
                                                        || str_comprcstr(tok.as_word, "-.") == 0
                                                        || str_comprcstr(tok.as_word, ".") == 0)) {
                // type punning
                tok.type = TOKEN_NUMBER;
            } else if (!has_token || tok.type != TOKEN_NUMBER) {
                emit_token(&tokens, &tok, &has_token);
                tok.type = TOKEN_NUMBER;
                tok.src_idx = i;
                tok.len = 0;
                tok.as_number = alloc_str();
                has_token = true;
            }
            str_push(&tok.as_number, c);
            tok.len++;
        } else if (c == '.' && has_token && tok.type == TOKEN_NUMBER && !str_contains(tok.as_number, '.')) {
            str_push(&tok.as_number, c);
            tok.len++;
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
        int64_t as_integer;
        double as_double;
        Block as_block;
    };
};

void free_block(Block block);

void free_node(const ASTNode node) {
    switch (node.type) {
        case AST_BLOCK:
            free_block(node.as_block);
            break;
        case AST_WORD:
            free_str(node.as_word);
            break;
        default: ;
    }
}

DEFINE_SEQ(Block, block, ASTNode)

void free_block(const Block block) {
    for (size_t i = 0; i < block.len; i++) {
        free_node(block.ptr[i]);
    }
    free(block.ptr);
}

typedef struct {
    Token *ptr;
    size_t len;
} TokensSlice;

void emit_node(Block *root, const ASTNode *node, bool *has_node) {
    if (*has_node) {
        *has_node = false;
        block_push(root, node);
    }
}

ASTNode parse_number(const Token token) {
    ASTNode result;
    result.src_idx = token.src_idx;
    result.src_len = token.len;
    size_t i = 0;
    uint32_t c = str_char_at(token.as_number, i);
    const bool isneg = c == '-';
    if (isneg) c = str_char_at(token.as_number, ++i);
    const size_t len = str_len(token.as_number);
    if (str_contains(token.as_number, '.')) {
        result.type = AST_DOUBLE;
        double big = 0.0;
        double small = 0.0;
        while (c != '.') {
            big *= 10.0;
            if (isneg)
                big -= ((double) (c - ASCII_0));
            else
                big += ((double) (c - ASCII_0));
            c = str_char_at(token.as_number, ++i);
        }
        i++; // skip '.'
        size_t e = 0;
        for (; i < len; i++, e++) {
            c = str_char_at(token.as_number, i);
            small *= 10.0;
            if (isneg)
                small -= ((double) (c - ASCII_0));
            else
                small += ((double) (c - ASCII_0));
        }
        for (size_t j = 0; j < e; j++) small /= 10.0;
        result.as_double = big + small;
    } else {
        result.type = AST_INTEGER;
        int64_t a = 0;
        for (; i < len; i++) {
            c = str_char_at(token.as_number, i);

            if (__builtin_mul_overflow(a, 10, &a)) {
                fprintf(stderr, "Panic: Parsing error: number ");
                fprintstr(stderr, token.as_number);
                fprintf(stderr, " is too large to fit in 64 bits.\n");
                panicf("");
            }
            if (__builtin_add_overflow(a, (isneg ? (-1 * (int64_t)(c - ASCII_0)) : (int64_t)(c - ASCII_0)), &a)) {
                fprintf(stderr, "Panic: Parsing error: number ");
                fprintstr(stderr, token.as_number);
                fprintf(stderr, " is too large to fit in 64 bits.\n");
                panicf("");
            }
        }
        result.as_integer = a;
    }

    return result;
}

Block parse_tokens(const TokensSlice tokens, size_t *i) {
    Block root = alloc_block();

    ASTNode node;
    bool has_node = false;
    for (; *i < tokens.len; (*i)++) {
        const Token tok = tokens.ptr[*i];

        switch (tok.type) {
            case TOKEN_WHITESPACE:
                emit_node(&root, &node, &has_node);
                break;
            case TOKEN_BRACKET:
                emit_node(&root, &node, &has_node);
                if (tok.as_bracket.bracket == '[') {
                    node.type = AST_BLOCK;
                    (*i)++;
                    node.as_block = parse_tokens(tokens, i);
                    node.src_idx = tok.src_idx;
                    const Token closing_bracket = tokens.ptr[(*i) - 1];
                    node.src_len = closing_bracket.src_idx - node.src_idx + 2;
                    has_node = true;
                } else {
                    (*i)++;
                    return root;
                }
                break;
            case TOKEN_NUMBER:
                emit_node(&root, &node, &has_node);
                node = parse_number(tok);
                has_node = true;
                break;
            case TOKEN_WORD:
                if (has_node && node.type == AST_WORD) {
                    str_pushstr(&node.as_word, tok.as_word);
                    node.src_len += tok.len;
                } else {
                    emit_node(&root, &node, &has_node);
                    node.type = AST_WORD;
                    node.src_idx = tok.src_idx;
                    node.src_len = tok.len;
                    node.as_word = alloc_str_clone(tok.as_word);
                    has_node = true;
                }
                break;
            default:
                panic_switch();
        }
    }

    emit_node(&root, &node, &has_node);

    return root;
}

void printatom(const ASTNode node) {
    switch (node.type) {
        case AST_WORD:
            printf("Word: ");
            fprintstr(stdout, node.as_word);
            break;
        case AST_INTEGER:
            printf("Integer: %ld", node.as_integer);
            break;
        case AST_DOUBLE:
            printf("Double: %lf", node.as_double);
            break;
        default: ;
    }
}

void printast(const Block root, const size_t indent) {
    for (size_t i = 0; i < root.len; i++) {
        const ASTNode node = root.ptr[i];

        for (size_t j = 0; j < indent * INDENT_WIDTH; j++) printf(" ");

        if (node.type == AST_BLOCK) {
            printf("Block: [\n");
            printast(node.as_block, indent + 1);
            for (size_t j = 0; j < indent * INDENT_WIDTH; j++) printf(" ");
            printf("]");
        } else {
            printatom(node);
        }
        printf("\n");

        fflush(stdout);
    }
}

int main(void) {
    const StringView sample_source = strv_fromcstr(
        "              -1 2 3 -4009000000000000 5 6 [ -.7 8 -9 123456.7890123456 [ -0000001.000001 ]   le.wo-rd  -00000327156028 --- - 2 ]     ");

    Tokens tokens = tokenize(sample_source);

    size_t parsing_index = 0;
    const Block root = parse_tokens((TokensSlice){tokens.ptr, tokens.len}, &parsing_index);

    printast(root, 0);


    free_tokens(tokens);
    free_block(root);

    return 0;
}
