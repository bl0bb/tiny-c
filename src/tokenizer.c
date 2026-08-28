#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "file.h"
#include "array.h"
#include "string.h"
#include "log.h"


/*
certain punctuations are more than one character, so we need to define them to:
1. prevent the compiler from misunderstanding our code (e.g. && can mean logical AND or a bitwise AND operation on a memory address: someNumber&&someVariable)
2. make it easier to identiy and group correct and incorrect punctuations
*/
// TODO: make sure theres no conflicting order between them, meaning if two puncts start with the same string, put the long one first to prevent it from tweaking out
// e.g. "+" being before "+=" makes it interpret it as them being separate: "+" and "=" instead of together "+="
const char *TOKENIZER_PUNCTS[] = {
    // arith assign
    "+=", "-=", "*=", "/=",
    // inc, dec
    "++", "--",
    // bit op assign
    "<<=", ">>=", "&=", "|=", "^=",
    // mod
    "%=",
    // comp
    "==", "!=", "<=", ">=",
    // pointer struct member access
    "->",
    // logical and or
    "&&", "||",
    // bit shift
    "<<", ">>",
    // variadic function
    "...",
    // token parsing operator (used in macros)
    "##",

    // single char
    // logical not
    "!",
    // string literal
    "\"",
    // char literal
    "\'",
    // plus
    "+",
    // minus
    "-",
    // mul, ptr / deref
    "*",
    // div
    "/",
    // struct member
    ".",
    // statement separator
    ",",
    // function, type cast, operation grouping / ordering
    "(",
    ")",
    // array
    "[",
    "]",
    // block
    "{",
    "}",
    // preprocess
    "#",
    // semicolon
    ";",
};

// send in a reference of a pointer, and it will increment the referenced pointer as long as the current character is whitespace and not null
void skip_whitespace(char **ptr) {
    while (**ptr && isspace(**ptr)) (*ptr)++;
}

// returns index of the punct found in TOKENIZER_PUNCTS, returns -1 if no punct was found
i8 tokenizer_get_punct(char **code_ptr) {
    for (u8 i = 0; i < ARRAY_SIZE(TOKENIZER_PUNCTS); i++) {
        if (str_starts_with(*code_ptr, TOKENIZER_PUNCTS[i])) {
            *code_ptr += strlen(TOKENIZER_PUNCTS[i]);
            return i;
        }
    }
    return -1;
}

bool tokenizer_parse_num_base10(char **code_ptr, u64 *out) {
    bool valid = true;
    *out = 0;
    while (**code_ptr) {
        // for simplicity, both characters and numbers are recognized as part of the number
        if (!isalnum(**code_ptr)) {
            break;
        }
        if (isdigit(**code_ptr)) {
            *out *= 10;
            *out += **code_ptr - '0';
        }
        (*code_ptr)++;
    }

    return true;
}

bool tokenizer_parse_num(char **code_ptr, u64 *out) {
    // TODO: add multiple parsing types (hex, octal, binary, etc)
    if (**code_ptr == '.') {
        printf("Decimal parsing not implemented\n");
        exit(1);
    }
    return tokenizer_parse_num_base10(code_ptr, out);
}

// parses a singular character
// either regular (e.g. a, b, c, 1, 2, 3, %, &, !)
// an encoded char (e.g. \0, \x36)
// special builtin encodings (e.g. \n, \r, \t, \')
bool tokenizer_parse_char_literal_raw(char **code_ptr, char *out) {
    if (**code_ptr == '\\') {
        // encoded char
        char encoded_ch = (*code_ptr)[1];
        if (!encoded_ch) {
            return false;
        }
        switch (encoded_ch) {
            case 'a': // alert / bell
                *out = 7;
                break;
            case 'b': // backspace
                *out = 8;
                break;
            case 'f': // form feed
                *out = 12;
                break;
            case 'n': // newline (line feed)
                *out = 10;
                break;
            case 'r': // carriage return
                *out = 13;
                break;
            case 't': // horizontal tab
                *out = 9;
                break;
            case 'v': // vertical tab
                *out = 11;
                break;
            case '\\': // backslash
                *out = 92;
                break;
            case '\'': // single quote
                *out = 39;
                break;
            case '"': // double quote
                *out = 34;
                break;
            case '?': // question mark
                *out = 63;
                break;
            default:
                return false;
        }
        return true;
    }
    *out = **code_ptr;
    return true;
}

// uses the above function to parse the char
// pbut checks that the char literal itself is valid
// meaning it check that its only one char (e.g. NOT 'abc')
// and checks for a closing (')
bool tokenizer_parse_char_literal(char **code_ptr, char *out) {
    (*code_ptr)++; // skip first (')
    if (**code_ptr == '\'') {
        return false;
    }
    bool valid = tokenizer_parse_char_literal_raw(code_ptr, out);
    // TODO: simplify to
    // **code_ptr == '\'';
    // or keep as is to add error handling or something
    if (**code_ptr != '\'') {
        return false;
    }
    (*code_ptr)++; // skip closing (')
    return true;
}

