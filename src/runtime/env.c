//
// Created by denis on 9/7/26.
//

#include "runtime/env.h"
#include "runtime/value.h"
#include "seq.h"

DEFINE_SEQ(ScopeBindings, bindings, Binding)

Frame alloc_root_frame(void) {
    return (Frame){NULL, alloc_bindings(), (Environment){alloc_bindings()}};
}

Frame alloc_subenv(Frame *parent) {
    return (Frame){parent, alloc_bindings(), (Environment){alloc_bindings()}};
}

void free_frame(const Frame frame) {
    free(frame.bindings.ptr);
}