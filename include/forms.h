#ifndef OPERATIVE_FORMS_H
#define OPERATIVE_FORMS_H
#include "root_forms.h"
#include "string_utils.h"

typedef struct Form Form;
typedef struct RootForms RootForms;

typedef enum {
    FORM_WORD,
    FORM_LONG,
    FORM_BLOCK
} FormType;

struct Form {
    union {
        String as_word;
        long as_long;
        RootForms as_block;
    };

    FormType type;
};

void free_form(Form form);

#endif //OPERATIVE_FORMS_H
