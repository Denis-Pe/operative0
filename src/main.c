#include <assert.h>

#include "string/string.h"
#include "parsing.h"
#include "seq.h"
#include "runtime/value.h"
#include "runtime/env.h"

Value identity(const Frame curr, const Block args) {
    return args.elements[0];
}

Value set_value(Frame curr, const Block args) {
    assert(args.elements[0].type == TYPE_WORD);

    const Binding b = (Binding){
        .word = args.elements[0].as_word,
        .value = args.elements[1]
    };
    bindings_push(&curr.bindings, &b);

    return (Value){.type = TYPE_INT, .as_integer = 0};
}

Value eval_ast(ASTBlock root);

Value eval_form(const ASTNode form) {
    Value result;

    switch (form.type) {
        case AST_WORD:
            result.type = TYPE_WORD;
            result.as_word = form.as_word;
            break;
        case AST_DOUBLE:
            result.type = TYPE_DOUBLE;
            result.as_double = form.as_double;
            break;
        case AST_INTEGER:
            result.type = TYPE_INT;
            result.as_integer = form.as_integer;
            break;
        case AST_BLOCK:
            result = eval_ast(form.as_block);
            break;
    }

    return result;
}

Value eval_ast(const ASTBlock root) {
    Value result = (Value){.type = TYPE_INT, .as_integer = 0}; // TODO what should an empty block "return"?

    for (size_t i = 0; i < root.len; i++) {
        result = eval_form(root.ptr[i]);
    }

    return result;
}

int main(void) {
    const StringView sample_source = strv_fromcstr(
        "              -1 2 3 -4009000000000000 5 6 [ -.7 8 -9 123456.7890123456 [ -0000001.000001 ]   le.wo-rd  -00000327156028 --- - 2 ]     ");

    const Tokens tokens = tokenize(sample_source);
    const ASTBlock root = parse_tokens((TokensSlice){tokens.ptr, tokens.len});

    // printast(root, 0);
    const Value v = eval_ast(root);
    printval(v);

    free_tokens(tokens);
    free_block(root);

    return 0;
}
