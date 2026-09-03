#include <assert.h>

#include "string/string.h"
#include "parsing.h"
#include "seq.h"

typedef struct Value Value;

typedef StringView Word;

typedef struct {
    Value *elements;
    size_t len;
} Block;

enum ValueType {
    TYPE_BLOCK,
    TYPE_WORD,
    TYPE_INT,
    TYPE_DOUBLE
};

struct Value {
    enum ValueType type;

    union {
        Word as_word;
        int64_t as_integer;
        double as_double;
        Block as_block;
    };
};

void printval(const Value v) {
    switch (v.type) {
        case TYPE_WORD:
            fprintstrv(stdout, v.as_word);;
            break;
        case TYPE_DOUBLE:
            printf("%lf", v.as_double);
            break;
        case TYPE_INT:
            printf("%zu", v.as_integer);
            break;
        case TYPE_BLOCK:
            printf("[");
            if (v.as_block.len) {
                printval(v.as_block.elements[0]);
            }
            for (size_t i = 1; i < v.as_block.len; i++) {
                printf(" ");
                printval(v.as_block.elements[i]);
            }
            printf("]");
            break;
        default:
            panic_switch();
    }
}

typedef struct {
    Word word;
    Value value;
} Binding;

DECLARE_SEQ(ScopeBindings, bindings, Binding)

DEFINE_SEQ(ScopeBindings, bindings, Binding)

typedef struct Environment Environment;

struct Environment {
    Environment *parent;
    ScopeBindings bindings;
};

Environment alloc_root_env(void) {
    return (Environment){NULL, alloc_bindings()};
}

Environment alloc_subenv(Environment *parent) {
    return (Environment){parent, alloc_bindings()};
}

void free_env(const Environment env) {
    free(env.bindings.ptr);
}

typedef Value (*OperativeFun)(Environment curr, Block args);

typedef struct {
    OperativeFun fun;
    size_t expected_args;
} Operative;

Value identity(const Environment curr, const Block args) {
    return (Value){.type = TYPE_BLOCK, .as_block = args};
}

Value set_value(Environment curr, const Block args) {
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

    size_t parsing_index = 0;
    const ASTBlock root = parse_tokens((TokensSlice){tokens.ptr, tokens.len}, &parsing_index);

    // printast(root, 0);
    const Value v = eval_ast(root);
    printval(v);

    free_tokens(tokens);
    free_block(root);

    return 0;
}
