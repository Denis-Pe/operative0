//
// Created by denis on 8/27/26.
//
#include "parsing.h"
#include "miscutil.h"

DEFINE_SEQ(Tokens, tokens, Token)
DEFINE_SEQ(ASTBlockSeq, astblock, ASTNode)

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

TokensResult tokenize(const StringView src) {
    Tokens tokens = alloc_tokens();

    const size_t src_len = strv_len(src);
    if (src_len == 0) {
        return (TokensResult){.isok = true, .res = tokens};
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
                free_tokens(tokens);
                StringView errmsg = strv_fromcstr("Source has unmatched brackets: extra closing bracket");
                return (TokensResult){
                    .isok = false,
                    .err = (Error){
                        .location = (ErrorLocation){.source = src, .src_idx = i, .len = 1},
                        .message = (GenericStringView){
                            .type = GENERICSTRV_VIEW,
                            .as_view = errmsg
                        }
                    }
                };
            }

            emit_token(&tokens, &tok, &has_token);

            tok = (Token){0};
            tok.type = TOKEN_BRACKET;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_bracket.bracket = ']';
            tok.as_bracket.depth = depth--;
            tokens_push(&tokens, &tok);
        } else if (c == '\'') {
            if (i + 1 == src_len) {
                free_tokens(tokens);
                return (TokensResult){
                    .isok = false,
                    .err = (Error){
                        (ErrorLocation){
                            .source = src,
                            .src_idx = i,
                            .len = 1,
                        },
                        .message = (GenericStringView){
                            .type = GENERICSTRV_VIEW, .as_view = strv_fromcstr("Syntax error: standalone quote")
                        }
                    }
                };
            }
            if (!is_valid_word_tok_first(strv_char_at(src, i + 1))) {
                free_tokens(tokens);
                return (TokensResult){
                    .isok = false,
                    .err = (Error){
                        (ErrorLocation){
                            .source = src,
                            .src_idx = i,
                            .len = 2,
                        },
                        .message = (GenericStringView){
                            .type = GENERICSTRV_VIEW,
                            .as_view = strv_fromcstr("Syntax error: quote must be followed by word")
                        }
                    }
                };
            }

            emit_token(&tokens, &tok, &has_token);

            tok.type = TOKEN_QUOTE;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_quote = '\'';
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
            if (has_token && tok.type == TOKEN_WORD && (strv_cmpcstr(tok.as_word, "-") == 0
                                                        || strv_cmpcstr(tok.as_word, "-.") == 0
                                                        || strv_cmpcstr(tok.as_word, ".") == 0)) {
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
            StringVChain chain = {0};
            chain.parts[0] = strv_fromcstr("Parsing error: Unexpected character found: ");
            chain.parts[1] = strv_slice(src, i, 1);
            free_tokens(tokens);
            return (TokensResult){
                .isok = false,
                .err = (Error){
                    .location = (ErrorLocation){.source = src, .src_idx = i, .len = 1},
                    .message = (GenericStringView){
                        .type = GENERICSTRV_CHAIN,
                        .as_chain = chain
                    }
                }
            };
        }
    }

    if (depth != 0) {
        free_tokens(tokens);
        StringView errmsg = strv_fromcstr("Source has unmatched brackets: too many opening brackets");
        return (TokensResult){
            .isok = false,
            .err = (Error){
                .location = {0},
                .message = (GenericStringView){
                    .type = GENERICSTRV_VIEW,
                    .as_view = errmsg
                }
            }
        };
    }

    emit_token(&tokens, &tok, &has_token);

    return (TokensResult){.isok = true, .res = tokens};
}

void free_block(const ASTBlock block) {
    for (size_t i = 0; i < block.seq.len; i++) {
        free_node(block.seq.ptr[i]);
    }
    free(block.seq.ptr);
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
        astblock_push(&root->seq, node);
    }
}

