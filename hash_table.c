#include "hashes.h"

Uint ft_hash(const char *key, size_t size)
{
        Uint hash;

        hash = 0;
        while (*key)
                hash = (hash *31) + *key++;
        return (hash % size);
}

void ft_refresh_nodes(HashTable *table, Node ***new_buckets, size_t new_size)
{
        size_t i;
        size_t new_index;
        Node *node;

        i = 0;
        while(i < table->size)
        {
                node = table->buckets[i];
                while (node)
                {
                        new_index = ft_hash(node->key, new_size);
                        node->next = *new_buckets[new_index];
                        *new_buckets[new_index] = node;
                        node = node->next;
                }
                i++;
        }
}

bool ft_resize_table(HashTable *table)
{
        size_t new_size;
        Node **new_buckets;

        new_size = table->size * 2;
        new_buckets = calloc(new_size, sizeof(Node *));
        if (!new_buckets)
                return (false);
        ft_refresh_nodes(table, &new_buckets, new_size);
        free(table->buckets);
        table->buckets = new_buckets;
        table->size = new_size;
        return (true);
}

HashTable *ft_create_table(size_t size)
{
        HashTable *table;

        table = malloc(sizeof(HashTable));
        if (!table)
                return (NULL);
        table->size = size;
        table->buckets = calloc(size, sizeof(Node *));
        table->count = 0;
        if (!table->buckets)
        {
                free(table);
                return (NULL);
        }
        return (table);
}

bool ft_insert(HashTable *table, char *key, char *value)
{
        Uint index;
        Node *new_node;
        Node *node;
        double l_factor;

        l_factor = (double)table->count / table->size;
        if (l_factor > 0.75)
                if (!ft_resize_table(table))
                        return (false);

        index = ft_hash(key, table->size);
        node = table->buckets[index];
        while (node)
        {
                if (!strcmp(node->key, key))
                {
                        node->value = value;
                        return (true);
                }
                node = node->next;
        }
        new_node = malloc(sizeof(Node));
        if (!new_node)
                return (false);
        new_node->key = key;
        new_node->value = value;
        new_node->next = table->buckets[index];
        table->buckets[index] = new_node;
        table->count++;
        return (true);
}

char* ft_get(HashTable *table, char *key)
{
        Uint index;
        char* value;
        Node* node;

        index = ft_hash(key, table->size);
        node = table->buckets[index];
        while (node)
        {
                if (!strcmp(node->key, key))
                        value = node->value;
                        return (value);
                node = node->next;
        }
        return (NULL);
}

bool ft_delete(HashTable *table, char *key)
{
        Uint index;
        Node *curr;
        Node *prev;

        index = ft_hash(key, table->size);
        curr = table->buckets[index];
        while (curr)
        {
                if (!strcmp(key, curr->key))
                {
                        if (prev)
                                prev->next = curr->next;
                        else
                                table->buckets[index] = curr->next;
                        free(curr);
                        return (true);
                }
                prev = curr;
                curr = curr->next;
        }
        return (false);
}

void ft_free_shit(HashTable *table)
{
        size_t i;
        Node *curr;
        Node *prev;

        i = 0;
        while(i < table->size)
        {
                curr = table->buckets[i];
                while (curr)
                {
                        prev = curr;
                        curr = curr->next;
                        free(prev);
                }
                i++;
        }
        free(table->buckets);
        free(table);
}
