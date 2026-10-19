#include "string/string.h"
#include "unity.h"

void setUp(void) {
}

void tearDown(void) {
}

void test_equality(void) {
    const char *uncooked = "fred";
    StringView a = strv_fromcstr(uncooked);
    String *s = alloc_str();
    str_push(&s, 'f');
    str_push(&s, 'r');
    str_push(&s, 'e');
    str_push(&s, 'd');
    TEST_ASSERT_TRUE(strv_cmp(a, strv_fromstr(s)) == 0);
    TEST_ASSERT_TRUE(strv_cmpcstr(a, uncooked) == 0);
    free_str(s);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_equality);
    return UNITY_END();
}
