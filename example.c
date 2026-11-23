#define HTABLE_IMPLEMENTATION
#include "hashes.h"

typedef struct sVec2
{
	float x, y;
} vec2;

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
	printf("\n---- int -> int----\n");
	tHashTable *t = ht_create(50, HT_INT, 0, HT_INT, 0);
	printf("HTERROR at creation: %s\n", ht_get_error_msg());
	int k = 42, v = 100;
	ht_insert(t, &k, 1, &v, 1);
	printf("HTERROR at insert: %s\n", ht_get_error_msg());
	ht_print(t);
	printf("HTERROR at print: %s\n", ht_get_error_msg());

	printf("\n---- string key -> int value----\n");
	tHashTable *s = ht_create(50, HT_STR, 0, HT_INT, 0);
	printf("HTERROR at creation: %s\n", ht_get_error_msg());
	ht_insert(s, "hello", 5, &(int){2},  1);
	printf("HTERROR at insert: %s\n", ht_get_error_msg());
	ht_insert(s, "wood",  4, &(int){23}, 1);
	printf("HTERROR at insert: %s\n", ht_get_error_msg());
	ht_print(s);
	printf("HTERROR at print: %s\n", ht_get_error_msg());
	const char *key = "wood";
	printf("Got value: %d from key '%s'\n", *(int *)HT_GET(s, key, strlen(key)), key);
	printf("HTERROR at get: %s\n", ht_get_error_msg());

	printf("\n---- vec2 key -> vec2 value, both stored by value----\n");
	tHashTable *vecs = HT_CREATE(50, HT_UNKNOWN, HT_TYPE_SIZE(vec2), HT_UNKNOWN, HT_TYPE_SIZE(vec2));
	printf("HTERROR at creation: %s\n", ht_get_error_msg());
	vec2 a = {5.2f, 232.91f};
	vec2 b = {1.0f, 2.0f};
	ht_insert(vecs, &a, 1, &b, 1);
	printf("HTERROR at insert: %s\n", ht_get_error_msg());
	ht_set_print_fns(vecs, print_vec2, print_vec2);
	printf("HTERROR at setting print fns: %s\n", ht_get_error_msg());
	ht_print(vecs);
	printf("HTERROR at print: %s\n", ht_get_error_msg());

	printf("\n---- pointer key -> vec2 value, pointer-identity lookup----\n");
	tHashTable *by_ptr = ht_create(50, HT_PTR, 0, HT_UNKNOWN, HT_TYPE_SIZE(vec2));
	printf("HTERROR at creation: %s\n", ht_get_error_msg());
	vec2 c = {3.14f, 2.71f};
	void *ptr_key = &c;
	ht_insert(by_ptr, &ptr_key, 1, &c, 1);
	printf("HTERROR at insert: %s\n", ht_get_error_msg());
	ht_set_print_fns(by_ptr, print_ptr_key, print_vec2);
	printf("HTERROR at setting print fns: %s\n", ht_get_error_msg());
	ht_print(by_ptr);
	printf("HTERROR at print: %s\n", ht_get_error_msg());

	ht_free(t);
	printf("HTERROR at free: %s\n", ht_get_error_msg());
	ht_free(s);
	printf("HTERROR at free: %s\n", ht_get_error_msg());
	ht_free(vecs);
	printf("HTERROR at free: %s\n", ht_get_error_msg());
	ht_free(by_ptr);
	printf("HTERROR at free: %s\n", ht_get_error_msg());
	return 0;
}
