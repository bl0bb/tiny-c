#ifndef AST_H
#define AST_H

#include "tokenizer.h"
#include "type.h"

// AST = abstract syntax tree
// a way to represent program logic
// it represents "indidivual actions" the program logic does, not the "higher level" definitions
// e.g. there is no "expression" or "statement" type as these labels are a category of multiple actions a program can do
// e.g. these are expressions: 1 + 1, x * 3
// but they are different logical actions (addition, subtraction)
// ast nodes represent the lowest representation of logic without stepping into assembly

typedef enum {
    AST_NODE_NONE = 0,

    AST_NODE_ADD, // +
    AST_NODE_SUB, // -
    AST_NODE_MUL, // *
    AST_NODE_DIV, // /
    AST_NODE_MOD, // %

    AST_NODE_LSHIFT, // <<
    AST_NODE_RSHIFT, // >>

    AST_NODE_NEG, // unary -

    AST_NODE_ADDR, // unary &
    AST_NODE_DEREF, // unary *

    // -> is just: deref then member access
    // like this:
    // someStruct->someMember
    // same as
    // (*someStruct).someMember
    AST_NODE_MEMBER, // .

    AST_NODE_BNOT, // ~
    AST_NODE_BAND, // &
    AST_NODE_BOR, // |
    AST_NODE_BXOR, // ^

    AST_NODE_LNOT, // !
    AST_NODE_LAND, // &&
    AST_NODE_LOR, // ||
    // wouldnt this be funny
    // AST_NODE_LXOR, // ^^

    AST_NODE_EQ, // ==
    AST_NODE_NE, // !=
    AST_NODE_LE, // <=
    AST_NODE_GE, // >=
    AST_NODE_LT, // <
    AST_NODE_GT, // >

    AST_NODE_IF, // "if"
    AST_NODE_FOR, // "for" or "while"
    AST_NODE_DO, // "do"
    AST_NODE_SWITCH, // "switch"
    AST_NODE_CASE, // "case" in a "switch"
    AST_NODE_BLOCK, // block { ... }
    AST_NODE_NUM, // numeric literal
    AST_NODE_CAST, // type cast
    AST_NODE_TERN, // ? :
    AST_NODE_RET, // "return"

    AST_NODE_COMMA, // ,

    AST_NODE_ASSIGN, // assignment =
    AST_NODE_VAR, // variable
    AST_NODE_FUNCALL, // function call
} ASTNodeType;

typedef struct ASTNode ASTNode;
struct ASTNode {
    ASTNodeType type;

    Type *ty;

    union {
        // if, ternary
        struct {
            ASTNode *cond;
            ASTNode *then;
            ASTNode *els;
        };
    };
};

#endif