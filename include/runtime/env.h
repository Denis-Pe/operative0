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
    ScopeBindings globals;
} Environment;

typedef struct {
    Environment env;
    ScopeBindings locals;
    Operative* op;
} Frame;

DECLARE_SEQ(FrameStack, fstack, Frame)

typedef struct Call Call;

struct Call {
    Operative op;
    Block args;
    Call *returningto;
    Frame frame;
};

DECLARE_SEQ(CallStack, cstack, Call)

typedef struct {
    bool isBound;
    Value value;
} LookupResult;

LookupResult lookup(Environment, Word key);

#endif //OPERATIVE_ENV_H
