#include "parsing.h"

#include <string.h>

#include "miscutil.h"
#include "string/pred.h"

DEFINE_SEQ(Tokens, tokens, Token)

DECLARE_SEQ(ValueScratch, vscratch, Value)
DEFINE_SEQ(ValueScratch, vscratch, Value)

DECLARE_SEQ(Sizes, sizes, size_t)
DEFINE_SEQ(Sizes, sizes, size_t)

void free_vscratch(ValueScratch scratch) {
    for (size_t i = 0; i < scratch.len; i++) {
        free_value(scratch.ptr[i]);
    }
    free(scratch.ptr);
}

void free_token(const Token token) {
}

void free_tokens(const Tokens tokens) {
    for (size_t i = 0; i < tokens.len; i++) {
        free_token(tokens.ptr[i]);
    }
    free(tokens.ptr);
}

bool is_valid_word_tok_first(const uint32_t c) {
    return c_isletter(c) || c == '_' || c == '-' || c == '.';
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
        return (TokensResult) {.isok = true, .res = tokens};
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

            tok = (Token) {0};
            tok.type = TOKEN_BRACKET;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_bracket.bracket = '[';
            tok.as_bracket.depth = ++depth;
            tokens_push(&tokens, &tok);
        } else if (c == ']') {
            if (depth == 0) {
                free_tokens(tokens);
                return (TokensResult) {
                    .isok = false,
                    .err = (Error) {.location = (ErrorLocation) {.source = src, .src_idx = i, .len = 1},
                                    .message = strvc_fromstrv(
                                        strv_fromcstr("Source has unmatched brackets: extra closing bracket"))}};
            }

            emit_token(&tokens, &tok, &has_token);

            tok = (Token) {0};
            tok.type = TOKEN_BRACKET;
            tok.src_idx = i;
            tok.len = 1;
            tok.as_bracket.bracket = ']';
            tok.as_bracket.depth = depth--;
            tokens_push(&tokens, &tok);
        } else if (c == '\'') {
            if (i + 1 == src_len) {
                free_tokens(tokens);
                return (TokensResult) {
                    .isok = false,
                    .err = (Error) {(ErrorLocation) {
                                        .source = src,
                                        .src_idx = i,
                                        .len = 1,
                                    },
                                    .message = strvc_fromstrv(strv_fromcstr("Syntax error: standalone quote"))}};
            }
            if (!is_valid_word_tok_first(strv_char_at(src, i + 1))) {
                free_tokens(tokens);
                return (TokensResult) {.isok = false,
                                       .err = (Error) {(ErrorLocation) {
                                                           .source = src,
                                                           .src_idx = i,
                                                           .len = 2,
                                                       },
                                                       .message = strvc_fromstrv(strv_fromcstr(
                                                           "Syntax error: quote must be followed by word"))}};
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

                tok = (Token) {0};
                tok.type = TOKEN_WHITESPACE;
                tok.src_idx = i;
                slice_begin = i;
                has_token = true;
            }

            tok.as_whitespace = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len++;
        } else if (c_isdigit(c)) {
            if (has_token && tok.type == TOKEN_WORD
                && (strv_cmpcstr(tok.as_word, "-") == 0 || strv_cmpcstr(tok.as_word, "-.") == 0
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

            tok = (Token) {0};
            tok.type = TOKEN_WORD;
            tok.src_idx = i;
            slice_begin = i;
            tok.as_word = strv_slice(src, slice_begin, i + 1 - slice_begin);
            tok.len = 1;
            has_token = true;
        } else {
            StringVChain chain = strvc_newempty();
            strvc_push(&chain, strv_fromcstr("Parsing error: Unexpected character found: "));
            strvc_push(&chain, strv_slice(src, i, 1));
            free_tokens(tokens);
            return (TokensResult) {
                .isok = false,
                .err = (Error) {.location = (ErrorLocation) {.source = src, .src_idx = i, .len = 1}, .message = chain}};
        }
    }

    if (depth != 0) {
        free_tokens(tokens);
        return (TokensResult) {.isok = false,
                               .err = (Error) {.location = {0},
                                               .message = strvc_fromstrv(strv_fromcstr(
                                                   "Source has unmatched brackets: too many opening brackets"))}};
    }

    emit_token(&tokens, &tok, &has_token);

    return (TokensResult) {.isok = true, .res = tokens};
}

