#include <stdio.h>
#include <stdarg.h>

#include "log.h"

// logging / error handling the user sees when trying to compile code, not to be confused with error handling for the compiler program itself
// ansi escape sequences: https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797

const i32 ansi_reset = 0;
const i32 ansi_bold = 1;
const i32 ansi_dim = 2;
const i32 ansi_italic = 3;
const i32 ansi_underline = 4;
const i32 ansi_slow_blinking = 5;
const i32 ansi_fast_blinking = 6; // wasnt listed on ANSI code github
const i32 ansi_inverse = 7;
const i32 ansi_invisible = 8;
const i32 ansi_strikethrough = 9;

const i32 ansi_foreground = 38;
const i32 ansi_background = 48;

const i32 ansi_color_as_id = 5;
const i32 ansi_color_as_rgb = 2;

void ansi_seq(FILE *out, int count, ...) {
    va_list args;
    va_start(args, count);

    fprintf(out, "\x1B[");

    for (int i = 0; i < count; i++) {
        int val = va_arg(args, int);
        fprintf(out, "%d", val);

        if (i < count - 1) {
            fprintf(out, ";");
        }
    }

    fprintf(out, "m");

    va_end(args);
}

// uses this format:
/*
../src/main.c:5:14: error: variable 'my_variable' not found
  5 | printf("My variable: %s\n", my_variable);
    |                             ^~~~~~~~~~~
*/
void raw_print_log_at(char *file_path, char *file_contents, char *message_type, i32 message_type_col, char *message, char *start, i32 length) {
    // setup color
    u8 message_type_col_r = (message_type_col >> 16) & 255;
    u8 message_type_col_g = (message_type_col >> 8) & 255;
    u8 message_type_col_b = (message_type_col >> 0) & 255;

    // get line and char
    i32 line_no = 1;
    i32 char_no = 1;
    char *line_start = file_contents;
    for (char *p = file_contents; p <= start; p++) {
        if (*p == '\n') {
            line_no++;
            char_no = 0;
            line_start = p + 1;
            continue;
        }

        // TODO: do i need this?
        // if (*p == '\r') {
        //     continue;
        // }

        char_no++;
    }

    // file_path
    ansi_seq(stdout, 7, ansi_reset, ansi_bold, ansi_foreground, ansi_color_as_rgb, 255, 255, 255);
    printf("%s:%d:%d: ", file_path, line_no, char_no);

    // message_type
    ansi_seq(stdout, 7, ansi_reset, ansi_bold, ansi_foreground, ansi_color_as_rgb, message_type_col_r, message_type_col_g, message_type_col_b);
    printf("%s: ", message_type);

    // message
    ansi_seq(stdout, 1, ansi_reset);
    printf("%s\n", message);

    // file contents



    // prints the line regularly
    // for (char *p = line_start; *p != '\n'; p++) {
    //     putchar(*p);
    // }

    // prints the line highlighting the bad token
    for (char *p = line_start; p < start; p++) {
        putchar(*p);
    }
    ansi_seq(stdout, 5, ansi_reset, ansi_bold, ansi_foreground, ansi_color_as_rgb, message_type_col_r, message_type_col_g, message_type_col_b);
    for (char *p = start; p < start + length; p++) {
        putchar(*p);
    }
    ansi_seq(stdout, 1, ansi_reset);
    for (char *p = start + length; *p != '\n'; p++) {
        putchar(*p);
    }

    printf("\n");



    // highlight
    for (i32 i = 1; i < char_no; i++) {
        printf(" ");
    }
    ansi_seq(stdout, 6, ansi_reset, ansi_foreground, ansi_color_as_rgb, message_type_col_r, message_type_col_g, message_type_col_b);
    printf("^");
    for (i32 i = 1; i < length; i++) {
        printf("~");
    }
    ansi_seq(stdout, 1, ansi_reset);
    printf("\n");



    /*
    {
        i32 cols[] = {
            0xff0000,
            0xff7f00, // ?
            0xffff00,
            // 0x7fff00, // !!!
            0x00ff00,
            // 0x00ff7f, // !!!
            0x00ffff,
            // 0x007fff, // !!!
            0x0000ff,
            // 0x7f00ff, // !!!
            0xff00ff,
            // 0xff007f, // !!!
        };

        char *msg =
            "       \n"
            "  rah  \n"
            "       \n";

        i32 line_start = 0;
        for (i32 i = 0;; i++) {
            if (msg[i] == 0) {
                break;
            }
            if (msg[i] == '\n') {
                line_start = i + 1;
                putchar(msg[i]);
                continue;
            }

            i32 col = cols[(i - line_start) % ARRAY_SIZE(cols)];
            u8 r = (col >> 16) & 255;
            u8 g = (col >> 8) & 255;
            u8 b = (col >> 0) & 255;

            ansi_seq(stdout, 11, ansi_reset, ansi_foreground, ansi_color_as_rgb, 0, 0, 0, ansi_background, ansi_color_as_rgb, r, g, b);
            putchar(msg[i]);
            ansi_seq(stdout, 1, ansi_reset);
        }
    }
    ansi_seq(stdout, 1, ansi_reset);
    printf("\n");
    */
}

// position
void log_at(File *current_file, char *message_type, i32 message_type_col, char *message, char *start, i32 length) {
    raw_print_log_at(current_file->path, current_file->data, message_type, message_type_col, message, start, length);
}

void error_at(File *current_file, char *message, char *start, i32 length) {
    log_at(current_file, "error", 0xff0000, message, start, length);
}

void warn_at(File *current_file, char *message, char *start, i32 length) {
    log_at(current_file, "warning", 0xffff00, message, start, length);
}

void note_at(File *current_file, char *message, char *start, i32 length) {
    log_at(current_file, "note", 0x00ffff, message, start, length);
}

// token
void log_tok(File *current_file, char *message_type, i32 message_type_col, char *message, Token *tok) {
    log_at(current_file, message_type, message_type_col, message, tok->start, tok->len);
}

void error_tok(File *current_file, char *message, Token *tok) {
    log_tok(current_file, "error", 0xff0000, message, tok);
}

void warn_tok(File *current_file, char *message, Token *tok) {
    log_tok(current_file, "warning", 0xffff00, message, tok);
}

void note_tok(File *current_file, char *message, Token *tok) {
    log_tok(current_file, "note", 0x00ffff, message, tok);
}