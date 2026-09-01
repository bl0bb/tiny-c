#include "type.h"

// predefine primitive types
ty_void = &(Type) { TY_VOID, 1, 1, false };
ty_bool = &(Type) { TY_BOOL, 1, 1, false };

ty_u8 = &(Type) { TY_INT8, 1, 1, false };
ty_u16 = &(Type) { TY_INT16, 2, 2, false };
ty_u32 = &(Type) { TY_INT32, 4, 4, false };
ty_u64 = &(Type) { TY_INT64, 8, 8, false };

ty_i8 = &(Type) { TY_INT8, 1, 1, true };
ty_i16 = &(Type) { TY_INT16, 2, 2, true };
ty_i32 = &(Type) { TY_INT32, 4, 4, true };
ty_i64 = &(Type) { TY_INT64, 8, 8, true };

// create new basic type
Type *type_new(TypeKind type, u32 size, u32 align) {
    Type *ty = calloc(1, sizeof(Type));
    ty->size = size;
    ty->align = align;
    return ty;
}

// shallow copy of a type
Type *type_copy(Type *ty) {
    Type *res = malloc(sizeof(Type));
    *res = *ty;
    return res;
}

// returns a new type, a pointer to the given type
// e.g.
// i32 -> *i32
// the pointer is returned obviously (read more in the comments above Type definition)
Type *type_pointer_to(Type *base) {
    Type *ty = type_new(TY_PTR, 8, 8);
    ty->is_signed = false;
    ty->base = base;
    return ty;
}