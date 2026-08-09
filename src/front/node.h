
#include "NodeTypes.h"
#include "TokenTypes.h"
#include "token.h"
#include <stddef.h>

struct node
{
    enum NodeType class;
};

typedef struct ZeroNode
{
    struct node class;
    enum TokenType type;
    char* value;
} zeroNode;

typedef struct OneNode
{
    struct node class;
    enum TokenType type;
    char* value;
    struct node* child;
} unaryNode;

typedef struct TwoNode
{
    struct node class;
    enum TokenType type;
    char* value;
    struct node* left_child;
    struct node* right_child;
} binaryNode;

typedef struct ManyNode
{
    struct node class;
    enum TokenType type;
    char* value;
    size_t child_count;
    struct node** children;
} manyNode;

#define upcast (struct node*)

struct node* create_factor(Token*);
struct node* create_unary(Token*, struct node*);
struct node* create_binary(Token*, struct node*, struct node*);
struct node* create_nary(Token*, size_t, struct node**);
struct node* cast_to_unary(struct ZeroNode*, struct node*);
struct node* cast_to_binary(struct ZeroNode*, struct node*, struct node*);
struct node* cast_to_nary(struct ZeroNode*, size_t, struct node**);
struct node* append_many_child(struct node*, struct node*);
void print_node(struct node*, int c);
void free_node(struct node*);