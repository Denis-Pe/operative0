#include "forms.h"


void free_form(const Form form) {
    if (form.type == FORM_WORD) {
        free_str(form.as_word);
    }
}
