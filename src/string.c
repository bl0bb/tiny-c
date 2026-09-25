#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "string.h"

// check if non null terminated string (str1, len1) is equal to null terminated string (str2)
bool str_cmp_nnull(const char *str1, i32 len1, const char *str2) {
    i32 len2 = strlen(str2);
    return len1 == len2 && strncmp(str1, str2, len1) == 0;
}

bool str_starts_with(char *str, const char *cmp_str) {
    return strncmp(str, cmp_str, strlen(cmp_str)) == 0;
}

// expands a string by the length of the string to append
// then appends the string
void str_append(char **str, const char *app_str) {
    i32 new_len = strlen(*str) + strlen(app_str);
    *str = realloc(*str, sizeof(char) * (new_len + 1));
    strcat(*str, app_str);
    (*str)[new_len] = 0;
}

// counts how many format arguments are in a format string (the type of format strings used in e.g. printf)
// e.g.
// "Name %s, Age %d" -> 2 arguments
// format string specification: https://cplusplus.com/reference/cstdio/printf/
i32 count_fmt_str_args(const char *fmt) {
    i32 arg_count = 0;

    for (const char *ptr = fmt; *ptr; ++ptr) {
        if (*ptr != '%') {
            continue;
        }
        ptr++; // skip %

        if (*ptr == '%') {
            // %% results in %, same way you would do \\ in a regular string to make a regular backslash
            continue;
        }

        // skip flags
        while (*ptr == '-' || *ptr == '+' || *ptr == ' ' || *ptr == '#' || *ptr == '0') {
            ptr++;
        }

        // skip width
        if (*ptr == '*') {
            arg_count++;  // * width as an int argument
            ptr++;
        } else {
            while (*ptr >= '0' && *ptr <= '9') {
                ptr++;
            }
        }

        // skip precision
        if (*ptr == '.') {
            ptr++;
            if (*ptr == '*') {
                arg_count++; // .* precision as an int argument
                ptr++;
            } else {
                while (*ptr >= '0' && *ptr <= '9') {
                    ptr++;
                }
            }
        }

        // skip length modifier
        if (*ptr == 'h' || *ptr == 'l' || *ptr == 'j' || *ptr == 'z' || *ptr == 't' || *ptr == 'L') {
            char modifier = *ptr++; // the next char
            if ((modifier == 'h' && *ptr == 'h') || (modifier == 'l' && *ptr == 'l')) {
                ptr++;
            }
        }

        // conversion character (e.g. the "f" in "%.6f")
        if (*ptr) {
            arg_count++;
        }
    }

    return arg_count;
}

// more or less just sprintf, but allocates more memory (the length of the string to add)
// get the length of the resulting string with formatting (format string, with the arguments replaced with actual values, e.g. %u -> 123)
// then reallocate the buffer string to fit the new data, then write the format string with arguments, then add null terminator
void asprintf(char **code, const char *fmt, ...) {
    i32 fmt_arg_count = count_fmt_str_args(fmt);

    va_list args;
    va_start(args, fmt);

    i32 code_len = strlen(*code);
    i32 len = snprintf(NULL, 0, fmt, args); // use this to get length, a bit slow but its simple and it works. TODO: rework?
    i32 new_len = code_len + len;
    *code = realloc(*code, sizeof(char) * (new_len + 1));
    vsprintf(*code + code_len, fmt, args); // sprintf writes at the pointer directly, not at the end of the string (the null terminator), so we need to add an offset
    (*code)[new_len] = 0;
    
    va_end(args);
}