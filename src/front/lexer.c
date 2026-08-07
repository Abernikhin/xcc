
#include "token.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct lexer {
    Token **tokens;
    size_t tokenc;
} Lexer;

Lexer* create_lexer() {

    Lexer* this = malloc(sizeof(Lexer));
    if(!this) {
        return NULL;
    }

    this->tokenc = 0;

    return this;

}

void free_lexer(Lexer* this) {

    for(size_t i = 0; i < this->tokenc; i++) {
        free_token(this->tokens[i]);
    }

    free(this);

}

static append(Lexer* this, Token* obj) {
    if(this->tokenc == 0) {
        this->tokens = malloc(sizeof(Token*));
        this->tokens[0] = obj;
        this->tokenc++;
    }

    this->tokens = realloc(this->tokens, this->tokenc++);
    this->tokens[this->tokenc-1] = obj;
}

#define add_name if(strlen(buffer) > 0) {\
    append(this, create_token(set_type(buffer), buffer));\
    free(buffer);\
    buffer = malloc(sizeof(char));\
    *buffer = '\0';\
}

static char* add_char(char* this, char obj) {

    size_t len = strlen(this);
    this = realloc(this, len+2);

    this[len] = obj;
    this[len+1] = '\0';

    return this;

}

void tokenize(Lexer* this, char* source) {

    char* buffer = malloc(sizeof(char));
    *buffer = '\0';

    char current;
    size_t i = 0;
    while(true) {
        current = source[i];
        if(current == '\0') break;

        if(
            current == ' '
            || current == '\t'
            || current == '\n'
        ) {
            add_name
            i++;
            continue;
        }

        if(current == ';') {
            add_name
            i++;
            append(this, create_token(Token_Semicolon, ";"));
            continue;
        }

        if(current == '=') {
            add_name
            i++;
            append(this, create_token(Token_Assign, "="));
            continue;
        }
        
        else {
            buffer = add_char(buffer, current);
            i++;
            continue;
        }
    }

    add_name
    append(this, create_token(Token_EOF, ""));

}
