#ifndef __PARSER_H__
#define __PARSER_H__

#include "token.h"

typedef struct parser Parser;

Parser* create_parser(Token** tokens);
void free_parser(Parser* parser);
void parse(Parser* parser);
void print_tree(Parser* parser);

#endif // __PARSER_H__
