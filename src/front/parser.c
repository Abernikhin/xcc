
#include "TokenTypes.h"
#include "node.h"
#include "token.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct parser {
    Token** tokens;
    struct node** tree;
    size_t tree_count;
    size_t pos;
    Token* current;
    bool error;
} Parser;

#define self Parser* this

Parser* create_parser(Token** tokens) {

    Parser* this = malloc(sizeof(Parser));
    if(!this) {
        return NULL;
    }

    this->tokens = tokens;
    this->pos = 0;
    this->current = (Token*)this->tokens[0];
    this->tree = NULL;
    this->tree_count = 0;
    this->error = false;

    return this;

}

void free_parser(self) {

    for(size_t i = 0; i < this->tree_count; i++) {
        free_node(this->tree[i]);
    }

    free(this->tree);

    free(this);

}

static void append(self, struct node* obj) {
    if(this->tree_count == 0) {
        this->tree = malloc(sizeof(struct node*) * 16);
        this->tree[0] = obj;
        this->tree_count++;
        return;
    }

    if(this->tree_count >= 16) {
        this->tree = realloc(this->tree, sizeof(struct node*) * (this->tree_count + 1));
        if (!this->tree) {
            printf("exit\n");
            exit(EXIT_FAILURE);
        }
    }
    this->tree[this->tree_count] = obj;
    this->tree_count++;
}

static void advance(self) {
    this->pos++;
    this->current = (Token*)this->tokens[this->pos];
}

static struct node* Storage(self);
static struct node* Modifier(self);
static struct node* Type(self);

static struct node* Pointer(self);
static struct node* Id(self);

static struct node* Expr(self);
static struct node* Term(self);
static struct node* Factor(self);

void Declaration(self);

void parse(self) {
    while (this->current->type != Token_EOF)
    {
        Declaration(this);
    }
    
}

void print_tree(self) {
    for(size_t i = 0; i < this->tree_count; i++) {
        print_node(this->tree[i], 1);
    }
}

static struct node* Storage(self) {
    if(this->current->type == Token_Storage) {
        struct node* obj = create_factor(this->current);
        advance(this);
        obj = cast_to_unary((zeroNode*)obj, Modifier(this));
        return obj;
    }

    Token* t = create_token(Token_Storage, "auto");
    struct node* obj = create_unary(t, Modifier(this));
    free_token(t);
    return obj;
}

static struct node* Modifier(self) {
    if(this->current->type == Token_Modifier) {
        struct node* obj = create_factor(this->current);
        advance(this);
        obj = cast_to_unary((zeroNode*)obj, Modifier(this));
        return obj;
    }

    return Type(this);
}

static struct node* Type(self) {
    if(this->current->type != Token_Type) {
        this->error = true;
    }

    struct node* obj = create_factor(this->current);
    advance(this);
    return obj;
}

static struct node* Pointer(self) {
    if(this->current->type == Token_Pointer || this->current->type == Token_Star || strcmp(this->current->value, "const") == 0) {
        struct node* obj = create_factor(this->current);
        advance(this);
        obj = cast_to_unary((zeroNode*)obj, Pointer(this));
        ((unaryNode*)obj)->type = Token_Pointer;
        return obj;
    }

    return Id(this);
}

static struct node* Id(self) {
    if(this->current->type != Token_Id) {
        this->error = true;
    }

    struct node* obj = create_factor(this->current);
    advance(this);
    return obj;
}

static struct node* Factor(self) {
    if(this->current->type == Token_Plus || this->current->type == Token_Minus) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* child = Factor(this);
        struct node* obj = create_unary(op, child);
        free_token(op);
        return obj;
    }

    if(this->current->type == Token_Lparent) {
        advance(this);
        struct node* obj = Expr(this);
        if(this->current->type != Token_Rparent) {
            this->error = true;
            printf("expected )\n");
            return obj;
        }
        advance(this);
        return obj;
    }

    if(this->current->type == Token_Number || this->current->type == Token_Id) {
        struct node* obj = create_factor(this->current);
        advance(this);
        return obj;
    }

    this->error = true;
    Token* t = create_token(Token_Number, "0");
    struct node* obj = create_factor(t);
    free_token(t);
    return obj;
}

static struct node* Term(self) {
    struct node* left = Factor(this);

    while(this->current->type == Token_Star || this->current->type == Token_Divide) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Factor(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Expr(self) {
    struct node* left = Term(this);

    while(this->current->type == Token_Plus || this->current->type == Token_Minus) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Term(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

void Declaration(self) {
    struct node* type = Storage(this);
    struct node* obj;
    while(true) {
        struct node* name = Pointer(this);
        if(this->current->type == Token_Assign) {
            advance(this);
            struct node* value = Expr(this);
            Token* a = create_token(Token_Assign, "=");
            Token* t = create_token(Token_Declaration, "declaration");
            struct node* data = create_binary(a, name, value);
            obj = create_binary(t, type, data);
            free_token(t);
            free_token(a);
            append(this, obj);
        } else {
            Token* t = create_token(Token_Declaration, "declaration");
            obj = create_binary(t, type, name);
            free_token(t);
            append(this, obj);
        }

        if(this->current->type == Token_Semicolon) {
            advance(this);
            break;
        }

        if(this->current->type == Token_Comma) {
            advance(this);
        }

        else {
            this->error = true;
            printf("exepted ;");
            return;
        }
    }
}
