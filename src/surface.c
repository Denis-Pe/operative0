//
// Created by denis on 10/9/26.
//

#include "surface.h"

#include <assert.h>

#define KILO 1024

String *read_file(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) {
        return NULL;
    }

    size_t fsize = 0;
    if (fseek(f, 0, SEEK_END) == 0) {
        fsize = ftell(f);
        rewind(f);
    }

    String *s = alloc_str_cap(fsize > 0 ? fsize : KILO);

    while (!(ferror(f) || feof(f))) {
        str_pushfile(&s, f, KILO);
    }

    if (ferror(f)) {
        fclose(f);
        free_str(s);
        return NULL;
    }

    fclose(f);
    return s;
}
