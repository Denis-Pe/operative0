//
// Created by denis on 9/7/26.
//

#ifndef OPERATIVE_ENV_H
#define OPERATIVE_ENV_H
#include "runtime/value.h"
#include "seq.h"

typedef struct {
    Word word;
    Value value;
} Binding;

DECLARE_SEQ(ScopeBindings, bindings, Binding)

typedef struct {
    bool is_there;
    size_t value;
} OptSize;

typedef struct Call Call;

struct Call {
    Operative op;
    Block args;
    size_t args_taken;
};

DECLARE_SEQ(CallStack, cstack, Call)

typedef struct Frame {
    ScopeBindings bindings;
    CallStack cstack;
    Operative op;
    OptSize parent_idx;
    size_t walk_idx;
} Frame;

DECLARE_SEQ(FrameStack, fstack, Frame)

typedef struct {
    bool is_bound;
    Value value;
} LookupResult;

LookupResult lookup_bindings(ScopeBindings, Word key);

LookupResult lookup(FrameStack, const Frame *startingfrom, Word key);

void bind(ScopeBindings *, Word key, Value);

#endif //OPERATIVE_ENV_H
