#include "ast.h"
#include "array.h"
#include "log.h"
#include <stdlib.h>
#include <stdio.h>



const char *PARSER_QUALIFIERS[] = {
    // qualifiers
    "static", "extern", "inline", "const", "volatile",




    // // typedef
    // "typedef",

    // // types
    // // tags
    // "struct", "union",
    // "enum",

    // // misc
    // "void", "bool",

    // // integers
    // // unsigned
    // "u8", "u16", "u32", "u64",

    // // signed
    // "i8", "i16", "i32", "i64",


    // // floats
    // "f32", "f64",
};




File *current_file;

// globals and locals are stored as linked lists
// keep in mind that is uses a "reverse" linked list
// e.g. adding a new global variable
// new_global->next = globals;
// globals = new_global;
// this means "globals" and "locals" keep track of the last object added
// this is because it is simpler (only keep track of tail, not head and tail)
// and when same name variables exist in C, it references the last declaration with that name
// e.g.
/*
int x = 5;
if (true) {
    int x = 7;
    printf("%d\n", x); // 7, both variables exist, but x=7 was defined last
}
*/
// since they are stored from tail to head (reverse)
// this means searching through all variables for a name match means searching from last declared -> first declared
// which should give the last match added instead of the first
// keep in mind that same-name variables in the same scope will throw an error
// but in the example above, the if statement creates a new scope which allows for a new variable with the same name to be declared
ASTObj *globals;
ASTObj *locals;


bool ast_is_qualifier(Token *tok) {
    for (i32 i = 0; i < ARRAY_SIZE(PARSER_QUALIFIERS); i++) {
        if (tokenizer_token_equals(tok, PARSER_QUALIFIERS[i])) {
            return true;
        }
    }
    return false;
}



// ast obj
ASTObj *ast_create_obj(Type *ty, DeclAttr *attr) {
    ASTObj *obj = calloc(1, sizeof(ASTObj));
    obj->ty = ty;
    obj->is_const = attr->is_const;
    obj->is_static = attr->is_static;
    obj->is_extern = attr->is_extern;
    obj->is_inline = attr->is_inline;
    return obj;
}



// node creation helpers
ASTNode *ast_create_empty(ASTNodeType type, Token *tok) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    node->tok = tok;
    return node;
}

ASTNode *ast_create_unary(ASTNodeType type, ASTNode *lhs, Token *tok) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    node->tok = tok;
    node->lhs = lhs;
    return node;
}

ASTNode *ast_create_binary(ASTNodeType type, ASTNode *lhs, ASTNode *rhs, Token *tok) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    node->tok = tok;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

ASTNode *ast_create_num(Token *tok, u64 num) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = AST_NODE_NUM;
    node->tok = tok;
    node->u64_val = num;
    return node;
}

// TODO: strings are stored in .rodata, so we keep track of string literals and put the constant ones (e.g. printf("some string")) into .rodata and the writable ones (e.g. char *myStr = "some string") into a a writable part of the program
// as of me writing this, this function only returns a pointer node that points to nothing
ASTNode *ast_create_str(Token *tok, char *str) {

}

ASTNode *ast_create_deref(Token *tok, ASTNode *lhs) {
    return ast_create_unary(AST_NODE_DEREF, lhs, tok);
}

// array indexing is just a derefing an offsetted pointer
// e.g.
// ptrOrArray[5]
// is the same as
// *(ptrOrArray + 5)
ASTNode *ast_create_index(Token *tok, ASTNode *lhs, ASTNode *idx) {
    return ast_create_deref(tok, ast_create_binary(AST_NODE_ADD, lhs, idx, tok));
}








// parsing
Type *ast_parse_pointers(Token **out, Token *tok, Type *ty) {
    while (tokenizer_token_equals(tok, "*")) {
        ty = type_pointer_to(ty);
        tok = tok->next;
    }
    *out = tok;
    return ty;
}

// parses array dimensions
// e.g.
// [], [5], [2][]
Type *ast_arr_dims(Token **out, Token *tok, Type *ty) {
    while (tok) {
        if (!tokenizer_skip_token(&tok, "[")) {
            break;
        }
        ASTNode *arr_len;
        if (tokenizer_skip_token(&tok, "]")) {
            // unknown length ([])
            arr_len = 0;
        } else {
            // known length
            // e.g.
            // [123]
            arr_len = ast_parse_expr(&tok, tok);
            if (!tokenizer_skip_token(&tok, "]")) {
                error_tok(current_file, "Expected \"]\" after array dimension in array type", tok);
                exit(1);
            }
        }

        // TODO: evaluate array len at compile time, probably best to do it after all parts of the program have been loaded into memory (so not here, but... maybe do it here? idk)
        ty = type_array_of(ty, arr_len);
    }

    *out = tok;
    return ty;
}

