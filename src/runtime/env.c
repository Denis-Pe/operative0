//
// Created by denis on 9/7/26.
//

#include "runtime/env.h"
#include "runtime/value.h"
#include "seq.h"

DEFINE_SEQ(ScopeBindings, bindings, Binding)

LookupResult lookup_bindings(const ScopeBindings bindings, const Word key) {
    for (size_t i = 0; i < bindings.len; i++) {
        const Binding b = bindings.ptr[i];
        if (strv_cmp(b.word, key)) {
            return (LookupResult){true, b.value};
        }
    }

    return (LookupResult){false};
}

LookupResult lookup(const Environment env, const Word key) {
    const LookupResult lr = lookup_bindings(env.locals, key);
    if (lr.isBound) {
        return lr;
    } else {
        return lookup_bindings(env.globals, key);
    }
}
