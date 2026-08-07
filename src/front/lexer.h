
#include "token.h"

typedef struct lexer {
    Token **tokens;
} Lexer;

Lexer* create_lexer();
void free_lexer(Lexer*);
void tokenize(Lexer*, char*);
