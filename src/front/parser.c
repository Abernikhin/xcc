
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
static struct node* Call(self, struct node* callee);

static struct node* Expr(self);
static struct node* Assignment(self);
static struct node* LogicalOr(self);
static struct node* LogicalAnd(self);
static struct node* BitwiseOr(self);
static struct node* BitwiseXor(self);
static struct node* BitwiseAnd(self);
static struct node* Equality(self);
static struct node* Relational(self);
static struct node* Shift(self);
static struct node* Additive(self);
static struct node* Multiplicative(self);
static struct node* Unary(self);
static struct node* Primary(self);

void Function(self, struct node* body);
static void Declaration(self);

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
        printf("unexepted type %s\n", this->current->value);
    }

    struct node* obj = create_factor(this->current);
    advance(this);
    return obj;
}

static struct node* Pointer(self) {
    if(this->current->type == Token_Pointer || this->current->type == Token_Star || strcmp(this->current->value, "const") == 0) {
        struct node* obj = create_factor(this->current);
        advance(this);
        ((unaryNode*)obj)->type = Token_Pointer;
        if(this->current->type == Token_Rparent || this->current->type == Token_Comma) {
            return obj;
        }
        obj = cast_to_unary((zeroNode*)obj, Pointer(this));
        return obj;
    }

    return Id(this);
}

static struct node* Id(self) {
    if(this->current->type != Token_Id) {
        this->error = true;
        printf("unexepted name %s\n", this->current->value);
    }

    struct node* obj = create_factor(this->current);
    advance(this);
    return obj;
}

static struct node* Primary(self) {
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

    if(this->current->type == Token_Id) {
        Token* a = create_token(Token_Call, "()");
        Token* t = create_token(Token_Argumant, "argumants");
        struct node* name = create_factor(this->current);
        advance(this);
        struct node* obj;
        if(this->current->type == Token_Lparent) {
            advance(this);
            struct node* args = create_nary(t, 0, NULL);
            while(this->current->type != Token_Rparent) {
                append_many_child(args, Expr(this));
                if(this->current->type == Token_Comma) {
                    advance(this);
                    continue;
                }
                break;
            }
            advance(this);
            obj = create_binary(a, name, args);
        } else {
            obj = name;
        }
        free_token(a);
        free_token(t);
        return obj;
    }

    struct node* obj = create_factor(this->current);
    advance(this);
    return obj;
}

static struct node* Unary(self) {
    if(this->current->type == Token_Plus || this->current->type == Token_Minus || this->current->type == Token_LogicalNot || this->current->type == Token_BitNot || this->current->type == Token_Inc || this->current->type == Token_Dec) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* child = Unary(this);
        struct node* obj = create_unary(op, child);
        free_token(op);
        return obj;
    }

    return Primary(this);
}

static struct node* Multiplicative(self) {
    struct node* left = Unary(this);

