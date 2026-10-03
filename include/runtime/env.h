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
} optsize;

typedef struct Frame {
    ScopeBindings bindings;
    Operative *op;
    optsize parent_idx;
    size_t walk_idx;
} Frame;

DECLARE_SEQ(FrameStack, fstack, Frame)

typedef struct Call Call;

struct Call {
    Operative *op;
    Block args;
    size_t args_taken;
    optsize returningtocall_idx;
    size_t frame_idx;
};

DECLARE_SEQ(CallStack, cstack, Call)

typedef struct {
    bool is_bound;
    Value value;
} LookupResult;

LookupResult lookup_bindings(ScopeBindings bindings, Word key);

LookupResult lookup(FrameStack, Frame startingfrom, Word key);

#endif //OPERATIVE_ENV_H
