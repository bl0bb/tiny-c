#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "types.h"
#include "file.h"

typedef enum {
    TOK_NONE = 0,
    TOK_NUM,
    TOK_STR,
    TOK_CHAR,
    TOK_KEYWORD, // or variable
    TOK_PUNCT,
} TokenType;



typedef struct Token Token;
struct Token {
    TokenType type;

    char *start;
    u16 len;




    // data for each token type:

    union {
        // string literal (e.g. "abcdefg"), then store the null-terminated string here
        struct {
            char *str_data;
        };

        // char literal (e.g. 'a', '\0', '\x36')
        struct {
            char char_val;
        };

        // number literal
        struct {
            // for easy access
            // im unsure about when C converts number to the desired type and when it doesnt
            // e.g. putting f64 into an u64 would convert the float to an unsigner integer right? and modify the bytes to achieve the conversion?
            // so im just adding each """type""" of number (unsigned, signed, floating point) to make it more readable and prevent potential bugs
            union {
                u64 u64_val;
                i64 i64_val;
                f64 f64_val;
            };
        };

        // punct
        struct {
            // stores the index of the punct found in TOKENIZER_PUNCTS
            i8 punct_idx;
        };
    };



    // next token
    Token *next;
};

Token *tokenizer_tokenize(File *file);

#endif