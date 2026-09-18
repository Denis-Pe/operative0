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
    ScopeBindings locals;
} Environment;

typedef struct Call Call;

struct Call {
    Operative op;
    Block args;
    Call *returningto;
    ScopeBindings bindings;
    Environment env;
};

typedef struct {
    bool isBound;
    Value value;
} LookupResult;

LookupResult lookup(Environment, Word key);

#endif //OPERATIVE_ENV_H
