#include "type.h"
#include <stdlib.h>

// predefine primitive types
Type *ty_void = &(Type) { TY_VOID, 1, 1, false };
Type *ty_bool = &(Type) { TY_BOOL, 1, 1, false };

Type *ty_u8 = &(Type) { TY_INT8, 1, 1, false };
Type *ty_u16 = &(Type) { TY_INT16, 2, 2, false };
Type *ty_u32 = &(Type) { TY_INT32, 4, 4, false };
Type *ty_u64 = &(Type) { TY_INT64, 8, 8, false };

Type *ty_i8 = &(Type) { TY_INT8, 1, 1, true };
Type *ty_i16 = &(Type) { TY_INT16, 2, 2, true };
Type *ty_i32 = &(Type) { TY_INT32, 4, 4, true };
Type *ty_i64 = &(Type) { TY_INT64, 8, 8, true };

Type *ty_f32 = &(Type) { TY_F32, 4, 4, true };
Type *ty_f64 = &(Type) { TY_F64, 8, 8, true };

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

// returns a new type, an array to the given type
// more or less the same as "type_pointer_to" but with array instead of a pointer
Type *type_array_of(Type *base, ASTNode *array_len_ast) {
    Type *ty = type_new(TY_ARRAY, 8, 8);
    ty->is_signed = false;
    ty->base = base;
    ty->array_len_ast = array_len_ast;
    return ty;
}

// creates a function with the return value to the given type
Type *type_func(Type *base) {
    // functions "do not exist" (hence 0 size and 0 align), they are purely for compile time validation
    // upon compilation a function gets a memory address, which is why any reference to a function must be a pointer
    // the function type only supplies the full type with the parameters and that the type is a function
    Type *ty = type_new(TY_FUNC, 0, 0);
    ty->is_signed = false;
    ty->base = base;
    return ty;
}