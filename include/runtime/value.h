//
// Created by denis on 9/7/26.
//

#ifndef OPERATIVE_VALUE_H
#define OPERATIVE_VALUE_H
#include "error.h"
#include "string/string.h"

typedef struct Value Value;

typedef StringView Word;

typedef struct {
    size_t idx;
    size_t len;
} SourceSpan;

typedef struct {
    Value *elements;
    size_t len;
} Block;

Block alloc_block(size_t size);

Block alloc_block_clean(size_t size);

void free_block(Block);

struct Call;

typedef Value (*BuiltInOp)(struct Call curr);

enum OpType {
    OP_BUILTIN,
    OP_FUNCTION
};

typedef struct {
    enum OpType type;
    size_t expected_args;

    union {
        BuiltInOp as_builtin;
        Block as_function;
    };
} Operative;

enum ValueType {
    TYPE_BLOCK,
    TYPE_WORD,
    TYPE_LITWORD,
    TYPE_INT,
    TYPE_DOUBLE,
    TYPE_OP,
    TYPE_NIL
};

struct Value {
    enum ValueType type;

    union {
        Word as_word;
        long long as_integer;
        double as_double;
        Block as_block;
        Operative as_op;
    };
};

typedef struct {
    bool isok;
    Value res;
    Error err;
} ValueResult;

typedef struct {
    bool isok;
    Block res;
    Error err;
} BlockResult;

void fprintval(FILE *, Value v);

void printval(Value v);

void free_value(Value v);

#endif //OPERATIVE_VALUE_H
