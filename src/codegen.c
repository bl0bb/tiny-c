#include <stdlib.h>
#include <stdio.h>

#include "string.h"
#include "codegen.h"

const char *OPERAND_SIZES[] = {
    "BYTE",
    "WORD",
    "DWORD",
    "QWORD",
};

const char *codegen_get_operand(i32 size) {
    if (size > 8) {
        printf("Operand size too large\n");
        exit(1);
    }
    if (size > 4) {
        return OPERAND_SIZES[3];
    } else if (size > 2) {
        return OPERAND_SIZES[2];
    } else if (size > 1) {
        return OPERAND_SIZES[1];
    }
    return OPERAND_SIZES[0];
}

// writes an instruction that updates a variable (stack is first operand)
// e.g. assigning, adding, subtracting, etc...
// e.g. move DWORD PTR [rbp-4], 5
void codegen_write_modify_variable_mem_op_str(char **code, const char *op, i32 size, i32 loc, const char *data) {
    asprintf(code, "%s %s PTR [rbp-%d], %s\n", op, codegen_get_operand(size), loc, data);
}
// writes an instruction that updates a variable (stack is second operand)
// e.g. eax, move DWORD PTR [rbp-4]
void codegen_write_modify_variable_str_mem_op(char **code, const char *op, i32 size, i32 loc, const char *data) {
    asprintf(code, "%s %s, %s PTR [rbp-%d]\n", op, data, codegen_get_operand(size), loc);
}

void codegen_write_func_start(char **code, const char *name) {
    const char *fmt =
        "%s:\n"
        "push\trbp\n"
        "mov\trbp, rsp\n";
    asprintf(code, fmt, name);
}

void codegen_write_func_end(char **code) {
    const char *fmt =
        "leave\n"
        "ret\n";
    asprintf(code, fmt);
}

void codegen_write_func_reserve_stack_space(char **code, u32 stack_space) {
    const char *fmt =
        "sub rsp, %u\n";
    asprintf(code, fmt, stack_space);
}

char *codegen_gen_assembly(ASTObj *globals) {
    
}