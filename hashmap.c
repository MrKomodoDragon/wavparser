#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t size;
  char **values;
} HashMap;

const size_t BASE = 0x811c9dc5;

const size_t PRIME = 0x01000193;
int hash(HashMap *map, char *key) {
  size_t initial = BASE;

  while (*key) {

    initial ^= *key++;

    initial *= PRIME;
  }

  return initial & (map->size - 1);
}

void insert(char *key, char *value, HashMap *hm) {
  int index = hash(hm, key);
  printf("Index generated: %d\n", index);
  hm->values[index] = value;
}

char *get(char *key, HashMap *hm) {
  int index = hash(hm, key);
  return hm->values[index];
}

int main(int argc, char **argv) {
  HashMap hm = {};
  hm.size = 1000;
  hm.values = malloc(sizeof(char *) * hm.size);
  insert("skibidi", "toilet", &hm);
  printf("%s", get("skibidi", &hm));
}
