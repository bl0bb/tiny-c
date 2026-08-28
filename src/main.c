#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "file.h"
#include "tokenizer.h"

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

    // rest of the code goes here

    // tokens do not reallocate their string value upon creation, they hold references, so we have to wait until the end of the program to free the buffer
    free(buffer);

    return 0;
}