ASTNodeResult parse_number(const Token token, const StringView source) {
    ASTNode result;
    StringVChain chain = {0};
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
        long long a = 0;
        for (; i < len; i++) {
            c = strv_char_at(token.as_number, i);
            const long long digit = (isneg ? (-1 * (long long) (c - ASCII_0)) : (long long) (c - ASCII_0));

            if (mul_overflow_ll(a, 10, &a))
                goto overflow;
            if (add_overflow_ll(a, digit, &a))
                goto overflow;
        }
        result.as_integer = a;
    }

    return (ASTNodeResult){.isok = true, .res = result};

overflow:
    chain.parts[0] = strv_fromcstr("Parsing error: number ");
    chain.parts[1] = token.as_number;
    chain.parts[2] = strv_fromcstr(" is too large to fit in a long long.");
    return (ASTNodeResult){
        .isok = false,
        .err = (Error){
            .location = (ErrorLocation){
                .source = source,
                .src_idx = token.src_idx,
                .len = token.len
            },
            .message = (GenericStringView){.type = GENERICSTRV_CHAIN, .as_chain = chain}
        }
    };
}

ASTBlockResult parse_tokens_helper(const TokensSlice tokens, size_t *i, const size_t depth, const StringView source) {
    ASTBlock root = (ASTBlock){alloc_astblock(), depth};

    ASTBlockResult bres;
    ASTNodeResult nres;
    ASTNode node;
    bool quoted = false;
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
                    bres = parse_tokens_helper(tokens, i, depth + 1, source);
                    if (!bres.isok) {
                        free_block(root);
                        return bres;
                    }
                    node.as_block = bres.res;
                    node.src_idx = tok.src_idx;
                    const Token closing_bracket = tokens.ptr[(*i) - 1];
                    node.src_len = closing_bracket.src_idx - node.src_idx + 2;
                    has_node = true;
                } else {
                    (*i)++;
                    return (ASTBlockResult){.isok = true, .res = root};
                }
                break;
            case TOKEN_NUMBER:
                emit_node(&root, &node, &has_node);
                nres = parse_number(tok, source);
                if (!nres.isok) {
                    free_block(root);
                    return (ASTBlockResult){.isok = false, .err = nres.err};
                }
                node = nres.res;
                has_node = true;
                break;
            case TOKEN_WORD:
                emit_node(&root, &node, &has_node);
                node.type = AST_WORD;
                node.src_idx = tok.src_idx;
                node.src_len = tok.len;
                node.as_word = (ASTWord){.content = tok.as_word, .is_quoted = quoted};
                quoted = false;
                has_node = true;
                break;
            case TOKEN_QUOTE:
                quoted = true;
                break;
            default:
                exhaustive_switch();
        }
    }

    emit_node(&root, &node, &has_node);

    return (ASTBlockResult){.isok = true, .res = root};
}

ASTBlockResult parse_tokens(const TokensSlice tokens, const StringView source) {
    size_t i = 0;
    return parse_tokens_helper(tokens, &i, 0, source);
}

void fprintatom(FILE *stream, const ASTNode node) {
    switch (node.type) {
        case AST_WORD:
            fprintf(stream, "Word: ");
            fprintstrv(stream, node.as_word.content);
            break;
        case AST_INTEGER:
            fprintf(stream, "Integer: %ld", node.as_integer);
            break;
        case AST_DOUBLE:
            fprintf(stream, "Double: %lf", node.as_double);
            break;
        default: ;
    }
}

void fprintast(FILE *stream, const ASTBlock root, const size_t indent) {
    for (size_t i = 0; i < root.seq.len; i++) {
        const ASTNode node = root.seq.ptr[i];

        for (size_t j = 0; j < indent * ASTPRINT_INDENT_WIDTH; j++) fprintf(stream, " ");

        if (node.type == AST_BLOCK) {
            fprintf(stream, "Block: [\n");
            fprintast(stream, node.as_block, indent + 1);
            for (size_t j = 0; j < indent * ASTPRINT_INDENT_WIDTH; j++) fprintf(stream, " ");
            fprintf(stream, "]");
        } else {
            fprintatom(stream, node);
        }
        fprintf(stream, "\n");

        fflush(stream);
    }
}
