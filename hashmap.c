// This is a "hashmap" in the sense that it has a key and a value

#include <stdio.h>
#include <string.h>

typedef struct {
    char* key;
    char* value;
} Element;

typedef struct {
    Element* map;
    int size;
} HashMap;

char* lookup(HashMap* hm, char* key) {
    for (int i = 0; i < hm->size; i++) {
        Element em = hm->map[i];
        if (strcmp(em.key, key) == 0) {
            return em.value;
        }
    }
    return NULL;
}

int main(int argc, char** argv) {
    HashMap map = {};
} 