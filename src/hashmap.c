#include <string.h>

#include "hashmap.h"

// idk how hashmaps work, so i just use a regular string lookup
// clean, short, simple, easy to debug, and its not the most important thing to work on right now
i32 hashmap_get_idx(const HashMap *hashmap, const char *key, i32 len) {
    i32 idx = -1;
    for (i32 i = 0; i < hashmap->size; i++) {
        if (str_cmp_nnull(key, len, hashmap->keys[i])) {
            return idx;
        }
    }
    return idx;
}

i32 hashmap_get_idx_str(const HashMap *hashmap, const char *key) {
    return hashmap_get_idx(hashmap, key, strlen(key));
}

bool hashmap_has(const HashMap *hashmap, const char *key, i32 len) {
    return hashmap_get_idx(hashmap, key, len) != -1;
}

bool hashmap_has_str(const HashMap *hashmap, const char *key) {
    return hashmap_has(hashmap, key, strlen(key));
}

void *hashmap_get(const HashMap *hashmap, const char *key, i32 len) {
    i32 idx = hashmap_get_idx(hashmap, key, len);
    if (idx == -1) {
        return 0;
    }

    return hashmap->values[idx];
}

void *hashmap_get_str(const HashMap *hashmap, const char *key) {
    return hashmap_get(hashmap, key, strlen(key));
}

// assumes you already checked that they key doesnt already exist in the hashmap
// takes a string, and its length, string does not need to be null terminated
void hashmap_put(HashMap *hashmap, const char *key, i32 len, const void *value) {
    hashmap->size++;
    hashmap->keys = realloc(hashmap->keys, sizeof(char *) * hashmap->size);
    hashmap->values = realloc(hashmap->values, sizeof(void *) * hashmap->size);
    hashmap->keys[hashmap->size - 1] = malloc(sizeof(char) * (len + 1));
    strncpy(&hashmap->keys[hashmap->size - 1], key, len);
    hashmap->values[hashmap->size - 1] = value;
}

// same as hashmap_put, but the key is null terminated
void hashmap_put_str(HashMap *hashmap, const char *key, const void *value) {
    hashmap_put(hashmap, key, strlen(key), value);
}

// same as hashmap_put, but checks if the key is already present and if so, does nothing and returns false, otherwise, stores the value and returns true
bool hashmap_put_check(const HashMap *hashmap, const char *key, i32 len, const void *value) {
    i32 idx = hashmap_get_idx(hashmap, key);
    if (idx == -1) {
        return false;
    }

    hashmap_put(hashmap, key, len, value);
    return true;
}

bool hashmap_put_check_str(const HashMap *hashmap, const char *key, const void *value) {
    return hashmap_put_check(hashmap, key, strlen(key), value);
}