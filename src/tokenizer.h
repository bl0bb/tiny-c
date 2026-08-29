#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "types.h"
#include "file.h"

typedef enum {
    TOK_NONE = 0,
    TOK_NUM, // since chars and numbers are "basically the same" (both are numerical values), they are treated as numbers
    TOK_STR,
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

        // number literal (e.g. 1, 0b0101, 0x36, 0.4f, .3)
        // or char literal (e.g. 'a', '\0', '\x36')
        // since chars are numerical values, there is no point in making separate logic for chars and numbers when they behave "the same" under the hood
        // this is fine unless for type checking in strict languages (like C++) or maybe ill implement something later
        // then it wont know if a number token is really a number or a char
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