//
// Created by denis on 8/27/26.
//
#include "parsing.h"

DEFINE_SEQ(Tokens, tokens, Token)
DEFINE_SEQ(ASTBlock, astblock, ASTNode)

void free_token(const Token token) {
}

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
    size_t slice_begin = 0;
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
                slice_begin = i;
                has_token = true;
            }

            tok.as_whitespace = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len++;
        } else if (c_isdigit(c)) {
            if (has_token && tok.type == TOKEN_WORD && (strv_comprcstr(tok.as_word, "-") == 0
                                                        || strv_comprcstr(tok.as_word, "-.") == 0
                                                        || strv_comprcstr(tok.as_word, ".") == 0)) {
                // type punning
                tok.type = TOKEN_NUMBER;
            } else if (!has_token || tok.type != TOKEN_NUMBER) {
                emit_token(&tokens, &tok, &has_token);
                tok.type = TOKEN_NUMBER;
                tok.src_idx = i;
                tok.len = 0;
                slice_begin = i;
                has_token = true;
            }
            tok.as_number = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len++;
        } else if (c == '.' && has_token && tok.type == TOKEN_NUMBER && !strv_contains(tok.as_number, '.')) {
            tok.as_number = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len++;
        } else if (has_token && tok.type == TOKEN_WORD && is_valid_word_tok_rest(c)) {
            tok.as_word = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len++;
        } else if (is_valid_word_tok_first(c)) {
            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_WORD;
            tok.src_idx = i;
            slice_begin = i;
            tok.as_word = strv_slice(src, slice_begin, i + 1 - slice_begin);
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

void free_block(const ASTBlock block) {
    for (size_t i = 0; i < block.len; i++) {
        free_node(block.ptr[i]);
    }
    free(block.ptr);
}

void free_node(const ASTNode node) {
    switch (node.type) {
        case AST_BLOCK:
            free_block(node.as_block);
            break;
        default: ;
    }
}

void emit_node(ASTBlock *root, const ASTNode *node, bool *has_node) {
    if (*has_node) {
        *has_node = false;
        astblock_push(root, node);
    }
}

ASTNode parse_number(const Token token) {
    ASTNode result;
    result.src_idx = token.src_idx;
    result.src_len = token.len;
    size_t i = 0;
    uint32_t c = strv_char_at(token.as_number, i);
    const bool isneg = c == '-';
    if (isneg) c = strv_char_at(token.as_number, ++i);
    const size_t len = strv_len(token.as_number);
    if (strv_contains(token.as_number, '.')) {
        result.type = AST_DOUBLE;
        double big = 0.0;
        double small = 0.0;
        while (c != '.') {
            big *= 10.0;
            if (isneg)
                big -= ((double) (c - ASCII_0));
            else
                big += ((double) (c - ASCII_0));
            c = strv_char_at(token.as_number, ++i);
        }
        i++; // skip '.'
        size_t e = 0;
        for (; i < len; i++, e++) {
            c = strv_char_at(token.as_number, i);
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
            c = strv_char_at(token.as_number, i);

            if (__builtin_mul_overflow(a, 10, &a)) {
                fprintf(stderr, "Panic: Parsing error: number ");
                fprintstrv(stderr, token.as_number);
                fprintf(stderr, " is too large to fit in 64 bits.\n");
                panicf("");
            }
            if (__builtin_add_overflow(a, (isneg ? (-1 * (int64_t) (c - ASCII_0)) : (int64_t) (c - ASCII_0)), &a)) {
                fprintf(stderr, "Panic: Parsing error: number ");
                fprintstrv(stderr, token.as_number);
                fprintf(stderr, " is too large to fit in 64 bits.\n");
                panicf("");
            }
        }
        result.as_integer = a;
    }

    return result;
}

ASTBlock parse_tokens(const TokensSlice tokens, size_t *i) {
    ASTBlock root = alloc_astblock();

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
                emit_node(&root, &node, &has_node);
                node.type = AST_WORD;
                node.src_idx = tok.src_idx;
                node.src_len = tok.len;
                node.as_word = tok.as_word;
                has_node = true;
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
            fprintstrv(stdout, node.as_word);
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

void printast(const ASTBlock root, const size_t indent) {
    for (size_t i = 0; i < root.len; i++) {
        const ASTNode node = root.ptr[i];

        for (size_t j = 0; j < indent * ASTPRINT_INDENT_WIDTH; j++) printf(" ");

        if (node.type == AST_BLOCK) {
            printf("Block: [\n");
            printast(node.as_block, indent + 1);
            for (size_t j = 0; j < indent * ASTPRINT_INDENT_WIDTH; j++) printf(" ");
            printf("]");
        } else {
            printatom(node);
        }
        printf("\n");

        fflush(stdout);
    }
}