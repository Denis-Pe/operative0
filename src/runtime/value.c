//
// Created by denis on 9/7/26.
//

#include "runtime/value.h"

#include "error.h"

void printval(const Value v) {
    switch (v.type) {
        case TYPE_WORD:
            fprintstrv(stdout, v.as_word);
            break;
        case TYPE_DOUBLE:
            printf("%lf", v.as_double);
            break;
        case TYPE_INT:
            printf("%ld", v.as_integer);
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
        case TYPE_OP:
            switch (v.type) {
                case OP_BUILTIN:
                    printf("(BUILTIN @ %p)", (void *) (uintptr_t) v.as_op.as_builtin);
                    break;
                case OP_FUNCTION:
                    printf("(FUN BLOCK @ %p)", (void *) &v.as_block);
                    break;
                default:
                    panic_switch();
            }
            break;
        default:
            panic_switch();
    }
}
