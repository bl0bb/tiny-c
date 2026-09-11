#ifndef AST_H
#define AST_H

#include "tokenizer.h"
#include "type.h"











bool ast_is_qualifier(Token *tok);

ASTNode *ast_create_empty(ASTNodeType type, Token *tok);
ASTNode *ast_create_unary(ASTNodeType type, ASTNode *lhs, Token *tok);
ASTNode *ast_create_binary(ASTNodeType type, ASTNode *lhs, ASTNode *rhs, Token *tok);

ASTNode *ast_create_num(Token *tok, u64 num);
ASTNode *ast_create_str(Token *tok, char *str);

Type *ast_parse_pointers(Token **out, Token *tok, Type *ty);
Type *ast_arr_dims(Token **out, Token *tok, Type *ty);
Type *ast_func_params(Token **out, Token *tok, Type *ty);
Type *ast_type_suffix(Token **out, Token *tok, Type *ty);
Type *ast_type(Token **out, Token *tok, Type *ty);
Type *ast_decl(Token **out, Token *tok, DeclAttr *attr);

ASTNode *ast_parse_expr(Token **out, Token *tok);
ASTNode *ast_parse_expr_stmt(Token **out, Token *tok);
ASTNode *ast_parse_comma(Token **out, Token *tok);
ASTNode *ast_parse_assign(Token **out, Token *tok);
ASTNode *ast_parse_tern(Token **out, Token *tok);
ASTNode *ast_parse_lor(Token **out, Token *tok);
ASTNode *ast_parse_land(Token **out, Token *tok);
ASTNode *ast_parse_bor(Token **out, Token *tok);
ASTNode *ast_parse_bxor(Token **out, Token *tok);
ASTNode *ast_parse_band(Token **out, Token *tok);
ASTNode *ast_parse_relational_eq(Token **out, Token *tok);
ASTNode *ast_parse_relational_diff(Token **out, Token *tok);
ASTNode *ast_parse_shift(Token **out, Token *tok);
ASTNode *ast_parse_add_sub(Token **out, Token *tok);
ASTNode *ast_parse_mul_div_mod(Token **out, Token *tok);
ASTNode *ast_parse_prefix(Token **out, Token *tok);
ASTNode *ast_parse_postfix(Token **out, Token *tok);
ASTNode *ast_parse_primary(Token **out, Token *tok);

ASTNode *ast_parse_block_stmt(Token **out, Token *tok);
ASTNode *ast_parse_stmt(Token **out, Token *tok);

void ast_parse_file(File *file, Token *tok);



#endif