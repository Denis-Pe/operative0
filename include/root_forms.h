#ifndef OPERATIVE_ROOT_FORMS_H
#define OPERATIVE_ROOT_FORMS_H
#include <stddef.h>

typedef struct Form Form;
typedef struct RootForms RootForms;

struct RootForms {
    Form *ptr;
    size_t len;
    size_t cap;
};

RootForms alloc_root_forms_cap(size_t cap);

RootForms alloc_root_forms();

void root_forms_push(RootForms *root_forms, const Form* form);

void free_root_forms(RootForms forms);

#endif //OPERATIVE_ROOT_FORMS_H
