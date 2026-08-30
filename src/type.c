#include "type.h"

// predefine primitive types
ty_void = &(Type) { TY_VOID, 8, 8, false };
ty_bool = &(Type) { TY_BOOL, 8, 8, false };

ty_u8 = &(Type) { TY_INT8, 8, 8, false };
ty_u16 = &(Type) { TY_INT16, 16, 16, false };
ty_u32 = &(Type) { TY_INT32, 32, 32, false };
ty_u64 = &(Type) { TY_INT64, 64, 64, false };

ty_i8 = &(Type) { TY_INT8, 8, 8, true };
ty_i16 = &(Type) { TY_INT16, 16, 16, true };
ty_i32 = &(Type) { TY_INT32, 32, 32, true };
ty_i64 = &(Type) { TY_INT64, 64, 64, true };