#ifndef OPERATIVE_ERROR_H
#define OPERATIVE_ERROR_H

#include "string_utils.h"

void panic_cstr(const char* format, ...);
void panic_errno();
void panic();

#endif //OPERATIVE_ERROR_H
