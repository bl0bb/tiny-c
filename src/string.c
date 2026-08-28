#include <string.h>

#include "string.h"

bool str_starts_with(char *str, const char *cmp_str) {
    return strncmp(str, cmp_str, strlen(cmp_str)) == 0;
}