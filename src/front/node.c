
#include "TokenTypes.h"
#include "token.h"
#include "NodeTypes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


struct node
{
    enum NodeType class;
};

struct ZeroNode
{
    struct node class;
    enum TokenType type;
    char* value;
};

struct OneNode
{
    struct node class;
    enum TokenType type;
    char* value;
    struct node* child;
};

struct TwoNode
{
    struct node class;
    enum TokenType type;
    char* value;
    struct node* left_child;
    struct node* right_child;
};

typedef struct ManyNode
{
    struct node class;
    enum TokenType type;
    char* value;
    size_t child_count;
    struct node** children;
} manyNode;

#define upcast (struct node*)

struct node* create_factor(Token* obj) {
    
    struct ZeroNode* this = malloc(sizeof(struct ZeroNode));
    if(!this) {
        return NULL;
    }

    (upcast this)->class = Node_Zero;
    this->type = obj->type;
    this->value = strdup(obj->value);

    return upcast this;

}

struct node* create_unary(Token* obj, struct node* child) {
    struct ZeroNode* base = (struct ZeroNode*)create_factor(obj);
    if(!base) {
        return NULL;
    }
    struct OneNode* this = realloc(base, sizeof(struct OneNode));
    if(!this) {
        return NULL;
    }
    (upcast this)->class = Node_One;
    this->child = child;

    return upcast this;

}

struct node* create_binary(Token* obj, struct node* left_child, struct node* right_child) {
    struct ZeroNode* base = (struct ZeroNode*)create_factor(obj);
    if(!base) {
        return NULL;
    }
    struct TwoNode* this = realloc(base, sizeof(struct TwoNode));
    if(!this) {
        return NULL;
    }

    (upcast this)->class = Node_Two;

    this->left_child = left_child;
    this->right_child = right_child;

    return upcast this;

}

struct node* create_nary(Token* obj, size_t child_count, struct node** children) {
    struct ZeroNode* base = (struct ZeroNode*)create_factor(obj);
    if(!base) {
        return NULL;
    }

    struct ManyNode* this = realloc(base, sizeof(struct ManyNode));
    if(!this) {
        return NULL;
    }

    (upcast this)->class = Node_Many;
    this->child_count = child_count;
    this->children = NULL;

    if (child_count > 0) {
        this->children = malloc(sizeof(struct node*) * child_count);
        if (!this->children) {
            free(this);
            return NULL;
        }

        for (size_t i = 0; i < child_count; i++) {
            this->children[i] = children[i];
        }
    }

    return upcast this;
}

struct node* cast_to_unary(struct ZeroNode* obj, struct node* child) {

    struct OneNode* this = realloc(obj, sizeof(struct OneNode));
    (upcast this)->class = Node_One;
    this->child = child;

    return upcast this;

}

struct node* cast_to_binary(struct ZeroNode* obj, struct node* left_child, struct node* right_child) {

    struct TwoNode* this = realloc(obj, sizeof(struct TwoNode));
    (upcast this)->class = Node_Two;
    this->left_child = left_child;
    this->right_child = right_child;

    return upcast this;

}

struct node* cast_to_nary(struct ZeroNode* obj, size_t child_count, struct node** children) {
    struct ManyNode* this = realloc(obj, sizeof(struct ManyNode));
    if(!this) {
        return NULL;
    }

    (upcast this)->class = Node_Many;
    this->child_count = child_count;
    this->children = NULL;

    if (child_count > 0) {
        this->children = malloc(sizeof(struct node*) * child_count);
        if (!this->children) {
            free(this);
            return NULL;
        }

        for (size_t i = 0; i < child_count; i++) {
            this->children[i] = children[i];
        }
    }

    return upcast this;
}

struct node* append_many_child(struct node* n, struct node* child) {
    if (!n || n->class != Node_Many) {
        return NULL;
    }

    struct ManyNode* this = (struct ManyNode*)n;
    struct node** new_children = realloc(this->children, sizeof(struct node*) * (this->child_count + 1));
    if (!new_children) {
        return NULL;
    }

    this->children = new_children;
    this->children[this->child_count] = child;
    this->child_count += 1;

    return n;
}

void print_node(struct node* n, int c) {
    if (!n) {
        printf("(null)\n");
        return;
    }

    switch (n->class) {
    case Node_Zero: {
        struct ZeroNode* this = (struct ZeroNode*)n;
        printf("- %s, %d\n", this->value ? this->value : "(null)", this->type);
        break;
    }
    case Node_One: {
        struct OneNode* this = (struct OneNode*)n;
        printf("- %s, %d\n", this->value ? this->value : "(null)", this->type);
        for(size_t i = 0; i < c; i++) {
            printf("  ");
        }
        print_node(this->child, c+1);
        break;
    }
    case Node_Two: {
        struct TwoNode* this = (struct TwoNode*)n;
        printf("- %s, %d\n", this->value ? this->value : "(null)", this->type);
        for(size_t i = 0; i < c; i++) {
            printf("  ");
        }
        print_node(this->left_child, c+1);
        for(size_t i = 0; i < c; i++) {
            printf("  ");
        }
        print_node(this->right_child, c+1);
        break;
    }
    case Node_Many: {
        struct ManyNode* this = (struct ManyNode*)n;
        printf("- %s, %d (%zu children)\n", this->value ? this->value : "(null)", this->type, this->child_count);
        for (size_t idx = 0; idx < this->child_count; idx++) {
            for(size_t i = 0; i < c; i++) {
                printf("  ");
            }
            print_node(this->children[idx], c+1);
        }
        break;
    }
    }
}

void free_node(struct node* n) {
    if (!n) return;

    switch (n->class) {
    case Node_Zero: {
        struct ZeroNode* this = (struct ZeroNode*)n;
        free(this->value);
        free(this);
        break;
    }
    case Node_One: {
        struct OneNode* this = (struct OneNode*)n;
        free_node(this->child);
        free(this->value);
        free(this);
        break;
    }
    case Node_Two: {
        struct TwoNode* this = (struct TwoNode*)n;
        free_node(this->left_child);
        free_node(this->right_child);
        free(this->value);
        free(this);
        break;
    }
    case Node_Many: {
        struct ManyNode* this = (struct ManyNode*)n;
        for (size_t i = 0; i < this->child_count; i++) {
            free_node(this->children[i]);
        }
        free(this->children);
        free(this->value);
        free(this);
        break;
    }
    }
}


