#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>

#include "tinyc.h"


Type *type_new(TypeKind type, u32 size, u32 align);
Type *type_copy(Type *ty);
Type *type_pointer_to(Type *base);
Type *type_array_of(Type *base, ASTNode *array_len);
Type *type_func(Type *base);

#endif