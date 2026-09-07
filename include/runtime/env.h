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

typedef struct Frame Frame;

struct Frame {
    Frame *parent;
    ScopeBindings bindings;
    Environment env;
};

Frame alloc_root_frame(void);

Frame alloc_subenv(Frame *parent);

void free_frame(Frame frame);

#endif //OPERATIVE_ENV_H
