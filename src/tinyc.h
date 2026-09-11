#ifndef TINY_C_H
#define TINY_C_H

#include "types.h"



// typedefs
typedef struct Token Token;
typedef struct Type Type;
typedef struct ASTNode ASTNode;
typedef struct DeclAttr DeclAttr;
typedef struct ASTObj ASTObj;




// this is part of AST
// but has to be up here because c cant determine a structs size if it uses a member like "DeclAttr attr;" (keep in mind "DeclAttr *attr;" works because its a pointer, and c always knows the size of a pointer)
// attributes of a declaration
struct DeclAttr {
    bool is_const;
    bool is_static;
    bool is_extern;
    bool is_inline;
    bool is_typedef;
    i32 align;
};




// Token
typedef enum {
    TOK_NONE = 0,
    TOK_NUM, // since chars and numbers are "basically the same" (both are numerical values), they are treated as numbers
    TOK_STR,
    TOK_KEYWORD, // or variable
    TOK_PUNCT,
} TokenType;



struct Token {
    TokenType type;

    char *start;
    u16 len;




    // data for each token type:

    union {
        // string literal (e.g. "abcdefg"), then store the null-terminated string here
        struct {
            char *str_data;
        };

        // number literal (e.g. 1, 0b0101, 0x36, 0.4f, .3)
        // or char literal (e.g. 'a', '\0', '\x36')
        // since chars are numerical values, there is no point in making separate logic for chars and numbers when they behave "the same" under the hood
        // this is fine unless for type checking in strict languages (like C++) or maybe ill implement something later
        // then it wont know if a number token is really a number or a char
        struct {
            // for easy access
            // im unsure about when C converts number to the desired type and when it doesnt
            // e.g. putting f64 into an u64 would convert the float to an unsigner integer right? and modify the bytes to achieve the conversion?
            // so im just adding each """type""" of number (unsigned, signed, floating point) to make it more readable and prevent potential bugs
            union {
                u64 u64_val;
                i64 i64_val;
                f64 f64_val;
            };
        };

        // punct
        struct {
            // stores the index of the punct found in TOKENIZER_PUNCTS
            i8 punct_idx;
        };
    };



    // next token
    Token *next;
};








// Type
// calling it TypeType sounds weird, so i just replaced the last Type with a synonym: "Kind"
// as in: what Kind of Type is this
typedef enum {
    TY_NONE = 0,

    TY_VOID,
    TY_BOOL,

    TY_INT8,  // 8 bit integer (signed or unsigned)
    TY_INT16, // 16 bit integer (signed or unsigned)
    TY_INT32, // 32 bit integer (signed or unsigned)
    TY_INT64, // 64 bit integer (signed or unsigned)

    TY_F32, // 32 bit float (float)
    TY_F64, // 64 bit float (double)

    TY_PTR,
    TY_ARRAY,
    TY_FUNC,

    TY_ENUM,

    TY_STRUCT,
    TY_UNION,
} TypeKind;

// the type struct represents one part of a complete type (depending on what type it is)
// e.g.
// only one struct is needed for an int (i32): TypeKind: TY_INT32, is_signed: true
// but for pointers
// it becomes a linked list
// keep in mind types are read right to left
// e.g.
// i32*
// a variable with this type is storing a pointer, not an i32
// derefing the pointer gives the NEXT type in line (i32)
// so in memory its represented like this
// ptr (->base) i32
// the base is only used for pointers (and function pointers, if that wasnt clear)
// because a pointer by itself is not a complete type, its pointing to some other type, hence the base member
struct Type {
    TypeKind type;

    u32 size;
    u32 align;
    bool is_signed;

    // token representation of the type, for compound literals and typedefs
    // and getting the name of the variable?
    // or should that be its own member?
    // TODO: above
    Token *tok;

    Type *base;

    // if type is in a function parameter, this is used to point to the next parameter
    Type *next_param;
    DeclAttr attr; // mainly used for to enforce "const" function arguments

    // type specific data
    union {
        // array
        struct {
            // length for array to be evaluated later
            ASTNode *array_len_ast;

            // TODO: use -1 for unknown length or a separate variable?
            // this cuts the max array size in half, but is simple and efficient
            i64 array_len;
            // while this preserves it, but adds complexity and 1 byte more memory
            // u64 array_len
            // bool array_len_known
        };
    };
};

extern Type *ty_void;
extern Type *ty_bool;

extern Type *ty_u8;
extern Type *ty_u16;
extern Type *ty_u32;
extern Type *ty_u64;

extern Type *ty_i8;
extern Type *ty_i16;
extern Type *ty_i32;
extern Type *ty_i64;

extern Type *ty_f32;
extern Type *ty_f64;








// AST
// AST = abstract syntax tree
// a way to represent program logic
// it represents "indidivual actions" the program logic does, not the "higher level" definitions
// e.g. there is no "expression" or "statement" type as these labels are a category of multiple actions a program can do
// e.g. these are expressions: 1 + 1, x * 3
// but they are different logical actions (addition, subtraction)
// ast nodes represent the lowest representation of logic without stepping into assembly

// function definitions arent logical nodes by themselves, they represent variables passed in, stack scope and how it behaves, a function call however is part of the logic

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

struct ASTNode {
    ASTNodeType type;

    Type *ty;

    // which token does this node belong to. used for debugging where parsing failed
    Token *tok;

    // in block statements, each statement inside is stored as a linked list
    // the "next" member is used to access the next statement
    ASTNode *next;

    // data for each type
    union {
        ASTNode *cond;
        ASTNode *then;

        // num
        struct {
            // just a copy paste from how tokens store numbers
            // im still unsure how to store and use them
            union {
                u64 u64_val;
                i64 i64_val;
                f64 f64_val;
            };
        };
        // unary / binary expression (unary only uses "lhs")
        // return also uses "lhs" for return value
        struct {
            ASTNode *lhs;
            ASTNode *rhs;
        };
        // if, ternary
        struct {
            ASTNode *els;
        };
        // for / while (while only uses "cond" and "then")
        struct {
            ASTNode *init;
            ASTNode *inc;
        };
        // do / do while (if its a "do while" and not a regular "do", then cond is NOT 0. if condition is 0, that means its a regular "do")
        // TODO: is there a better or easier-to-understand way of storing "do while" statements? should they perhaps be regular "while" statements with a flag of sorts? or just keep it as a "do"? idk

        // switch
        struct {
            ASTNode *cmp_val;
            ASTNode *next_case; // first case in the case linked list for this switch
        };
        // case (in a switch)
        struct {
            ASTNode *val; // the value to check for in this case
        };
        // function call
        struct {
            ASTNode *next_arg; // used for both the first argument and the next arguments
        };
        // block
        struct {
            // body is used to access the first element
            // because a block statement can have "loose" block statements inside
            // using "next" for the first statement and every next statement would not work
            // so we use "body" for the first statement
            // e.g.
            /*
            if (x > 5) {
                {
                    // logic here
                }
                {
                    // more logic here
                }
            }
            */
            ASTNode *body;
        };
        // variable
        struct {
            ASTObj *var;
        };
    };
};

// object that is not code
// this could be a string literal, global variable, function, etc...
struct ASTObj {
    // next global variable, next local variable, next function param
    ASTObj *next;

    Type *ty;

    Token *tok; // TODO: token for this object, idk if i need it

    ASTNode *init_data; // initializer for a global variable

    bool is_const;
    bool is_static;
    bool is_extern;
    bool is_inline;

    // function
    ASTNode *param; // first param, use .next on each param after that
    ASTNode *body; // function body
};





#endif