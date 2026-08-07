
#include "TokenTypes.h"

#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>

struct token
{
    
    enum TokenType type;
    char* value;

};

static char* Storage[] = {
    "auto",
    "static",
};

static char* Modifier[] = {
    "const",
};

static char* Types[] = {
    "char",
    "short",
    "int",
    "long",
    "float",
    "double",
    "bool"
};

static char* Pointer[] = {
    "const",
    "unique"
};

enum TokenType set_type(char* s) {

    for(size_t i = 0; i < strlen(s); i++) {
        if(!isdigit(s[i])) break;
        if(i == strlen(s)) return Token_Number;
    }

    for(char i = 0; i < 2; i++) {
        if(strcmp(Storage[i], s) == 0) return Token_Storage;
    }

    for(char i = 0; i < 1; i++) {
        if(strcmp(Modifier[i], s) == 0) return Token_Modifier;
    }

    for(char i = 0; i < 7; i++) {
        if(strcmp(Types[i], s) == 0) return Token_Type;
    }

    for(char i = 0; i < 2; i++) {
        if(strcmp(Pointer[i], s) == 0) return Token_Pointer;
    }

    return Token_Id;
}

struct token* create_token(enum TokenType t, char* v) {

    struct token* this = malloc(sizeof(struct token));
    if(!this) return NULL;
    
    this->type = t;
    this->value = stdup(v);

    return this;

}

void free_token(struct token* this) {

    free(this->value);
    free(this);

}

struct token* copy_token(struct token* this) {

    struct token* copy = malloc(sizeof(struct token));
    if(!copy) return NULL;
    
    copy->type = this->type;
    copy->value = stdup(this->value);

    return copy;

}

void print_token(struct token* this) {

    printf("Token:\n\ttype - %d\n\tvalue %s\n", this->type, this->value);
    
}
