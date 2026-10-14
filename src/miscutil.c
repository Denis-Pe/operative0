#include <limits.h>

#include "miscutil.h"

#if defined(__clang__) || defined(__GNUC__)

bool add_overflow_ll(const long long a, const long long b, long long *result) {
    return __builtin_add_overflow(a, b, result);
}

bool mul_overflow_ll(const long long a, const long long b, long long *result) {
    return __builtin_mul_overflow(a, b, result);
}

#else

bool add_overflow_ll(const long long a, const long long b, long long *result) {
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        return true;
    }
    *result = a + b;
    return false;
}

bool mul_overflow_ll(const long long a, const long long b, long long *result) {
    if (a > 0) {
        if (b > 0 && a > INT_MAX / b) return true;
        if (b < 0 && b < INT_MIN / a) return true;
    } else if (a < 0) {
        if (b > 0 && a < INT_MIN / b) return true;
        if (b < 0 && b < INT_MAX / a) return true;
    }
    *result = a * b;
    return false;
}

#endif

