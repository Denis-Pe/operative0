//
// Created by denis on 9/7/26.
//

#include "runtime/env.h"
#include "runtime/value.h"
#include "seq.h"

DEFINE_SEQ(ScopeBindings, bindings, Binding)
DEFINE_SEQ(FrameStack, fstack, Frame)
DEFINE_SEQ(CallStack, cstack, Call)

LookupResult lookup_bindings(const ScopeBindings bindings, const Word key) {
    for (size_t i = 0; i < bindings.len; i++) {
        const Binding b = bindings.ptr[i];
        if (strv_cmp(b.word, key) == 0) {
            return (LookupResult){true, b.value};
        }
    }

    return (LookupResult){false};
}

LookupResult lookup(const FrameStack fstack, const Frame *startingfrom, const Word key) {
    const Frame *f = startingfrom;
    while (f) {
        const LookupResult lr = lookup_bindings(f->bindings, key);
        if (lr.is_bound) {
            return lr;
        }
        f = f->parent_idx.is_there ? fstack.ptr + f->parent_idx.value : NULL;
    }
    return (LookupResult){false};
}

void bind(ScopeBindings *bindings, const Word key, const Value v) {
    for (size_t i = 0; i < bindings->len; i++) {
        const Binding b = bindings->ptr[i];
        const Word bk = b.word;
        if (strv_cmp(key, bk) == 0) {
            bindings->ptr[i].value = v;
            return;
        }
    }
    const Binding newb = (Binding){key, v};
    bindings_push(bindings, &newb);
}
