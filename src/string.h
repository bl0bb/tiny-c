#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

#include "types.h"

bool str_starts_with(char *str, const char *cmp_str);
void str_append(char **str, const char *app_str);
i32 count_fmt_str_args(const char *fmt);
void asprintf(char **code, const char *fmt, ...);

#endif