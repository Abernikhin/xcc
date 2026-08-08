
#include "lexer.h"

void front(char* source) {
    Lexer* lexer = create_lexer();

    tokenize(lexer, source);
    print_tokens(lexer);

    free_lexer(lexer);
}
