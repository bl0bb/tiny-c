#ifndef LOG_H
#define LOG_H

#include "types.h"
#include "file.h"
#include "tokenizer.h"

// position
void error_at(File *current_file, char *message, char *start, i32 length);
void warn_at(File *current_file, char *message, char *start, i32 length);
void note_at(File *current_file, char *message, char *start, i32 length);

// token
void error_tok(File *current_file, char *message, Token *tok);
void warn_tok(File *current_file, char *message, Token *tok);
void note_tok(File *current_file, char *message, Token *tok);

#endif