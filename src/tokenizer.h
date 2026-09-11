#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "tinyc.h"
#include "types.h"
#include "file.h"

Token *tokenizer_tokenize(File *file);
bool tokenizer_token_equals(Token *tok, const char *str);
bool tokenizer_skip_token(Token **tok, char *str);
i32 tokenizer_get_keyword(Token *tok);

#endif