bool tokenizer_parse_string_literal(char **code_ptr, char **out) {
    (*code_ptr)++; // skip first (")

    u32 str_length = 0;
    *out = malloc(sizeof(char) * (str_length + 1));
    (*out)[str_length] = 0;
    while (true) {
        if (!**code_ptr) {
            return false;
        }
        if (**code_ptr == '"') {
            (*code_ptr)++; // skip closing (")
            return true;
        }
        char ch;
        bool char_valid = tokenizer_parse_char_literal_raw(code_ptr, &ch);
        if (!char_valid) {
            return false;
        }
        (*out)[str_length++] = ch;
        *out = malloc(sizeof(char) * (str_length + 1));
        (*out)[str_length] = 0;

        (*code_ptr)++;
    }
}

Token tokenizer_create_token(char *start_ptr, char *end_ptr, TokenType type) {
    return (Token) {
        .start = start_ptr,
            .len = end_ptr - start_ptr,
            .type = type,
    };
}

Token *tokenizer_tokenize(File *file) {
    Token head = {};
    Token *tail = &head;

    char *code_ptr = file->data;
    // u32 code_len = strlen(code_ptr);
    // char *code_end = code_ptr + code_len - 1;

    while (true) {
        skip_whitespace(&code_ptr);
        if (!*code_ptr) break;


        // single line comment
        if (str_starts_with(code_ptr, "//")) {
            while (*code_ptr && *code_ptr != '\n') {
                code_ptr++;
            }
            code_ptr++; // skip the newline
            continue;
        }
        // multi line comment
        if (str_starts_with(code_ptr, "/*")) {
            while (*code_ptr && code_ptr[1] && !str_starts_with(code_ptr, "*/")) {
                code_ptr++;
            }
            code_ptr++; // skip the "*"
            if (!*code_ptr) break;
            code_ptr++; // skip the "/"
            continue;
        }

        // preprocess
        if (*code_ptr == '#') {
            // TODO: add preprocessing
        }


        // actual token parsing
        // initialize token with type = TOK_NONE, so that the rest of the members are initialized to some value instead of being garbage
        // but it doesnt matter since the token is being always being replaced and not updated
        Token tok = {
            .type = TOK_NONE,
        };

        char *new_code_ptr = code_ptr;

        if (isalpha(*new_code_ptr) || *new_code_ptr == '_') {
            // keyword or variable
            // (A-Z) (a-z) (_)

            // numbers are allowed in variable names, just not the first character
            // so use alnum which checks for (A-Z) (a-z) (0-9) instead of just (A-Z) (a-z)
            while (isalnum(*new_code_ptr) || *new_code_ptr == '_') {
                new_code_ptr++;
            }

            tok = tokenizer_create_token(code_ptr, new_code_ptr, TOK_KEYWORD);
        } else if (isdigit(*new_code_ptr) || *new_code_ptr == '.') {
            // number
            // (0-9) (.)

            u64 num_val;
            bool valid = tokenizer_parse_num(&new_code_ptr, &num_val);
            if (!valid) {
                error_at(file, "Invalid number", new_code_ptr, 1);
                exit(1);
            }

            tok = tokenizer_create_token(code_ptr, new_code_ptr, TOK_NUM);
            tok.u64_val = num_val;
        } else if (*new_code_ptr == '"') {
            // string literal
            // "TEXT GOES HERE"

            char *str_data;
            bool valid = tokenizer_parse_string_literal(&new_code_ptr, &str_data);
            if (!valid) {
                error_at(file, "Invalid string literal", code_ptr, 1);
                exit(1);
            }

            tok = tokenizer_create_token(code_ptr, new_code_ptr, TOK_STR);
            tok.str_data = str_data;
        } else if (*new_code_ptr == '\'') {
            // char literal
            // 'CHAR GOES HERE'

            char ch;
            bool valid = tokenizer_parse_char_literal(&new_code_ptr, &ch);
            if (!valid) {
                error_at(file, "Invalid char literal", new_code_ptr, 1);
                exit(1);
            }

            tok = tokenizer_create_token(code_ptr, new_code_ptr, TOK_CHAR);
            tok.char_val = ch;
        } else {
            i8 punct_idx = tokenizer_get_punct(&new_code_ptr);

            if (punct_idx != -1) {
                tok = tokenizer_create_token(code_ptr, new_code_ptr, TOK_PUNCT);
                tok.punct_idx = punct_idx;
            } else {
                error_at(file, "Unknown token", new_code_ptr, 1);
                exit(1);
            }
        }

        code_ptr = new_code_ptr;

        tail->next = malloc(sizeof(Token));
        *tail->next = tok;
        tail = tail->next;
    }

    return head.next;
}