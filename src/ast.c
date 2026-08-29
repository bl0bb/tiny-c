#include "ast.h"

// expects an expression and parses it
// uses pratt parsing and parses expressions as long as it can
// e.g. will parse this entire expression: x = 5 + 3 * 2 / 5
void ast_parse_expr(Token **out, Token *tok) {

}

// pratt parsing for each expression
// c operator precedence: https://en.cppreference.com/c/language/operator_precedence

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

    while (tok && tokenizer_token_equals(tok, "<="))

    return lhs;
}

// assignment
// =, +=, -=, etc...
ASTNode *ast_parse_assign(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_tern(&tok, tok);
    
}

// ternary
// expr ? expr : expr
ASTNode *ast_parse_tern(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_lor(&tok, tok);
    
}

// logical OR (||)
ASTNode *ast_parse_lor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_land(&tok, tok);
    
}

// logical AND (&&)
ASTNode *ast_parse_land(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_bor(&tok, tok);
    
}

// bitwise OR (|)
ASTNode *ast_parse_bor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_bxor(&tok, tok);
    
}

// bitwise XOR (^)
ASTNode *ast_parse_bxor(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_band(&tok, tok);
    
}

// bitwise AND (&)
ASTNode *ast_parse_band(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_relational_eq(&tok, tok);
    
}

// poor naming scheme for "ast_parse_relational_eq" and "ast_parse_relational_diff"
// idk what else to name them
// relational: ==, !=
ASTNode *ast_parse_relational_eq(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_relational_diff(&tok, tok);
    
}

// relational: <, <=, >, >=
ASTNode *ast_parse_relational_diff(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_add_sub(&tok, tok);
    
}

// addition, subtraction: +, -
ASTNode *ast_parse_add_sub(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_mul_div_mod(&tok, tok);
    
}

// mul, div, mod: *, /, %
ASTNode *ast_parse_mul_div_mod(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_prefix(&tok, tok);
    
}

// prefix
// ++value, --value, *value, &value, (typeCast)value, etc...
ASTNode *ast_parse_prefix(Token **out, Token *tok) {
    ASTNode *lhs = ast_parse_postfix(&tok, tok);
    
}

// postfix
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