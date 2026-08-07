
#include "TokenTypes.h"

typedef struct token {

    enum TokenType type;
    char* value;

} Token;

Token* create_token(enum TokenType, char*);
Token* copy_token(Token*);
void free_token(Token*);
void print_token(Token*);
enum TokenType set_type(char*);
