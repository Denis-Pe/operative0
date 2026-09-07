//
// Created by denis on 9/7/26.
//

#ifndef OPERATIVE_VALUE_H
#define OPERATIVE_VALUE_H
#include "parsing.h"
#include "string/string.h"

typedef struct Value Value;

typedef StringView Word;

typedef struct {
    Value *elements;
    size_t len;
} Block;

struct Call;

typedef Value (*BuiltInOp)(struct Call curr);

typedef struct {
    size_t expected_args;

    union {
        BuiltInOp as_builtin;
        ASTBlock as_function;
    };
} Operative;

enum ValueType {
    TYPE_BLOCK,
    TYPE_WORD,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_OP
};

struct Value {
    enum ValueType type;

    union {
        Word as_word;
        int64_t as_integer;
        double as_double;
        Block as_block;
        Operative as_op;
    };
};

void printval(Value v);

#endif //OPERATIVE_VALUE_H
