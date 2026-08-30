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

typedef struct Type Type;
struct Type {
    TypeKind type;

    u32 size;
    u32 align;
    bool is_signed;

    // token representation of the type, for compound literals and typedefs
    Token *tok;
};

#endif