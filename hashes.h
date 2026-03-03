#ifndef HASHES_H
#define HASHES_H
#define TABLE_SIZE 50
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>

typedef struct Node {
        char *key;
        char *value;
        struct Node *next;
}       Node;

typedef struct HashTable {
        Node    **buckets;
        size_t  size;
        size_t  count;
}       HashTable;

typedef unsigned int Uint;

HashTable       *ft_create_table(size_t size);
Uint            ft_hash(const char *key, size_t size);
bool            ft_insert(HashTable *table, char *key, char *value);
char*           ft_get(HashTable *table, char *key);
bool            ft_delete(HashTable *table, char *key);
bool            ft_resize_table(HashTable *table);
void            ft_free_shit(HashTable *table);

#endif
