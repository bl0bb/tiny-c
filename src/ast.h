#ifndef AST_H
#define AST_H

#include "tokenizer.h"

// AST = abstract syntax tree
// a way to represent program logic
// it represents "indidivual actions" the program logic does, not the "higher level" definitions
// e.g. there is no "expression" or "statement" type as these labels are a category of multiple actions a program can do
// e.g. these are expressions: 1 + 1, x * 3
// but they are different logical actions (addition, subtraction)
// ast nodes represent the lowest representation of logic without stepping into assembly

typedef enum {
    AST_NODE_TYPE_NONE = 0,

    AST_NODE_TYPE_ADD, // +
    AST_NODE_TYPE_SUB, // -
    AST_NODE_TYPE_MUL, // *
    AST_NODE_TYPE_DIV, // /
    AST_NODE_TYPE_MOD, // %

    AST_NODE_TYPE_LSHIFT, // <<
    AST_NODE_TYPE_RSHIFT, // >>

    AST_NODE_TYPE_NEG, // unary -

    AST_NODE_TYPE_ADDR, // unary &
    AST_NODE_TYPE_DEREF, // unary *

    // -> is just: deref then member access
    // like this:
    // someStruct->someMember
    // same as
    // (*someStruct).someMember
    AST_NODE_TYPE_MEMBER, // .

    AST_NODE_TYPE_BNOT, // ~
    AST_NODE_TYPE_BAND, // &
    AST_NODE_TYPE_BOR, // |
    AST_NODE_TYPE_BXOR, // ^

    AST_NODE_TYPE_LNOT, // !
    AST_NODE_TYPE_LAND, // &&
    AST_NODE_TYPE_LOR, // ||
    // wouldnt this be funny
    // AST_NODE_TYPE_LXOR, // ^^

    AST_NODE_TYPE_EQ, // ==
    AST_NODE_TYPE_NE, // !=
    AST_NODE_TYPE_LE, // <=
    AST_NODE_TYPE_GE, // >=
    AST_NODE_TYPE_LT, // <
    AST_NODE_TYPE_GT, // >

    AST_NODE_TYPE_IF, // "if"
    AST_NODE_TYPE_FOR, // "for" or "while"
    AST_NODE_TYPE_DO, // "do"
    AST_NODE_TYPE_SWITCH, // "switch"
    AST_NODE_TYPE_CASE, // "case" in a "switch"
    AST_NODE_TYPE_BLOCK, // block { ... }
    AST_NODE_TYPE_NUM, // numeric literal
    AST_NODE_TYPE_CAST, // type cast
    AST_NODE_TYPE_TERN, // ? :
    AST_NODE_TYPE_RET, // "return"

    AST_NODE_TYPE_COMMA, // ,

    AST_NODE_TYPE_ASSIGN, // assignment =
    AST_NODE_TYPE_VAR, // variable
    AST_NODE_TYPE_FUNCALL, // function call
} ASTNodeType;

typedef struct ASTNode ASTNode;
struct ASTNode {
    ASTNodeType type;
};

#endif