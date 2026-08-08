
#include "lexer.h"
#include "parser.h"

void front(char* source) {
    Lexer* lexer = create_lexer();

    tokenize(lexer, source);

    Parser* parser = create_parser(lexer->tokens);
    parse(parser);
    print_tree(parser);

    free_parser(parser);
    free_lexer(lexer);
}
