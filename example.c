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
	tHashTable *t = HT_CREATE(50, HT_INT, 0, HT_INT, 0);
	printf("HTERROR at creation: %s\n", HT_GET_ERROR_MSG());
	int k = 42, v = 100;
	HT_INSERT(t, &k, 1, &v, 1);
	printf("HTERROR at insert: %s\n", HT_GET_ERROR_MSG());
	HT_PRINT(t);
	printf("HTERROR at print: %s\n", HT_GET_ERROR_MSG());

	printf("\n---- string key -> int value----\n");
	tHashTable *s = ht_create(50, HT_STR, 0, HT_INT, 0);
	printf("HTERROR at creation: %s\n", HT_GET_ERROR_MSG());
	HT_INSERT(s, "hello", 5, &(int){2},  1);
	printf("HTERROR at insert: %s\n", HT_GET_ERROR_MSG());
	HT_INSERT(s, "wood",  4, &(int){23}, 1);
	printf("HTERROR at insert: %s\n", HT_GET_ERROR_MSG());
	HT_PRINT(s);
	printf("HTERROR at print: %s\n", HT_GET_ERROR_MSG());
	const char *key = "wood";
	printf("Got value: %d from key '%s'\n", *(int *)HT_GET(s, key, strlen(key)), key);
	printf("HTERROR at get: %s\n", HT_GET_ERROR_MSG());

	printf("\n---- vec2 key -> vec2 value, both stored by value----\n");
	tHashTable *vecs = HT_CREATE(50, HT_UNKNOWN, HT_TYPE_SIZE(vec2), HT_UNKNOWN, HT_TYPE_SIZE(vec2));
	printf("HTERROR at creation: %s\n", HT_GET_ERROR_MSG());
	vec2 a = {5.2f, 232.91f};
	vec2 b = {1.0f, 2.0f};
	HT_INSERT(vecs, &a, 1, &b, 1);
	printf("HTERROR at insert: %s\n", HT_GET_ERROR_MSG());
	HT_SET_PRINT_FNS(vecs, print_vec2, print_vec2);
	printf("HTERROR at setting print fns: %s\n", HT_GET_ERROR_MSG());
	HT_PRINT(vecs);
	printf("HTERROR at print: %s\n", HT_GET_ERROR_MSG());

	printf("\n---- pointer key -> vec2 value, pointer-identity lookup----\n");
	tHashTable *by_ptr = ht_create(50, HT_PTR, 0, HT_UNKNOWN, HT_TYPE_SIZE(vec2));
	printf("HTERROR at creation: %s\n", HT_GET_ERROR_MSG());
	vec2 c = {3.14f, 2.71f};
	void *ptr_key = &c;
	HT_INSERT(by_ptr, &ptr_key, 1, &c, 1);
	printf("HTERROR at insert: %s\n", HT_GET_ERROR_MSG());
	HT_SET_PRINT_FNS(by_ptr, print_ptr_key, print_vec2);
	printf("HTERROR at setting print fns: %s\n", HT_GET_ERROR_MSG());
	HT_PRINT(by_ptr);
	printf("HTERROR at print: %s\n", HT_GET_ERROR_MSG());

	HT_FREE(t);
	printf("HTERROR at free: %s\n", HT_GET_ERROR_MSG());
	HT_FREE(s);
	printf("HTERROR at free: %s\n", HT_GET_ERROR_MSG());
	HT_FREE(vecs);
	printf("HTERROR at free: %s\n", HT_GET_ERROR_MSG());
	HT_FREE(by_ptr);
	printf("HTERROR at free: %s\n", HT_GET_ERROR_MSG());
	return 0;
}