    while(this->current->type == Token_Star || this->current->type == Token_Divide || this->current->type == Token_Modulo) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Unary(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Additive(self) {
    struct node* left = Multiplicative(this);

    while(this->current->type == Token_Plus || this->current->type == Token_Minus) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Multiplicative(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Shift(self) {
    struct node* left = Additive(this);

    while(this->current->type == Token_LeftShift || this->current->type == Token_RightShift) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Additive(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Relational(self) {
    struct node* left = Shift(this);

    while(this->current->type == Token_Less || this->current->type == Token_Greater || this->current->type == Token_LessEqual || this->current->type == Token_GreaterEqual) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Shift(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Equality(self) {
    struct node* left = Relational(this);

    while(this->current->type == Token_Equal || this->current->type == Token_NotEqual) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Relational(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* BitwiseAnd(self) {
    struct node* left = Equality(this);

    while(this->current->type == Token_BitAnd) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Equality(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* BitwiseXor(self) {
    struct node* left = BitwiseAnd(this);

    while(this->current->type == Token_BitXor) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = BitwiseAnd(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* BitwiseOr(self) {
    struct node* left = BitwiseXor(this);

    while(this->current->type == Token_BitOr) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = BitwiseXor(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* LogicalAnd(self) {
    struct node* left = BitwiseOr(this);

    while(this->current->type == Token_LogicalAnd) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = BitwiseOr(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* LogicalOr(self) {
    struct node* left = LogicalAnd(this);

    while(this->current->type == Token_LogicalOr) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = LogicalAnd(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Assignment(self) {
    struct node* left = LogicalOr(this);

    if(this->current->type == Token_Assign || this->current->type == Token_AddAssign || this->current->type == Token_SubAssign || this->current->type == Token_MulAssign || this->current->type == Token_DivAssign || this->current->type == Token_ModAssign || this->current->type == Token_AndAssign || this->current->type == Token_OrAssign || this->current->type == Token_XorAssign || this->current->type == Token_LeftShiftAssign || this->current->type == Token_RightShiftAssign) {
        Token* op = create_token(this->current->type, this->current->value);
        advance(this);
        struct node* right = Assignment(this);
        left = create_binary(op, left, right);
        free_token(op);
    }

    return left;
}

static struct node* Expr(self) {
    return Assignment(this);
}

void Function(self, struct node* body) {
    if(this->current->type == Token_Keyword) {
        if(strcmp(this->current->value, "return") == 0) {
            struct node* obj = create_factor(this->current);
            advance(this);
            obj = cast_to_unary((zeroNode*)obj, Expr(this));
            append_many_child(body, obj);
            return;
        } else if(strcmp(this->current->value, "loop") == 0) {
            struct node* obj = create_nary(this->current, 0, NULL);
            advance(this);
            if(this->current->type == Token_Begin) {
                advance(this);

                while(this->current->type != Token_End) {
                    Function(this, obj);
                    if(this->current->type == Token_Semicolon) {
                        advance(this);
                        continue;
                    }
                }
                advance(this);
                append_many_child(body, obj);
            } else {
                this->error = true;
                printf("expected { after loop\n");
            }
            return;
        } else if(strcmp(this->current->value, "break") == 0) {
            struct node* obj = create_factor(this->current);
            advance(this);
            if(this->current->type != Token_Semicolon) {
                this->error = true;
                printf("exepted ; after break\n");
            }
            append_many_child(body, obj);
        } else if(strcmp(this->current->value, "continue") == 0) {
            struct node* obj = create_factor(this->current);
            advance(this);
            if(this->current->type != Token_Semicolon) {
                this->error = true;
                printf("exepted ; after continue\n");
            }
            append_many_child(body, obj);
        } else if(strcmp(this->current->value, "if") == 0) {
            struct node* obj = create_factor(this->current);
            advance(this);
            if(this->current->type != Token_Lparent) {
                this->error = true;
                printf("exepted ( after if\n");
            }
            Token* b = create_token(Token_Body, "body");
            obj = cast_to_binary((zeroNode*)obj, Expr(this), create_nary(b, 0, NULL));
            if(
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_LogicalAnd ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_LogicalOr ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_LogicalNot ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_Equal ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_NotEqual ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_GreaterEqual ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_LessEqual ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_Greater ||
                ((binaryNode*)(((binaryNode*)obj)->left_child))->type != Token_Less
            ) {
                this->error = true;
                printf("condition mast return bool. last operation must be == != <= >= < > ! && || !\n");
            }
            if(this->current->type == Token_Begin) {
                advance(this);

                while(this->current->type != Token_End) {
                    Function(this, ((binaryNode*)obj)->right_child);
                    if(this->current->type == Token_Semicolon) {
                        advance(this);
                        continue;
                    }
                }
                advance(this);
                append_many_child(body, obj);
            } else {
                this->error = true;
                printf("expected { after if(condition)\n");
            }
            return;
        } else if(strcmp(this->current->value, "else") == 0) {
            struct node* obj = create_nary(this->current, 0, NULL);
            advance(this);
            if(this->current->type == Token_Begin) {
                advance(this);

                while(this->current->type != Token_End) {
                    Function(this, obj);
                    if(this->current->type == Token_Semicolon) {
                        advance(this);
                        continue;
                    }
                }
                advance(this);
                append_many_child(body, obj);
            } else {
                this->error = true;
                printf("expected { after else\n");
            }
            return;
        }

        return;
    }

    if(
        this->current->type == Token_Storage ||
        this->current->type == Token_Modifier ||
        this->current->type == Token_Type
    ) {
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
                append_many_child(body, obj);
            } else {
                Token* t = create_token(Token_Declaration, "declaration");
                obj = create_binary(t, type, name);
                free_token(t);
                append_many_child(body, obj);
            }

            if(this->current->type == Token_Semicolon) {
                break;
            }

            else if(this->current->type == Token_Comma) {
                advance(this);
            }

            else {
                this->error = true;
                printf("exepted in function ;\n");
            }
        }
    }

    if(this->current->type == Token_Semicolon) {
        advance(this);
        return;
    }

    append_many_child(body, Expr(this));
}

static void Declaration(self) {
    struct node* type = Storage(this);
    struct node* obj;

    size_t capaicty = 0;

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
        } else if (this->current->type == Token_Lparent) {
            advance(this);
            Token* a = create_token(Token_Function, "function");
            Token* t = create_token(Token_Argumant, "argumants");
            Token* d = create_token(Token_Declaration, "declaration");
            Token* b = create_token(Token_Body, "body");
            struct node* args = create_nary(t, 0, NULL);
            while(this->current->type != Token_Rparent) {
                struct node* atype = Modifier(this);
                struct node* aname = Pointer(this);
                struct node* arg = create_binary(d, atype, aname);
                append_many_child(args, arg);
                if(this->current->type == Token_Comma) advance(this);
            }
            advance(this);
            obj = create_nary(a, 0, NULL);
            append_many_child(obj, type);
            append_many_child(obj, name);
            append_many_child(obj, args);
            if(this->current->type == Token_Begin) {
                if(capaicty > 0) {
                    this->error = true;
                    printf("unexepted {\n");
                }
                struct node* body = create_nary(b, 0, NULL);
                advance(this);

                while(this->current->type != Token_End) {
                    Function(this, body);
                    if(this->current->type == Token_Semicolon) {
                        advance(this);
                        continue;
                    }
                }
                append_many_child(obj, body);

                free_token(t);
                free_token(a);
                free_token(d);
                free_token(b);
                append(this, obj);
                advance(this);

                return;

            }

            free_token(t);
            free_token(a);
            free_token(d);
            free_token(b);
            append(this, obj);
        } else {
            Token* t = create_token(Token_Declaration, "declaration");
            obj = create_binary(t, type, name);
            free_token(t);
            append(this, obj);
        }

        capaicty++;

        if(this->current->type == Token_Semicolon) {
            advance(this);
            break;
        }

        else if(this->current->type == Token_Comma) {
            advance(this);
        }

        else {
            this->error = true;
            printf("exepted ;\n");
            return;
        }
    }
}
