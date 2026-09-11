#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#include "tinyc.h"
#include "file.h"
#include "tokenizer.h"
#include "ast.h"

// using long because the size of long depends on the target architecture (32 bit or 64 bit)
// TODO: maybe i should reevaluate my use of u64 and i64...
long get_file_length(FILE *file) {
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);
    return length;
}

i32 main(i32 argc, char *argv[]) {
    printf("Hello, World!\n");

    char *file_path = "tests/test.c";

    FILE *file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("Unable to open file");
        return 1;
    }

    long length = get_file_length(file);

    // allocate memory (+1 for null terminator)
    char *buffer = malloc(sizeof(char) * (length + 1));
    // if (buffer == NULL) {
    //     perror("Memory allocation failed");
    //     fclose(file);
    //     return 1;
    // }

    // read file into buffer
    fread(buffer, 1, length, file);
    buffer[length] = '\0';  // Null-terminate string
    fclose(file);

    printf("File length: %d\n", length);
    printf("File contents:\n%s\n", buffer);

    File main_file = {
        .path = file_path,
        .data = buffer,
    };

    Token *head = tokenizer_tokenize(&main_file);

    Token *cur_tok = head;
    while (cur_tok) {
        char *type_str;
        switch (cur_tok->type) {
            case TOK_NONE:
                type_str = "NONE";
                break;
            case TOK_NUM:
                type_str = "NUM";
                break;
            case TOK_STR:
                type_str = "STR";
                break;
            case TOK_KEYWORD:
                type_str = "KEYWORD";
                break;
            case TOK_PUNCT:
                type_str = "PUNCT";
                break;
        }
        i32 padding = 4;
        printf("Token type: %s:\n", type_str);
        for (char *code_ref = cur_tok->start - padding; *code_ref && code_ref < cur_tok->start + cur_tok->len + padding; code_ref++) {
            if (isspace(*code_ref)) {
                printf(" ");
                continue;
            }
            printf("%c", *code_ref);
        }
        printf("\n");
        for (i32 i = 0; i < padding; i++) {
            printf(" ");
        }
        for (i32 i = 0; i < cur_tok->len; i++) {
            printf("~");
        }
        for (i32 i = 0; i < padding; i++) {
            printf(" ");
        }
        printf("\n");

        cur_tok = cur_tok->next;
    }

    ast_parse_file(&main_file, head);

    // tokens do not reallocate their string value upon creation, they hold references, so we have to wait until the end of the program to free the buffer
    free(buffer);

    return 0;
}