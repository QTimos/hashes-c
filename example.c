#define HTABLE_IMPLEMENTATION
#include "hashes.h"

typedef struct sVec2 { float x, y; } vec2;

static void print_vec2(const void *item)
{
    const vec2 *p = item;
    printf("(%g, %g)", p->x, p->y);
}

static void print_ptr_key(const void *key)
{
    const vec2 *p = *(const vec2 * const *)key;
    printf("%p", (const void *)p);
}

int main(void)
{
    /* int -> int */
    tHashTable *t = ht_create(50, HT_INT, 0, HT_INT, 0);
    int k = 42, v = 100;
    HT_INSERT(t, &k, 1, &v, 1);
    HT_PRINT(t);

    /* string key -> int value */
    tHashTable *s = ht_create(50, HT_STR, 0, HT_INT, 0);
    HT_INSERT(s, "hello", 5, &(int){2},  1);
    HT_INSERT(s, "wood",  4, &(int){23}, 1);
    HT_PRINT(s);

    /* vec2 key -> vec2 value, both stored by value */
    tHashTable *vecs = ht_create(50, HT_UNKNOWN, HT_TYPE_SIZE(vec2), HT_UNKNOWN, HT_TYPE_SIZE(vec2));
    vec2 a = {5.2f, 232.91f};
    vec2 b = {1.0f, 2.0f};
    HT_INSERT(vecs, &a, 1, &b, 1);
    ht_set_print_fns(vecs, print_vec2, print_vec2);
    HT_PRINT(vecs);

    /* pointer key -> vec2 value, pointer-identity lookup */
    tHashTable *by_ptr = ht_create(50, HT_PTR, 0, HT_UNKNOWN, HT_TYPE_SIZE(vec2));
    vec2 c = {3.14f, 2.71f};
    void *ptr_key = &c;
    HT_INSERT(by_ptr, &ptr_key, 1, &c, 1);
    ht_set_print_fns(by_ptr, print_ptr_key, print_vec2);
    HT_PRINT(by_ptr);

    ht_free(t);
    ht_free(s);
    ht_free(vecs);
    ht_free(by_ptr);
    return 0;
}
