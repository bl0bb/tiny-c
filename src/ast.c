#include "ast.h"

// expects an expression and parses it
// uses pratt parsing and parses expressions as long as it can
// e.g. will parse this entire expression: x = 5 + 3 * 2 / 5
void ast_parse_expr(Token **out, Token *tok) {

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

            rhs = ast_create_binary(additional_operator, lhs, op_rhs);
        }

        lhs = ast_create_binary(AST_NODE_ASSIGN, lhs, rhs, tok);
    }

    *out = tok;
    return lhs;
}

// ternary
// right to left
// expr ? expr : expr
// TODO: i forgot ternary was right to left, so its left to right at the moment, so make it right to left
ASTNode *ast_parse_tern(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_lor(&tok, tok);

    while (tok) {
        if (!tokenizer_token_equals(tok, "?")) {
            break;
        }
        Token *start = tok;

        tok = tok->next;

        ASTNode *if_true = ast_parse_lor(&tok, tok);

        if (!tokenizer_skip_token(&tok, ":")) {
            printf("Expected \":\"\n");
            exit(1);
        }

        ASTNode *if_false = ast_parse_lor(&tok, tok);

        ASTNode *tern = ast_create(AST_NODE_TERN, start);
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
        lhs = ast_create_binary(relation_type, lhs, rhs);
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
        lhs = ast_create_binary(relation_type, lhs, rhs);
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
        lhs = ast_create_binary(op_type, lhs, rhs);
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
        lhs = ast_create_binary(op_type, lhs, rhs);
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
        lhs = ast_create_binary(op_type, lhs, rhs);
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
    ASTNode *lhs = ast_parse_postfix(&tok, tok);

    ASTNodeType op_type;
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
            printf("\"sizeof\" not implemented\n");
            exit(1);
        } else if (tokenizer_token_equals(tok, "_Alignof")) {
            printf("\"_Alignof\" not implemented\n");
            exit(1);
        }

        // non-unary ast nodes (x++ is the same as x += 1, which is an addition node, and an assignment node)
        if (tokenizer_token_equals(tok, "++")) {
            op_type = AST_NODE_ADD;
        } else if (tokenizer_token_equals(tok, "--")) {
            op_type = AST_NODE_SUB;
        }
        printf("Unexpected token in primary\n");
        exit(1);
        if (tokenizer_token_equals(tok, "(")) {
            // if parenthesis is found, check if its a cast, or just a regular parenthesis (e.g. 5 * (3 + 2))
        }
    }


}

// postfix
// left to right
// value++, value--, someStruct.someMember, someStruct->someMember, funcCall(), arrayIndex[], etc...
ASTNode *ast_parse_postfix(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_primary(&tok, tok);

}

// primary (meaning something that immediately represents a value)
// variable, "string", "char", 123, true, false, null
// compound literals also go here even through they are technically in the postfix
// i just dont think it makes sense that it should be a postfix... a postfix should add to some existing value... but as far as i understand compound literals, they are a value by themselves and does not add to an existing value
ASTNode *ast_parse_primary(Token **out, Token *tok) {

}