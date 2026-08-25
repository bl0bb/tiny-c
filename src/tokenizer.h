#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "types.h"

typedef enum {
    TOK_NONE = 0,
    TOK_NUM,
    TOK_STR,
    TOK_KEYWORD, // or variable
    TOK_PUNCT,
} TokenType;

typedef struct {
    TokenType type;

    char *start;
    u16 len;

    // if token is a string literal (e.g. "abcdefg"), then store the null-terminated string here
    char *str_value;

    // stored as u64, simply because its the largest datatype (along with f64 and i64 but to prevent confusion i use u64 because i interpret it both as unsigned 64 bit number and "raw data that can be interpreted in multiple ways")
    // it stores the raw data of the number it parsed
    // this could be a negative 64 bit number
    // a float (f32)
    // a double (f64)
    u64 num_val;

    // next token
    Token *next;
} Token;

#endif