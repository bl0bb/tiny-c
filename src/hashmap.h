#ifndef HASHMAP_H
#define HASHMAP_H

#include "types.h"

typedef struct HashMap HashMap;

struct HashMap {
    i32 size;
    char **keys;
    void **values;
};


i32 hashmap_get_idx(const HashMap *hashmap, const char *key, i32 len);
i32 hashmap_get_idx_str(const HashMap *hashmap, const char *key);
bool hashmap_has(const HashMap *hashmap, const char *key, i32 len);
bool hashmap_has_str(const HashMap *hashmap, const char *key);
void *hashmap_get(const HashMap *hashmap, const char *key, i32 len);
void *hashmap_get_str(const HashMap *hashmap, const char *key);
void hashmap_put(HashMap *hashmap, const char *key, i32 len, const void *value);
void hashmap_put_str(HashMap *hashmap, const char *key, const void *value);
bool hashmap_put_check(const HashMap *hashmap, const char *key, i32 len, const void *value);
bool hashmap_put_check_str(const HashMap *hashmap, const char *key, const void *value);

#endif