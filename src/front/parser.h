#ifndef __PARSER_H__
#define __PARSER_H__

#include "token.h"
#include <stdbool.h>

typedef struct parser {
    bool error;
} Parser;

Parser* create_parser(Token** tokens);
void free_parser(Parser* parser);
void parse(Parser* parser);
void print_tree(Parser* parser);

#endif // __PARSER_H__
