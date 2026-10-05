#include <assert.h>

#include "string/string.h"
#include "parsing.h"
#include "seq.h"
#include "runtime/value.h"
#include "runtime/env.h"

Value identity(Call curr) {
    printf("Identity executed\n");
    return curr.args.elements[0];
}

Value set_value(Call curr, const Block args) {
    // assert(args.elements[0].type == TYPE_WORD);
    //
    // const Binding b = (Binding){
    //     .word = args.elements[0].as_word,
    //     .value = args.elements[1]
    // };
    // bindings_push(&curr.frame.bindings, &b);

    return (Value){.type = TYPE_NIL};
}

Value frozen(const ASTNode form) {
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
            result.type = TYPE_BLOCK;
            /* TODO leak ; make a global map of frozen blocks with main()-level lifetime?
                pre-allocate all literal blocks during parsing? */
            result.as_block = alloc_block(form.as_block.seq.len);
            for (size_t i = 0; i < form.as_block.seq.len; i++) {
                result.as_block.elements[i] = frozen(form.as_block.seq.ptr[i]);
            }
            break;
    }

    return result;
}

Value eval_ast(ASTBlock, ScopeBindings builtins);

Value eval_form(const ASTNode form, const FrameStack fstack, Frame *frame) {
    Value result;
    LookupResult lr;

    switch (form.type) {
        case AST_WORD:
            lr = lookup(fstack, frame, form.as_word);
            if (lr.is_bound) {
                result = lr.value;
            } else {
                fprintf(stderr, "Error: evaluated word `");
                fprintstrv(stderr, form.as_word);
                panicf("` is unbound.");
            }
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
            result = frozen(form);
            break;
        default:
            panic_switch();
    }

    return result;
}

// void do_call(Call c) {
//     switch (c.op.type) {
//         case OP_BUILTIN:
//             c.op.as_builtin(c);
//             break;
//         case OP_FUNCTION:
//
//
//
//     }
// }

Value eval_ast(const ASTBlock root, ScopeBindings builtins) {
    Value result = (Value){.type = TYPE_NIL};
    FrameStack fstack = alloc_fstack();
    Operative root_op = (Operative){
        .type = OP_FUNCTION,
        .expected_args = 0,
        .as_function = root
    };
    Frame root_frame = (Frame){
        .bindings = builtins,
        .cstack = alloc_cstack(),
        .op = root_op,
        .parent_idx = (OptSize){false},
        .walk_idx = 0
    };
    /**** both used in the switch below */
    Frame new_frame;
    Operative op;
    /****/
    fstack_push(&fstack, &root_frame);
    Frame *f = fstack_peek(&fstack);

    while (f) {
        CallStack *cstack = &f->cstack;
        Call *c = cstack_peek(cstack);

        ASTBlock tree = f->op.as_function; // builtins executed instantly below
        size_t i = f->walk_idx++;
        if (i == tree.seq.len) {
            if (cstack->len != 0) {
                fprintf(
                    stderr, "Error: the following block's last operative has incomplete arity. Expected %zu, got %zu\n",
                    c->args.len, c->args_taken);
                fprintast(stderr, tree, 0);
                panicf("");
            }
            free(f->bindings.ptr);
            free(cstack->ptr);
            fstack_pop(&fstack);
            f = fstack_peek(&fstack);
            continue;
        }
        result = eval_form(tree.seq.ptr[i], fstack, f);
        switch (result.type) {
            case TYPE_OP:
                op = result.as_op;
                Call newc = (Call){
                    op,
                    alloc_block(op.expected_args),
                    0,
                    cstack->len > 0 ? (OptSize){true, cstack->len - 1} : (OptSize){false},
                };
                cstack_push(cstack, &newc);
                c = cstack_peek(cstack);
                break;
            default:
                if (c) {
                    // if args_taken was already there, the call would've been executed below in the previous iteration
                    c->args.elements[c->args_taken++] = result;
                }
                break;
        }

        // TODO returning from user-defined functions
        while (c && c->args_taken == c->args.len) {
            op = c->op;
            switch (op.type) {
                case OP_BUILTIN:
                    result = op.as_builtin(*c);
                    break;
                case OP_FUNCTION:
                    new_frame = (Frame){
                        .bindings = alloc_bindings(),
                        .op = op,
                        .parent_idx = (OptSize){true, 0},
                        /* TODO the parent would be set at the point of its definition, i.e. in the builtin
                             so a function defined in a frame would have its parent set to the active frame */
                        .walk_idx = 0
                    };
                    fstack_push(&fstack, &new_frame);
                    break;
                default:
                    panic_switch();
            }
            free(c->args.elements);
            cstack_pop(cstack);
            c = cstack_peek(cstack);
            if (c) c->args.elements[c->args_taken++] = result; // TODO this is actually invalid on case OP_FUNCTION
        }

        f = fstack_peek(&fstack);
    }

    free(fstack.ptr);

    return result;
}

void def(ScopeBindings *bindings, const char *name, const size_t args, const BuiltInOp op) {
    bind(bindings, strv_fromcstr(name), (Value){
             .type = TYPE_OP, .as_op = (Operative){.type = OP_BUILTIN, .expected_args = args, .as_builtin = op}
         });
}

ScopeBindings default_builtins(void) {
    ScopeBindings bindings = alloc_bindings();
    def(&bindings, "identity", 1, identity);
    return bindings;
}

int main(void) {
    const StringView sample_source = strv_fromcstr(
        "         identity identity identity identity ");

    const Tokens tokens = tokenize(sample_source);
    const ASTBlock root = parse_tokens((TokensSlice){tokens.ptr, tokens.len});

    // printast(root, 0);
    const ScopeBindings builtins = default_builtins();
    const Value v = eval_ast(root, builtins);
    printval(v);
    printf("\n");

    free_tokens(tokens);
    free_block(root);

    return 0;
}