void emit_value(ValueScratch *root, const Value *value, bool *has_node) {
    if (*has_node) {
        *has_node = false;
        vscratch_push(root, value);
    }
}

ValueResult parse_number(const Token token, const StringView source) {
    Value result;
    StringVChain chain = {0};
    size_t i = 0;
    uint32_t c = strv_char_at(token.as_number, i);
    const bool isneg = c == '-';
    if (isneg) c = strv_char_at(token.as_number, ++i);
    const size_t len = strv_len(token.as_number);
    if (strv_contains(token.as_number, '.')) {
        result.type = TYPE_DOUBLE;
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
        result.type = TYPE_INT;
        long long a = 0;
        for (; i < len; i++) {
            c = strv_char_at(token.as_number, i);
            const long long digit = (isneg ? (-1 * (long long) (c - ASCII_0)) : (long long) (c - ASCII_0));

            if (mul_overflow_ll(a, 10, &a)) goto overflow;
            if (add_overflow_ll(a, digit, &a)) goto overflow;
        }
        result.as_integer = a;
    }

    return (ValueResult) {.isok = true, .res = result};

overflow:
    chain = strvc_newempty();
    strvc_push(&chain, strv_fromcstr("Parsing error: number "));
    strvc_push(&chain, token.as_number);
    strvc_push(&chain, strv_fromcstr(" is too large to fit in a long long."));
    return (ValueResult) {
        .isok = false,
        .err = (Error) {.location = (ErrorLocation) {.source = source, .src_idx = token.src_idx, .len = token.len},
                        .message = chain}};
}

void vscratch_pushblock(ValueScratch *vs, Sizes *sizes, const size_t current_depth) {
    Block b;
    Value vb;
    if (vs->len == 0) {
        goto push_empty;
    }
    size_t i = sizes->len - 1;
    size_t buflen = 0;
    while (sizes->ptr[i] == current_depth) {
        buflen++;
        i--;
    }
    if (buflen) {
        b = alloc_block(buflen);
        vb = (Value) {TYPE_BLOCK, .as_block = b};
        memcpy(b.elements, vs->ptr + i + 1, buflen * sizeof(Value));
        vs->len -= buflen;
        vscratch_push(vs, &vb);
        return;
    } else {
        goto push_empty;
    }
push_empty:
    b = alloc_block(0);
    vb = (Value) {TYPE_BLOCK, .as_block = b};
    vscratch_push(vs, &vb);
}

BlockResult parse_tokens(const TokensSlice tokens, const StringView source) {
    ValueScratch vs = alloc_vscratch();
    Sizes sizes = alloc_sizes();
    Token tok;
    Value val;
    ValueResult vr;
    size_t depth = 0;
    bool is_quoted = false;
    for (size_t i = 0; i < tokens.len; i++) {
        tok = tokens.ptr[i];
        switch (tok.type) {
            case TOKEN_QUOTE:
                is_quoted = true;
                break;
            case TOKEN_WORD:
                val.type = is_quoted ? TYPE_LITWORD : TYPE_WORD;
                val.as_word = tok.as_word;
                vscratch_push(&vs, &val);
                sizes_push(&sizes, &depth);
                is_quoted = false;
                break;
            case TOKEN_BRACKET:
                if (tok.as_bracket.bracket == '[') {
                    ++depth;
                } else {
                    vscratch_pushblock(&vs, &sizes, depth);
                    --depth;
                }
                break;
            case TOKEN_NUMBER:
                vr = parse_number(tok, source);
                if (!vr.isok) {
                    free_vscratch(vs);
                    return (BlockResult) {.isok = false, .err = vr.err};
                }
                val = vr.res;
                vscratch_push(&vs, &val);
                sizes_push(&sizes, &depth);
                break;
            case TOKEN_WHITESPACE:
                break;
        }
    }
    Block b = alloc_block(vs.len);
    memcpy(b.elements, vs.ptr, vs.len * sizeof(Value));
    free(vs.ptr);
    free(sizes.ptr);
    return (BlockResult) {.isok = true, .res = b};
}
