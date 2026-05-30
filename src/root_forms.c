#include "root_forms.h"
#include "forms.h"

#include <stdlib.h>

RootForms alloc_root_forms_cap(const size_t cap) {
    Form* ptr = malloc(sizeof(Form) * cap);
    const RootForms forms = {ptr, 0, cap};
    return forms;
}

RootForms alloc_root_forms() {
    return alloc_root_forms_cap(16);
}

void root_forms_push(RootForms *root_forms, const Form *form) {
    if (root_forms->len == root_forms->cap) {
        root_forms->ptr = realloc(root_forms->ptr, root_forms->cap * 2);
        root_forms->cap *= 2;
    }
    root_forms->ptr[root_forms->len++] = *form;
}

void free_root_forms(const RootForms forms) {
    for (size_t i = 0; i < forms.len; i++) {
        free_form(forms.ptr[i]);
    }
    free(forms.ptr);
}