#include "string/string.h"
#include "parsing.h"

int main(void) {
    const StringView sample_source = strv_fromcstr(
        "              -1 2 3 -4009000000000000 5 6 [ -.7 8 -9 123456.7890123456 [ -0000001.000001 ]   le.wo-rd  -00000327156028 --- - 2 ]     ");

    const Tokens tokens = tokenize(sample_source);

    size_t parsing_index = 0;
    const ASTBlock root = parse_tokens((TokensSlice){tokens.ptr, tokens.len}, &parsing_index);

    printast(root, 0);


    free_tokens(tokens);
    free_block(root);

    return 0;
}