// parses function parameters
// e.g.
// (), (int), (char, int, char*)
Type *ast_func_params(Token **out, Token *tok, Type *ty) {
    if (tokenizer_skip_token(&tok, "(")) {
        ty = type_func(ty);

        Type *param = ty;

        while (tok) {
            DeclAttr attr = {};
            Type *new_param = ast_decl(&tok, tok, &attr);

            // TODO: validate for disallowed qualifiers in "attr"
            // e.g. inline, typedef

            new_param = ast_type(&tok, tok, new_param);
            new_param->attr = attr;

            param->next_param = new_param;
            param = new_param;

            if (!tokenizer_skip_token(&tok, ",")) {
                break;
            }
        }

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" at end of parameter list in function type", tok);
            exit(1);
        }
    }

    *out = tok;
    return ty;
}

// parses the suffix part of a type
// array dimensions and function parameters
Type *ast_type_suffix(Token **out, Token *tok, Type *ty) {
    if (tokenizer_token_equals(tok, "[")) {
        ty = ast_arr_dims(&tok, tok, ty);
    } else if (tokenizer_token_equals(tok, "(")) {
        ty = ast_func_params(&tok, tok, ty);
    }
    *out = tok;
    return ty;
}

// parses the pointer, array, function, name (optional) part of a type
// e.g.
//     const char *myArray[5]
// parses this -> ~~~~~~~~~~~
// for parsing a complete type
// with specifiers / qualifiers "const"
// and the resulting type name (char, int, etc...)
// do something like:
// Type *ty = ast_decl(...)
// ty = ast_type(..., ty)
Type *ast_type(Token **out, Token *tok, Type *ty) {
    if (tokenizer_token_equals(tok, "struct")) {
        // TODO
        return 0;
    } else if (tokenizer_token_equals(tok, "union")) {
        // TODO
        return 0;
    }

    ty = ast_parse_pointers(&tok, tok, ty);

    if (tok->type == TOK_KEYWORD) {
        ty->tok = tok;
        tok = tok->next;
    } else if (tokenizer_token_equals(tok, "(")) {
        tok = tok->next;
        Type *inner_type = ast_type(&tok, tok, ty);

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" after parenthesized type", tok);
            exit(1);
        }

        inner_type->base = ty;
        ty = inner_type;
    }

    ty = ast_type_suffix(&tok, tok, ty);

    *out = tok;
    return ty;
}


// parses declaration
Type *ast_decl(Token **out, Token *tok, DeclAttr *attr) {
    // get qualifiers
    while (ast_is_qualifier(tok)) {
        if (tokenizer_token_equals(tok, "const")) {
            if (attr->is_const) {
                error_tok(current_file, "Declaration already declared \"const\"", tok);
                exit(1);
            }
            attr->is_const = true;
        } else if (tokenizer_token_equals(tok, "extern")) {
            if (attr->is_extern) {
                error_tok(current_file, "Declaration already declared \"extern\"", tok);
                exit(1);
            }
            attr->is_extern = true;
        } else if (tokenizer_token_equals(tok, "inline")) {
            if (attr->is_inline) {
                error_tok(current_file, "Declaration already declared \"inline\"", tok);
                exit(1);
            }
            attr->is_inline = true;
        } else if (tokenizer_token_equals(tok, "static")) {
            if (attr->is_static) {
                error_tok(current_file, "Declaration already declared \"static\"", tok);
                exit(1);
            }
            attr->is_static = true;
        } else {
            error_tok(current_file, "Token marked as \"qualifier\" but did not match any qualifiers in the list", tok);
            exit(1);
        }
        tok = tok->next;
    }

    // get the base type
    Type *base_ty = 0;
    if (tokenizer_token_equals(tok, "void")) {
        base_ty = ty_void;
    } else if (tokenizer_token_equals(tok, "bool")) {
        base_ty = ty_bool;
    } else if (tokenizer_token_equals(tok, "u8") || tokenizer_token_equals(tok, "char")) {
        base_ty = ty_u8;
    } else if (tokenizer_token_equals(tok, "u16")) {
        base_ty = ty_u16;
    } else if (tokenizer_token_equals(tok, "u32")) {
        base_ty = ty_u32;
    } else if (tokenizer_token_equals(tok, "u64")) {
        base_ty = ty_u64;
    } else if (tokenizer_token_equals(tok, "i8")) {
        base_ty = ty_i8;
    } else if (tokenizer_token_equals(tok, "i16")) {
        base_ty = ty_i16;
    } else if (tokenizer_token_equals(tok, "i32")) {
        base_ty = ty_i32;
    } else if (tokenizer_token_equals(tok, "i64")) {
        base_ty = ty_i64;
    } else if (tokenizer_token_equals(tok, "f32")) {
        base_ty = ty_f32;
    } else if (tokenizer_token_equals(tok, "f64")) {
        base_ty = ty_f64;
    } else {
        error_tok(current_file, "No base type", tok);
        exit(1);
    }
    tok = tok->next;

    // TODO: make a copy of base_ty?

    *out = tok;
    return base_ty;
}





