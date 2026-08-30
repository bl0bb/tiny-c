#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

#include "tokenizer.h"

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
typedef struct Type Type;
struct Type {
    TypeKind type;

    u32 size;
    u32 align;
    bool is_signed;

    // token representation of the type, for compound literals and typedefs
    Token *tok;

    Type *base;
};

#endif