#include <ctype.h>

#include "tokenizer.h"
#include "file.h"


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
};

// send in a reference of a pointer, and it will increment the referenced pointer as long as the current character is whitespace and not null
void skip_whitespce(char **ptr) {
    while (**ptr && isspace(**ptr)) *ptr++;
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

    while (true) {
        skip_whitespace(&code_ptr);
        if (!*code_ptr) break;




        // actual token parsing
        // initialize token with type = TOK_NONE, so that the rest of the members are initialized to some value instead of being garbage
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

            tok.num_val = num_val;
        } else {
            u8 punct_idx = tokenizer_get_punct();
        }

        code_ptr = new_code_ptr;


        tail->next = malloc(sizeof(Token *));
        *tail->next = tok;
        tail = tail->next;
    }

    return head.next;
}