// expects an expression and parses it
// uses pratt parsing and parses expressions as long as it can
// e.g. will parse this entire expression: x = 5 + 3 * 2 / 5
// this function is purely for readability and "scalability"
// it will always call the highest precedence operator to initiate the whole expression pratt parsing
// if another operator is added that has a higher precedence than ",", this function will call that function instead since its the new highest
// it also helps with reading, as reading "ast_parse_expr" clearly means "we are parsing an expression" where as "ast_parse_comma" could be confusing, as if were expecting it to be a comma
ASTNode *ast_parse_expr(Token **out, Token *tok) {
    ASTNode *node = ast_parse_comma(&tok, tok);
    *out = tok;
    return node;
}

// parses an expression statement
// literally just an expression followed by a semicolon
// used in the statement parser
ASTNode *ast_parse_expr_stmt(Token **out, Token *tok) {
    ASTNode *node = ast_parse_expr(&tok, tok);
    if (!tokenizer_skip_token(&tok, ";")) {
        error_tok(current_file, "Expected \";\" after expression", tok);
        exit(1);
    }
    *out = tok;
    return node;
}

// pratt parsing for each expression
// c operator precedence: https://en.cppreference.com/c/language/operator_precedence

// comma
// left to right
// multiple expressions but comma separated
// e.g. x = 5, y = 8;
// i have no idea why this even exists, aside from incrementing in for loops
// e.g.
// for (int i = 5, int j = 0; i < 10; i++, j++) { ... }
// a block statement could achieve the same effect but its a STATEMENT not an expression (does not return a value)
// but what if a block statement DID return a value...
// a comma expression returns the value produced by the last expression, so i++, j++ returns the value produced by j++
// a block statement should not produce any value
// but what if it did...
// what if {i++; j++;} returns the value produced by j++
// then it could work as a replacement
// right now i dont even care
// because i have to include the comma expression otherwise testing with popular C programs might fail due to me not having implemented the comma expression
ASTNode *ast_parse_comma(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_assign(&tok, tok);

    while (tok) {
        if (!tokenizer_token_equals(tok, ",")) {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_assign(&tok, tok);
        lhs = ast_create_binary(AST_NODE_COMMA, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// assignment
// right to left
// =, +=, -=, etc...
ASTNode *ast_parse_assign(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_tern(&tok, tok);

    while (tok) {
        ASTNodeType additional_operator;
        if (tokenizer_token_equals(tok, "=")) {
            additional_operator = AST_NODE_NONE;
        } else if (tokenizer_token_equals(tok, "+=")) {
            additional_operator = AST_NODE_ADD;
        } else if (tokenizer_token_equals(tok, "-=")) {
            additional_operator = AST_NODE_SUB;
        } else if (tokenizer_token_equals(tok, "*=")) {
            additional_operator = AST_NODE_MUL;
        } else if (tokenizer_token_equals(tok, "/=")) {
            additional_operator = AST_NODE_DIV;
        } else if (tokenizer_token_equals(tok, "%=")) {
            additional_operator = AST_NODE_MOD;
        } else if (tokenizer_token_equals(tok, "&=")) {
            additional_operator = AST_NODE_BAND;
        } else if (tokenizer_token_equals(tok, "|=")) {
            additional_operator = AST_NODE_BOR;
        } else if (tokenizer_token_equals(tok, "^=")) {
            additional_operator = AST_NODE_BXOR;
        } else if (tokenizer_token_equals(tok, "~=")) {
            additional_operator = AST_NODE_BNOT;
        } else if (tokenizer_token_equals(tok, "<<=")) {
            additional_operator = AST_NODE_LSHIFT;
        } else if (tokenizer_token_equals(tok, ">>=")) {
            additional_operator = AST_NODE_RSHIFT;
        } else {
            break;
        }

        tok = tok->next;

        ASTNode *rhs;

        if (additional_operator == AST_NODE_NONE) {
            rhs = ast_parse_tern(&tok, tok);
        } else {
            ASTNode *op_rhs = ast_parse_tern(&tok, tok);

            rhs = ast_create_binary(additional_operator, lhs, op_rhs, tok);
        }

        lhs = ast_create_binary(AST_NODE_ASSIGN, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// ternary
// right to left
// expr ? expr : expr
ASTNode *ast_parse_tern(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_lor(&tok, tok);

    if (tokenizer_token_equals(tok, "?")) {
        Token *start = tok;

        tok = tok->next;

        ASTNode *if_true = ast_parse_tern(&tok, tok);

        if (!tokenizer_skip_token(&tok, ":")) {
            error_tok(current_file, "Expected \":\" after true value in ternary", tok);
            exit(1);
        }

        ASTNode *if_false = ast_parse_tern(&tok, tok);

        ASTNode *tern = ast_create_empty(AST_NODE_TERN, start);
        tern->cond = lhs;
        tern->then = if_true;
        tern->els = if_false;

        lhs = tern;
    }

    *out = tok;
    return lhs;
}

// logical OR (||)
// left to right
ASTNode *ast_parse_lor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_land(&tok, tok);

    while (tok) {
        if (!tokenizer_token_equals(tok, "||")) {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_land(&tok, tok);

        lhs = ast_create_binary(AST_NODE_LOR, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// logical AND (&&)
// left to right
ASTNode *ast_parse_land(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_bor(&tok, tok);

    while (tok) {
        if (!tokenizer_token_equals(tok, "&&")) {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_bor(&tok, tok);

        lhs = ast_create_binary(AST_NODE_LAND, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// bitwise OR (|)
// left to right
ASTNode *ast_parse_bor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_bxor(&tok, tok);

    while (tok) {
        if (!tokenizer_token_equals(tok, "|")) {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_bxor(&tok, tok);

        lhs = ast_create_binary(AST_NODE_BOR, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// bitwise XOR (^)
// left to right
ASTNode *ast_parse_bxor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_band(&tok, tok);

    while (tok && tokenizer_token_equals(tok, "^")) {
        tok = tok->next;

        ASTNode *rhs = ast_parse_band(&tok, tok);

        lhs = ast_create_binary(AST_NODE_BXOR, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// bitwise AND (&)
// left to right
ASTNode *ast_parse_band(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_relational_eq(&tok, tok);

    while (tok && tokenizer_token_equals(tok, "&")) {
        tok = tok->next;

        ASTNode *rhs = ast_parse_relational_eq(&tok, tok);

        lhs = ast_create_binary(AST_NODE_BAND, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// relational: ==, !=
// left to right
// idk what else to name them
// poor naming scheme for "ast_parse_relational_eq" and "ast_parse_relational_diff"
ASTNode *ast_parse_relational_eq(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_relational_diff(&tok, tok);

    while (tok) {
        ASTNodeType relation_type;
        if (tokenizer_token_equals(tok, "==")) {
            relation_type = AST_NODE_EQ;
        } else if (tokenizer_token_equals(tok, "!=")) {
            relation_type = AST_NODE_NE;
        } else {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_relational_diff(&tok, tok);
        lhs = ast_create_binary(relation_type, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// relational: <, <=, >, >=
// left to right
ASTNode *ast_parse_relational_diff(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_shift(&tok, tok);

    while (tok) {
        ASTNodeType relation_type;
        if (tokenizer_token_equals(tok, "<")) {
            relation_type = AST_NODE_LT;
        } else if (tokenizer_token_equals(tok, "<=")) {
            relation_type = AST_NODE_LE;
        } else if (tokenizer_token_equals(tok, ">")) {
            relation_type = AST_NODE_GT;
        } else if (tokenizer_token_equals(tok, ">=")) {
            relation_type = AST_NODE_GE;
        } else {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_shift(&tok, tok);
        lhs = ast_create_binary(relation_type, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// bit shifting: <<, >>
// left to right
ASTNode *ast_parse_shift(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_add_sub(&tok, tok);

    while (tok) {
        ASTNodeType op_type;
        if (tokenizer_token_equals(tok, "<<")) {
            op_type = AST_NODE_LSHIFT;
        } else if (tokenizer_token_equals(tok, ">>")) {
            op_type = AST_NODE_RSHIFT;
        } else {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_add_sub(&tok, tok);
        lhs = ast_create_binary(op_type, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// addition, subtraction: +, -
// left to right
ASTNode *ast_parse_add_sub(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_mul_div_mod(&tok, tok);

    while (tok) {
        ASTNodeType op_type;
        if (tokenizer_token_equals(tok, "+")) {
            op_type = AST_NODE_ADD;
        } else if (tokenizer_token_equals(tok, "-")) {
            op_type = AST_NODE_SUB;
        } else {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_mul_div_mod(&tok, tok);
        lhs = ast_create_binary(op_type, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// mul, div, mod: *, /, %
// left to right
ASTNode *ast_parse_mul_div_mod(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_prefix(&tok, tok);

    while (tok) {
        ASTNodeType op_type;
        if (tokenizer_token_equals(tok, "*")) {
            op_type = AST_NODE_MUL;
        } else if (tokenizer_token_equals(tok, "/")) {
            op_type = AST_NODE_DIV;
        } else if (tokenizer_token_equals(tok, "%")) {
            op_type = AST_NODE_MOD;
        } else {
            break;
        }
        tok = tok->next;

        ASTNode *rhs = ast_parse_prefix(&tok, tok);
        lhs = ast_create_binary(op_type, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;

}

// prefix
// right to left
// ++value, --value, *value, &value, (typeCast)value, etc...
// since prefix is right to left (operator closest to value comes first), it uses pratt parsing on itself in order to achieve the right to left order
// e.g.
// &*ptr
// addr(deref(ptr))
// when you "read" the code, addr is called first, but since its pratt parsing, deref finishes execution first and returns a deref node, then addr receives the deref and create an addr node, resulting in the order: addr->deref->ptr (get addr of deref of ptr)
ASTNode *ast_parse_prefix(Token **out, Token *tok) {
    ASTNodeType op_type = AST_NODE_NONE;
    // TODO: for unary ++ and --
    if (tokenizer_token_equals(tok, "+")) {
        // unary "+" does absolutely nothing in most cases
        // it can perform integer promotion
        // e.g.
        // char myChar = 'a';
        // sizeof(myChar) // 1 byte (as expected)
        // sizeof(+myChar) // 4 bytes (myChar was converted to int)
        // it does NOT turn a negative number positive (e.g. x = -10, +x is still -10)
        // so i guess i should just repeat the logic of a type cast to int (e.g. +x logically becomes (int)x)
        // TODO: to whats stated above
        error_tok(current_file, "Unary \"+\" not implemented", tok);
        exit(1);
    } else if (tokenizer_token_equals(tok, "-")) {
        op_type = AST_NODE_NEG;
    } else if (tokenizer_token_equals(tok, "!")) {
        op_type = AST_NODE_LNOT;
    } else if (tokenizer_token_equals(tok, "~")) {
        op_type = AST_NODE_BNOT;
    } else if (tokenizer_token_equals(tok, "*")) {
        op_type = AST_NODE_DEREF;
    } else if (tokenizer_token_equals(tok, "&")) {
        op_type = AST_NODE_ADDR;
    } else {
        // alignof and sizeof
        if (tokenizer_token_equals(tok, "sizeof")) {
            tok = tok->next;
            error_tok(current_file, "\"sizeof\" not implemented", tok);
            exit(1);
        } else if (tokenizer_token_equals(tok, "_Alignof")) {
            tok = tok->next;
            error_tok(current_file, "\"_Alignof\" not implemented", tok);
            exit(1);
        }

        // non-unary (binary) ast nodes (x++ is the same as x += 1, which is an addition node and an assignment node)
        if (tokenizer_token_equals(tok, "++")) {
            op_type = AST_NODE_ADD;
        } else if (tokenizer_token_equals(tok, "--")) {
            op_type = AST_NODE_SUB;
        }

        if (op_type != AST_NODE_NONE) {
            // ++ or --
            // TODO: implement
        }

        // if parenthesis is found, check if its a cast, or just a regular parenthesis (e.g. 5 * (3 + 2))
        // if its a cast, parse the cast, otherwise proceed to next parsing step (postfix)
        bool is_cast = false;
        if (tokenizer_token_equals(tok, "(")) {
            // set "is_cast" to true, if cast was found
            // TODO: implement
        }

        // this is the "base case" for the recursive "ast_parse_prefix" call, if no more prefixes are found, then proceed to next parsing step (postfix)
        if (is_cast == false) {
            ASTNode *node = ast_parse_postfix(&tok, tok);
            *out = tok;
            return node;
        }

        tok = tok->next;

        // TODO: implement the ++, -- part thing
    }

    tok = tok->next;

    ASTNode *node = ast_create_unary(op_type, ast_parse_prefix(&tok, tok), tok);
    *out = tok;
    return node;
}

// postfix
// left to right
// value++, value--, someStruct.someMember, someStruct->someMember, funcCall(), arrayIndex[], etc...
ASTNode *ast_parse_postfix(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_primary(&tok, tok);
    note_tok(current_file, "Miau", tok);

    while (tok) {
        Token *op_tok = tok;
        tok = tok->next;
        if (tokenizer_token_equals(op_tok, "++")) {
            // x++
            // means use x first, then increment
            // which, in terms of compilation is difficult to implement because of order of operation
            // so logically
            // x++ is (x = x + 1) - 1
            // x += 1 returns the value of x after assignment, the -1 is used to get the original value of x
            // when code generating, this will be optimized away
            // there really is no better way of doing this without adding a whole lot of complexity and making everything more complicated

            // x++ is (x = x + 1) - 1
            // create the x + 1
            ASTNode *x_plus_1 = ast_create_binary(AST_NODE_ADD, lhs, ast_create_num(tok, 1), tok);
            // assign to x
            ASTNode *assign_x = ast_create_binary(AST_NODE_ASSIGN, lhs, x_plus_1, tok);
            // -1
            ASTNode *sub_x = ast_create_binary(AST_NODE_SUB, assign_x, ast_create_num(tok, 1), tok);

            lhs = sub_x;
        } else if (tokenizer_token_equals(op_tok, "--")) {
            // x-- is (x = x - 1) + 1
            // create the x - 1
            ASTNode *x_sub_1 = ast_create_binary(AST_NODE_SUB, lhs, ast_create_num(tok, 1), tok);
            // assign to x
            ASTNode *assign_x = ast_create_binary(AST_NODE_ASSIGN, lhs, x_sub_1, tok);
            // +1
            ASTNode *add_x = ast_create_binary(AST_NODE_ADD, assign_x, ast_create_num(tok, 1), tok);

            lhs = add_x;
        } else if (tokenizer_token_equals(op_tok, ".")) {

        } else if (tokenizer_token_equals(op_tok, "->")) {

        } else if (tokenizer_token_equals(op_tok, "[")) {
            ASTNode *idx = ast_parse_expr(&tok, tok);
            lhs = ast_create_index(tok, lhs, idx);
        } else if (tokenizer_token_equals(op_tok, "(")) {
            // function call
            // read arguments nodes and store them in the resulting AST_NODE_FUNCALL node
            ASTNode *funcall = ast_create_empty(AST_NODE_FUNCALL, tok);
            if (!tokenizer_skip_token(&tok, ")")) {
                ASTNode *cur_arg = funcall;
                while (tok) {
                    ASTNode *arg = ast_parse_expr(&tok, tok);
                    cur_arg->next_arg = arg;
                    cur_arg = arg;
                    if (tokenizer_skip_token(&tok, ")")) {
                        break;
                    }
                    if (!tokenizer_skip_token(&tok, ",")) {
                        error_tok(current_file, "Expected \")\" to close function call or \",\" between arguments", tok);
                        exit(1);
                    }
                }
            }
        } else {
            break;
        }
    }

    *out = tok;
    return lhs;
}

// primary (meaning something that immediately represents a value)
// variable, "string", "char", 123, true, false, null
// compound literals also go here even through they are technically in the postfix
// i just dont think it makes sense that it should be a postfix... a postfix should add to some existing value... but as far as i understand compound literals, they are a value by themselves and does not add to an existing value
// are they a part of postfix because of type casts? i dont know, i dont care (as of me writing this)
ASTNode *ast_parse_primary(Token **out, Token *tok) {
    ASTNode *node;
    if (tok->type == TOK_NUM) {
        node = ast_create_num(tok, tok->u64_val);
    } else if (tok->type == TOK_STR) {
        node = ast_create_str(tok, tok->str_data);
    } else if (tok->type == TOK_KEYWORD) {
        // check if its NOT a keyword, that means its a variable
        if (tokenizer_token_equals(tok, "struct")) {
            // TODO: implement
        }
        if (tokenizer_token_equals(tok, "union")) {
            // TODO: implement
        }
        if (tokenizer_token_equals(tok, "enum")) {
            // TODO: implement
        }
        
        // if matched with any other keyword that does not have any logic here, its incorrect
        if (tokenizer_get_keyword(tok)) {
            error_tok(current_file, "Unexpected keyword", tok);
            exit(1);
        }

        node = ast_create_empty(AST_NODE_VAR, tok);
        node->var = (ASTObj *)1; // TODO: lookup variable
    } else {
        error_tok(current_file, "Invalid primary expression", tok);
        exit(1);
    }

    *out = tok;
    return node;
}







// statement parsing
// parses a block statement
// "{" statements... "}"
ASTNode *ast_parse_block_stmt(Token **out, Token *tok) {
    if (!tokenizer_token_equals(tok, "{")) {
        error_tok(current_file, "Expected \"{\" at start of block statement", tok);
        exit(1);
    }

    ASTNode *block_stmt = ast_create_empty(AST_NODE_BLOCK, tok);
    tok = tok->next;

    ASTNode head_stmt = {};
    ASTNode *cur_stmt = &head_stmt;
    while (tok) {
        if (tokenizer_skip_token(&tok, "}")) {
            break;
        }

        ASTNode *stmt = ast_parse_stmt(&tok, tok);

        cur_stmt->next = stmt;
        cur_stmt = stmt;
    }

    block_stmt->body = head_stmt.next;

    return block_stmt;
}


// parses a statement
ASTNode *ast_parse_stmt(Token **out, Token *tok) {
    ASTNode *node = 0;

    if (tokenizer_token_equals(tok, "if")) {
        node = ast_create_empty(AST_NODE_IF, tok);
        tok = tok->next;

        if (!tokenizer_skip_token(&tok, "(")) {
            error_tok(current_file, "Expected \"(\" before condition in \"if\"", tok);
            exit(1);
        }

        node->cond = ast_parse_expr(&tok, tok);

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" after condition in \"if\"", tok);
            exit(1);
        }

        node->then = ast_parse_stmt(&tok, tok);

        if (tokenizer_token_equals(tok, "else")) {
            node->els = ast_parse_stmt(&tok, tok);
        }
    } else if (tokenizer_token_equals(tok, "for")) {
        node = ast_create_empty(AST_NODE_FOR, tok);
        tok = tok->next;

        if (!tokenizer_skip_token(&tok, "(")) {
            error_tok(current_file, "Expected \"(\" before loop setup in \"for\"", tok);
            exit(1);
        }

        node->init = ast_parse_expr_stmt(&tok, tok);
        node->cond = ast_parse_expr_stmt(&tok, tok);
        node->inc = ast_parse_expr(&tok, tok); // not a statement since its the last part of the loop setup and therefore doesnt contain a semicolon

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" after loop setup in \"for\"", tok);
            exit(1);
        }

        node->then = ast_parse_stmt(&tok, tok);
    } else if (tokenizer_token_equals(tok, "while")) {
        node = ast_create_empty(AST_NODE_FOR, tok);
        tok = tok->next;

        if (!tokenizer_skip_token(&tok, "(")) {
            error_tok(current_file, "Expected \"(\" before condition in \"while\"", tok);
            exit(1);
        }

        node->cond = ast_parse_expr(&tok, tok);

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" after condition in \"while\"", tok);
            exit(1);
        }

        node->then = ast_parse_stmt(&tok, tok);
    } else if (tokenizer_token_equals(tok, "do")) {
        node = ast_create_empty(AST_NODE_DO, tok);
        tok = tok->next;

        node->then = ast_parse_stmt(&tok, tok);

        // no longer a regular "do", now its a "do while"
        if (tokenizer_skip_token(&tok, "while")) {
            if (!tokenizer_skip_token(&tok, "(")) {
                error_tok(current_file, "Expected \"(\" before condition in \"do while\"", tok);
                exit(1);
            }

            node->cond = ast_parse_stmt(&tok, tok);

            if (!tokenizer_skip_token(&tok, ")")) {
                error_tok(current_file, "Expected \")\" after condition in \"do while\"", tok);
                exit(1);
            }
        }
    } else if (tokenizer_token_equals(tok, "switch")) {
        node = ast_create_empty(AST_NODE_SWITCH, tok);
        tok = tok->next;

        if (!tokenizer_skip_token(&tok, "(")) {
            error_tok(current_file, "Expected \"(\" before compare value in \"switch\"", tok);
            exit(1);
        }

        node->cmp_val = ast_parse_expr(&tok, tok);

        if (!tokenizer_skip_token(&tok, ")")) {
            error_tok(current_file, "Expected \")\" after compare value in \"switch\"", tok);
            exit(1);
        }

        if (!tokenizer_skip_token(&tok, "{")) {
            error_tok(current_file, "Expected \"{\" before cases in \"switch\"", tok);
            exit(1);
        }

        // TODO: push switch to stack
    } else if (tokenizer_token_equals(tok, "case")) {
        // TODO: check if we are in a switch, if no switch is found, throw error

        // the way cases work is a bit different
        // logically, each case just defines a label to jump to
        // the code in a case is not in its own scope
        // think of it like plain assembly
        // each case is a label that the program can jump to
        // no scopes, no overhead
        // just plain labels
        // so how do we know which switch this case belongs to?
        // its simple, when walking through the code, eventually we encounter a switch, then we store that switch in a switch stack, and when we encounter a case, the top of the stack (the last switch added), is the switch this case belongs to
        // i might change it later, but right now i feel like this works just fine
        node = ast_create_empty(AST_NODE_CASE, tok);
        tok = tok->next;

        // i dont get why C doesnt accept comma expressions here
        // now i get how switch works, and that it NEEDS constant expressions (a constant value that does not execute any logic)
        // but we have already implemented a nice parser function for comma separated expressions
        // which is much nicer and more readable (in my opinion)
        // instead of having to type out each case by hand
        // case 1:
        // case 2:
        // case 3:
        // etc...
        // proper case in C:
        /*
        case 1:
        case 2:
        case 3:
            // do something
            break;
        */
        // you CANNOT do:
        // case 1, 2, 3:
        // for whatevery reason
        // should i implement it maybe in my own version?
        // TODO: come back to this later
        node->cmp_val = ast_parse_expr(&tok, tok); // TODO: make sure, when validating code later, that the expression here is CONSTANT

        if (!tokenizer_skip_token(&tok, ":")) {
            error_tok(current_file, "Expected \":\" after case value in \"case\"", tok);
            exit(1);
        }
    } else if (tokenizer_token_equals(tok, "}")) {
        // TODO: check if switch stack is not empty, then pop switch
    } else if (tokenizer_token_equals(tok, "{")) {
        node = ast_parse_block_stmt(&tok, tok);
    } else if (tokenizer_token_equals(tok, "return")) {
        node = ast_create_empty(AST_NODE_RET, tok);
        tok = tok->next;

        node->lhs = ast_parse_expr(&tok, tok);

        if (!tokenizer_skip_token(&tok, ";")) {
            error_tok(current_file, "Expected \";\" return value in \"return\"", tok);
            exit(1);
        }
    } else {
        node = ast_parse_expr_stmt(&tok, tok);
    }

    *out = tok;
    return node;
}

bool ast_is_function(Token *tok) {
    if (tokenizer_token_equals(tok->next, "(")) {
        return true;
    }
    return false;
}

void ast_parse_global_var(Token **out, Token *tok, Type *ty, DeclAttr *attr) {
    ASTObj *var;



    var->next = globals;
    globals = var;
    *out = tok;
}

void ast_parse_global_func(Token **out, Token *tok, Type *ty, DeclAttr *attr) {
    ASTObj *func = ast_create_obj(ty, attr);
    func->body = ast_parse_block_stmt(&tok, tok);



    func->next = globals;
    globals = func;
    *out = tok;
}

void ast_print_type(Type *ty) {
    while (ty) {
        char *ty_str = 0;
        switch (ty->type) {
            case TY_NONE:
                ty_str = "NONE";
                break;
            case TY_VOID:
                ty_str = "VOID";
                break;
            case TY_BOOL:
                ty_str = "BOOL";
                break;
            case TY_INT8:
                ty_str = "INT8";
                break;
            case TY_INT16:
                ty_str = "INT16";
                break;
            case TY_INT32:
                ty_str = "INT32";
                break;
            case TY_INT64:
                ty_str = "INT64";
                break;
            case TY_F32:
                ty_str = "F32";
                break;
            case TY_F64:
                ty_str = "F64";
                break;
            case TY_PTR:
                ty_str = "PTR";
                break;
            case TY_ARRAY:
                ty_str = "ARRAY";
                break;
            case TY_FUNC:
                ty_str = "FUNC";
                break;
            case TY_ENUM:
                ty_str = "ENUM";
                break;
            case TY_STRUCT:
                ty_str = "STRUCT";
                break;
            case TY_UNION:
                ty_str = "UNION";
                break;
            default:
                printf("Corrupt type\n");
                exit(1);
        }

        printf("%s", ty_str);

        if (ty->base) {
            printf(" -> ");
            ty = ty->base;
        } else {
            break;
        }
    }

    printf("\n");
}

void ast_parse_file(File *file, Token *tok) {
    current_file = file;

    // set globals as a nullptr to indicate end of linked list
    globals = 0;

    while (tok) {
        DeclAttr attr = {};
        Type *ty = ast_decl(&tok, tok, &attr);
        ty = ast_type(&tok, tok, ty);

        ast_print_type(ty);

        if (attr.is_typedef) {
            // TODO: add typedef
            continue;
        }

        if (ty->type == TY_FUNC) {
            // parse function and store it globally
            ast_parse_global_func(&tok, tok, ty, &attr);
        } else {
            // parse any other variable, typedef. basically anything that is not a function
            ast_parse_global_var(&tok, tok, ty, &attr);
        }
    }
}