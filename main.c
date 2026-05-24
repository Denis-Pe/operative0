#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// Owned string, not null-terminated
typedef struct {
    char *ptr;
    size_t len;
    size_t cap;
} OString;

    

/// Borrowed string, not null-terminated
typedef struct {
    const char *ptr;
    size_t len;
} BString;

typedef enum {
    FORM_WORD,
    FORM_LONG,
} FormType;

typedef struct {
    union {
        OString as_word;
        long as_long;
    };

    FormType type;
} Form;

typedef enum {
    INIT,
    PARSING_WORD,
    PARSING_LONG,
} ParsingState;

/**
 * Parses a form until the next whitespace or the end of the string
 * @return
 */
Form parse_form(const BString src) {
}

int main(void) {
    char *sample_source = "function 1 nested-function 123 variable";

    printf("Hello, World!\n");

    return 0;
}
