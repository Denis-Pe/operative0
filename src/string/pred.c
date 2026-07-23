//
// Created by denis on 7/23/26.
//

#include "string/pred.h"

#include <assert.h>

bool c_isletter(const uint32_t codepoint) {
    assert(codepoint <= 127);
    return (codepoint >= 'A' && codepoint <= 'Z')
           || (codepoint >= 'a' && codepoint <= 'z');
}

bool c_isdigit(const uint32_t codepoint) {
    assert(codepoint <= 127);
    return codepoint >= '0' && codepoint <= '9';
}

bool c_iswhitespace(const uint32_t codepoint) {
    assert(codepoint <= 127);
    return (codepoint >= 9 && codepoint <= 13) || codepoint == 32;
}
