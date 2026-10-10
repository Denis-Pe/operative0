//
// Created by denis on 10/9/26.
//

#include "surface.h"

#include <assert.h>

String *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    const size_t fsize = ftell(f);
    rewind(f);

    String *s = alloc_str_cap(fsize);
    str_pushfile(&s, f, fsize);

    fclose(f);

    return s;
}
