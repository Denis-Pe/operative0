//
// Created by denis on 9/7/26.
//

#include "runtime/value.h"

#include <stdlib.h>
#include <string.h>

#include "error.h"

Block alloc_block(const size_t size) {
    const size_t allocation_size = size ? sizeof(Value) * size : 1;
    Value *ptr = malloc(allocation_size);
    if (ptr == NULL) {
        panic_errno();
    }
    return (Block) {ptr, size};
}

Block alloc_block_clean(const size_t size) {
    const Block b = alloc_block(size);
    for (size_t i = 0; i < size; i++) {
        b.elements[i] = (Value) {.type = TYPE_NIL};
    }
    return b;
}

void fprintval(FILE *stream, const Value v) {
    switch (v.type) {
        case TYPE_LITWORD:
            fprintf(stream, "'");
            fprintstrv(stream, v.as_word);
            break;
        case TYPE_WORD:
            fprintstrv(stream, v.as_word);
            break;
        case TYPE_DOUBLE:
            fprintf(stream, "%lf", v.as_double);
            break;
        case TYPE_INT:
            fprintf(stream, "%lld", v.as_integer);
            break;
        case TYPE_BLOCK:
            fprintf(stream, "[");
            if (v.as_block.len) {
                fprintval(stream, v.as_block.elements[0]);
            }
            for (size_t i = 1; i < v.as_block.len; i++) {
                fprintf(stream, " ");
                fprintval(stream, v.as_block.elements[i]);
            }
            fprintf(stream, "]");
            break;
        case TYPE_OP:
            switch (v.as_op.type) {
                case OP_BUILTIN:
                    fprintf(stream, "(BUILTIN @ %p)", (void *) (uintptr_t) v.as_op.as_builtin);
                    break;
                case OP_FUNCTION:
                    fprintf(stream, "(FUN BLOCK @ %p with length %zu)", (void *) v.as_op.as_function.elements,
                            v.as_op.as_function.len);
                    break;
                default:
                    exhaustive_switch();
            }
            break;
        default:
            exhaustive_switch();
    }
}

void printval(const Value v) {
    fprintval(stdout, v);
}

void free_value(Value v) {
    if (v.type == TYPE_BLOCK) {
        free_block(v.as_block);
    }
}

void free_block(Block b) {
    for (size_t i = 0; i < b.len; i++) {
        free_value(b.elements[i]);
    }
    free(b.elements);
}
