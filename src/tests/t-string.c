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

void test_concat(void) {
    String *expected = alloc_str_fromcstr("nasislike");
    String *concat = alloc_str();
    String *nas = alloc_str_fromcstr("nas");
    String *is = alloc_str_fromcstr("is");
    String *like = alloc_str_fromcstr("like");
    str_pushstr(&concat, nas);
    str_pushstr(&concat, is);
    str_pushstr(&concat, like);
    TEST_ASSERT_TRUE(str_cmp(concat, expected) == 0);
    free_str(expected);
    free_str(concat);
    free_str(nas);
    free_str(is);
    free_str(like);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_equality);
    RUN_TEST(test_concat);
    return UNITY_END();
}
