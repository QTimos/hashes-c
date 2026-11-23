#ifndef HASHES_H
#define HASHES_H
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>

struct sNode;
struct sHashTable;

/*
 * Key / value type enum.
 *
 *   HT_UNKNOWN  caller supplies the size explicitly (custom structs etc.)
 *   HT_CHAR     single chars, byte-wise (sizeof(char) = 1)
 *   HT_STR      C-string CONTENT (sizeof(char) = 1). Pass key_len = strlen(s).
 *   HT_PTR      pointer value, IDENTITY-based (sizeof(void *)).
 *
 * Everything else is a scalar builtin.
 */
typedef enum {
	HT_UNKNOWN = 0,
	HT_CHAR,
	HT_STR,
	HT_BOOL,
	HT_INT,
	HT_UINT,
	HT_LONG,
	HT_ULONG,
	HT_LLONG,
	HT_ULLONG,
	HT_SHORT,
	HT_USHORT,
	HT_FLOAT,
	HT_DOUBLE,
	HT_SIZE,
	HT_PTR,
	HT_I8,
	HT_I16,
	HT_I32,
	HT_I64,
	HT_U8,
	HT_U16,
	HT_U32,
	HT_U64
} tHTType;

typedef enum
{
	SUCCESS = 0,
	TABLE_CREATE_ERROR = 1,
	DIVBYZERO_INSERT_ERROR = 2,
	INSERT_ERROR = 3,
	RESIZE_ERROR = 4,
	KEY_ERROR = 5,
	SIZE_ERROR = 6,
	DUP_ERROR = 7,
	NULL_TABLE_ERROR = 8,
	NULL_KEY_ERROR = 9,
	NULL_VALUE_ERROR = 10
} HTErrors;

/* Custom display: called once per key (or value) by ht_print.
 * Only used when the caller sets key_print_fn / val_print_fn.
 * Signature must be exactly void (*)(const void *). */
typedef void (*tHTPrintFn)(const void *item);

typedef struct sNode
{
	const void *key;
	void *value;
	size_t key_len;
	size_t value_len;
	struct sNode *next;
} tNode;

typedef struct sHashTableVTable
{
	bool (*ht_insert)(struct sHashTable *self, const void *key, size_t key_len, const void *value, size_t value_len);
	bool (*ht_delete)(struct sHashTable *self, const void *key, size_t key_len);
	void* (*ht_get)(struct sHashTable *self, const void *key, size_t key_len);
	void (*ht_print)(struct sHashTable *self);
} tHashTableVTable;

typedef struct sHashTable
{
	tNode **buckets;
	size_t size;
	size_t count;
	size_t node_key_type_size;
	size_t node_val_type_size;
	tHTType key_type;
	tHTType val_type;
	tHTPrintFn key_print_fn;   /* optional; overrides type dispatch for keys */
	tHTPrintFn val_print_fn;   /* optional; overrides type dispatch for values */
	const tHashTableVTable *vtable;
} tHashTable;

/*
 * ht_create:
 *   size                : number of initial buckets (>= 1)
 *   key_type/val_type   : builtin types, or HT_UNKNOWN for a custom size
 *   key_custom_size     : 0 when key_type != HT_UNKNOWN, else sizeof(struct...)
 *   val_custom_size     : same, for the value
 *
 * Examples:
 *   ht_create(50, HT_INT, 0,     HT_INT, 0);                    // int -> int
 *   ht_create(50, HT_STR, 0,     HT_INT, 0);                    // string -> int
 *   ht_create(50, HT_UNKNOWN, sizeof(struct point), HT_INT, 0); // struct -> int
 *
 * Returns NULL on invalid args or allocation failure; ht_get_error_msg() reports why.
 */
/*
 * ht_get_error_msg reads the error state of the most recent ht_* call.
 * Every public ht_* function resets the slot to SUCCESS on entry and sets
 * it to a specific code on failure, so the slot describes "this call"
 * without the caller needing to clear it manually.
 *
 * The slot is per-thread when the compiler supports thread-local storage
 * (see HT_THREAD_SAFE_ERROR_STATE). When that flag is 0, the slot is a
 * process-global static and these reads are NOT thread-safe.
 */
tHashTable *ht_create(size_t size, tHTType key_type, size_t key_custom_size, tHTType val_type, size_t val_custom_size);
void ht_free(tHashTable *table);
bool ht_insert(tHashTable *self, const void *key, size_t key_len, const void *value, size_t value_len);
void* ht_get(tHashTable *self, const void *key, size_t key_len);
bool ht_delete(tHashTable *self, const void *key, size_t key_len);
void ht_print(tHashTable *table);
void ht_set_print_fns(tHashTable *table, tHTPrintFn key_fn, tHTPrintFn val_fn);
const char* ht_get_error_msg(void);

// macros to the functions and a wrapper macro for sizeof()
#define HT_CREATE(s, kt, ks, vt, vs) (ht_create((s), (kt), (ks), (vt), (vs)))
#define HT_FREE(t) (ht_free(t))
#define HT_INSERT(t, k, kl, v, vl) ((t) ? ht_insert((t), (k), (kl), (v), (vl)) : false)
#define HT_GET(t, k, kl) ((t) ? ht_get((t), (k), (kl)) : NULL)
#define HT_DEL(t, k, kl) ((t) ? ht_delete((t), (k), (kl)) : false)
#define HT_PRINT(t) ((t) ? ht_print((t)) : (void)0)
#define HT_SET_PRINT_FNS(t, kf, vf) (ht_set_print_fns((t), (kf), (vf)))
#define HT_GET_ERROR_MSG() ht_get_error_msg()
#define HT_TYPE_SIZE(T)   sizeof(T)

/* thread-local error state */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#  define HT_THREAD_LOCAL _Thread_local
#  define HT_THREAD_SAFE_ERROR_STATE 1
#elif defined(__GNUC__)
#  define HT_THREAD_LOCAL __thread
#  define HT_THREAD_SAFE_ERROR_STATE 1
#elif defined(_MSC_VER)
#  define HT_THREAD_LOCAL __declspec(thread)
#  define HT_THREAD_SAFE_ERROR_STATE 1
#else
#  define HT_THREAD_LOCAL /* empty / not thread safe branch */
#  define HT_THREAD_SAFE_ERROR_STATE 0
#endif

#if !HT_THREAD_SAFE_ERROR_STATE && !defined(HT_ACCEPT_UNSAFE_THREAD_STATE)
#  if defined(__GNUC__) || defined(__clang__) || \
      (defined(__STDC_VERSION__) && __STDC_VERSION__ >= 202311L)
#    warning "HT: error state is not thread-local on this toolchain (not thread-safe). Define HT_ACCEPT_UNSAFE_THREAD_STATE to silence."
#  elif defined(_MSC_VER)
#    pragma message("HT: error state is not thread-local on this toolchain (not thread-safe). Define HT_ACCEPT_UNSAFE_THREAD_STATE to silence.")
#  endif
#endif


#ifdef HTABLE_IMPLEMENTATION

static HT_THREAD_LOCAL int8_t ht_err_state = 0;
static inline int8_t* get_err(void)
{
	return &ht_err_state;
}
#define _HTERROR (get_err())
#define HTERROR *_HTERROR

static size_t ht_get_type_size(tHTType type)
{
	switch (type) {
		case HT_CHAR:   return sizeof(char);
		case HT_STR:    return sizeof(char);
		case HT_BOOL:   return sizeof(bool);
		case HT_INT:    return sizeof(int);
		case HT_UINT:   return sizeof(unsigned int);
		case HT_LONG:   return sizeof(long);
		case HT_ULONG:  return sizeof(unsigned long);
		case HT_LLONG:  return sizeof(long long);
		case HT_ULLONG: return sizeof(unsigned long long);
		case HT_SHORT:  return sizeof(short);
		case HT_USHORT: return sizeof(unsigned short);
		case HT_FLOAT:  return sizeof(float);
		case HT_DOUBLE: return sizeof(double);
		case HT_SIZE:   return sizeof(size_t);
		case HT_PTR:    return sizeof(void *);
		case HT_I8:     return sizeof(int8_t);
		case HT_I16:    return sizeof(int16_t);
		case HT_I32:    return sizeof(int32_t);
		case HT_I64:    return sizeof(int64_t);
		case HT_U8:     return sizeof(uint8_t);
		case HT_U16:    return sizeof(uint16_t);
		case HT_U32:    return sizeof(uint32_t);
		case HT_U64:    return sizeof(uint64_t);
		case HT_UNKNOWN:
		default:        return 0;
	}
}

static void *_duplicate_val(const void *data, size_t len, size_t type_size)
{
	if (!data || !len || !type_size) return NULL;
	if (len > SIZE_MAX / type_size) return NULL;
	size_t size_in_bytes = len * type_size;
	uint8_t *dup = malloc(size_in_bytes);
	if (!dup) return NULL;
	return memcpy(dup, data, size_in_bytes);
}

static uint32_t _hash(const void *key, size_t key_len_in_bytes, size_t size)
{
	const uint8_t *k = (const uint8_t *)key;
	uint32_t hash = 0;

	for (size_t i = 0; i < key_len_in_bytes; i++)
		hash = (hash * 31) + k[i];
	return (uint32_t)(hash % size);
}

static void _refresh_nodes(tHashTable *table, tNode **new_buckets, size_t new_size)
{
	tNode *node;
	tNode *next_node;

	for (size_t i = 0; i < table->size; i++)
	{
		node = table->buckets[i];
		while (node)
		{
			next_node = node->next;
			size_t new_index = _hash(node->key,
			                         node->key_len * table->node_key_type_size,
			                         new_size);
			node->next = new_buckets[new_index];
			new_buckets[new_index] = node;
			node = next_node;
		}
	}
}

static bool _resize_table(tHashTable *table)
{
	if (table->size > SIZE_MAX / 2)
	{
		HTERROR = RESIZE_ERROR;
		return false;
	}
	size_t new_size = table->size * 2;
	tNode **new_buckets = (tNode **)calloc(new_size, sizeof(tNode *));
	if (!new_buckets)
	{
		HTERROR = RESIZE_ERROR;
		return false;
	}
	_refresh_nodes(table, new_buckets, new_size);
	free(table->buckets);
	table->buckets = new_buckets;
	table->size = new_size;
	return true;
}

// Print one stored item (key or value): either via user fn, or by type dispatch.
static void ht_print_item(tHTType type, size_t elem_size, const void *data, size_t count, tHTPrintFn fn)
{
	if (fn)
	{
		fn(data);
		return;
	}
	if (type == HT_UNKNOWN)
	{
		printf("<%zu bytes>", count * elem_size);
		return;
	}
	if (type == HT_STR)
	{
		size_t n = count * elem_size;
		printf("\"%.*s\"", (int)n, (const char *)data);
		return;
	}
	for (size_t i = 0; i < count; i++)
	{
		const void *p = (const uint8_t *)data + i * elem_size;
		if (i > 0) printf(" ");
		switch (type) {
			case HT_CHAR:   printf("'%c'", *(const char *)p); break;
			case HT_BOOL:   printf("%s", *(const bool *)p ? "true" : "false"); break;
			case HT_INT:    printf("%d", *(const int *)p); break;
			case HT_UINT:   printf("%u", *(const unsigned int *)p); break;
			case HT_LONG:   printf("%ld", *(const long *)p); break;
			case HT_ULONG:  printf("%lu", *(const unsigned long *)p); break;
			case HT_LLONG:  printf("%lld", *(const long long *)p); break;
			case HT_ULLONG: printf("%llu", *(const unsigned long long *)p); break;
			case HT_SHORT:  printf("%hd", *(const short *)p); break;
			case HT_USHORT: printf("%hu", *(const unsigned short *)p); break;
			case HT_FLOAT:  printf("%f", *(const float *)p); break;
			case HT_DOUBLE: printf("%f", *(const double *)p); break;
			case HT_SIZE:   printf("%zu", *(const size_t *)p); break;
			case HT_PTR:    printf("%p", *(void * const *)p); break;
			case HT_I8:     printf("%" PRId8,  *(const int8_t *)p);  break;
			case HT_I16:    printf("%" PRId16, *(const int16_t *)p); break;
			case HT_I32:    printf("%" PRId32, *(const int32_t *)p); break;
			case HT_I64:    printf("%" PRId64, *(const int64_t *)p); break;
			case HT_U8:     printf("%" PRIu8,  *(const uint8_t *)p);  break;
			case HT_U16:    printf("%" PRIu16, *(const uint16_t *)p); break;
			case HT_U32:    printf("%" PRIu32, *(const uint32_t *)p); break;
			case HT_U64:    printf("%" PRIu64, *(const uint64_t *)p); break;
			default:        printf("?"); break;
		}
	}
}

bool ht_insert(tHashTable *self, const void *key, size_t key_len,
               const void *value, size_t value_len)
{
	HTERROR = SUCCESS;
	if (!self)
	{
		HTERROR = NULL_TABLE_ERROR;
		return false;
	}
	if (!key || !key_len)
	{
		HTERROR = NULL_KEY_ERROR;
		return false;
	}
	if (!value || !value_len)
	{
		HTERROR = NULL_VALUE_ERROR;
		return false;
	}
	if (!self->size)
	{
		HTERROR = DIVBYZERO_INSERT_ERROR;
		return false;
	}

	uint32_t index;
	tNode *new_node;
	tNode *node;
	double l_factor = (double)self->count / self->size;

	if (l_factor > 0.75)
		if (!_resize_table(self))
			return false;

	index = _hash(key, key_len * self->node_key_type_size, self->size);
	node = self->buckets[index];
	while (node)
	{
		if (node->key_len == key_len &&
		    !memcmp(node->key, key, self->node_key_type_size * key_len))
		{
			void *tmp_val = _duplicate_val(value, value_len, self->node_val_type_size);
			if (!tmp_val)
			{
				HTERROR = DUP_ERROR;
				return false;
			}
			free(node->value);
			node->value = tmp_val;
			node->value_len = value_len;
			return true;
		}
		node = node->next;
	}

	new_node = (tNode *)malloc(sizeof(tNode));
	if (!new_node)
	{
		HTERROR = INSERT_ERROR;
		return false;
	}
	new_node->key = _duplicate_val(key, key_len, self->node_key_type_size);
	if (!new_node->key)
	{
		free(new_node);
		HTERROR = DUP_ERROR;
		return false;
	}
	new_node->value = _duplicate_val(value, value_len, self->node_val_type_size);
	if (!new_node->value)
	{
		free((void *)new_node->key);
		free(new_node);
		HTERROR = DUP_ERROR;
		return false;
	}
	new_node->key_len = key_len;
	new_node->value_len = value_len;
	new_node->next = self->buckets[index];
	self->buckets[index] = new_node;
	self->count++;
	return true;
}

void* ht_get(tHashTable *self, const void *key, size_t key_len)
{
	HTERROR = SUCCESS;
	if (!self)
	{
		HTERROR = NULL_TABLE_ERROR;
		return NULL;
	}
	if (!key || !key_len)
	{
		HTERROR = NULL_KEY_ERROR;
		return NULL;
	}
	if (!self->size)
	{
		HTERROR = DIVBYZERO_INSERT_ERROR;
		return NULL;
	}

	uint32_t index = _hash(key, key_len * self->node_key_type_size, self->size);
	tNode *node = self->buckets[index];

	while (node)
	{
		if (node->key_len == key_len &&
		    !memcmp(node->key, key, self->node_key_type_size * key_len))
			return node->value;
		node = node->next;
	}
	HTERROR = KEY_ERROR;
	return NULL;
}

bool ht_delete(tHashTable *self, const void *key, size_t key_len)
{
	HTERROR = SUCCESS;
	if (!self)
	{
		HTERROR = NULL_TABLE_ERROR;
		return false;
	}
	if (!key || !key_len)
	{
		HTERROR = NULL_KEY_ERROR;
		return false;
	}
	if (!self->size)
	{
		HTERROR = DIVBYZERO_INSERT_ERROR;
		return false;
	}

	uint32_t index = _hash(key, key_len * self->node_key_type_size, self->size);
	tNode *curr = self->buckets[index];
	tNode *prev = NULL;

	while (curr)
	{
		if (curr->key_len == key_len &&
		    !memcmp(key, curr->key, self->node_key_type_size * key_len))
		{
			if (prev)
				prev->next = curr->next;
			else
				self->buckets[index] = curr->next;
			self->count--;
			free((void *)curr->key);
			free(curr->value);
			free(curr);
			return true;
		}
		prev = curr;
		curr = curr->next;
	}
	HTERROR = KEY_ERROR;
	return false;
}

void ht_print(tHashTable *table)
{
	HTERROR = SUCCESS;
	if (!table)
	{
		HTERROR = NULL_TABLE_ERROR;
		return;
	}

	printf("HashTable(size=%zu, count=%zu)\n", table->size, table->count);
	if (table->count == 0) return;

	printf("{\n");
	for (size_t i = 0; i < table->size; i++)
	{
		tNode *node = table->buckets[i];
		while (node)
		{
			// printf("  [%zu] ", i);
			ht_print_item(table->key_type, table->node_key_type_size,
			              node->key, node->key_len, table->key_print_fn);
			printf(": ");
			ht_print_item(table->val_type, table->node_val_type_size, node->value, node->value_len, table->val_print_fn);
			printf("\n");
			node = node->next;
		}
	}
	printf("}\n");
}

void ht_set_print_fns(tHashTable *table, tHTPrintFn key_fn, tHTPrintFn val_fn)
{
	HTERROR = SUCCESS;
	if (!table)
	{
		HTERROR = NULL_TABLE_ERROR;
		return;
	}
	table->key_print_fn = key_fn;
	table->val_print_fn = val_fn;
}

tHashTable *ht_create(size_t size,
                      tHTType key_type, size_t key_custom_size,
                      tHTType val_type, size_t val_custom_size)
{
	HTERROR = SUCCESS;
	if (!size)
	{
		HTERROR = SIZE_ERROR;
		return NULL;
	}
	size_t key_size;
	if (key_type == HT_UNKNOWN)
	{
		if (!key_custom_size)
		{
			HTERROR = SIZE_ERROR;
			return NULL;
		}
		key_size = key_custom_size;
	}
	else
	{
		if (key_custom_size) 
		{
			HTERROR = SIZE_ERROR;
			return NULL;
		}
		key_size = ht_get_type_size(key_type);
	}
	size_t val_size;
	if (val_type == HT_UNKNOWN)
	{
		if (!val_custom_size)
		{
			HTERROR = SIZE_ERROR;
			return NULL;
		}
		val_size = val_custom_size;
	}
	else
	{
		if (val_custom_size)
		{
			HTERROR = SIZE_ERROR;
			return NULL;
		}
		val_size = ht_get_type_size(val_type);
	}
	if (!key_size || !val_size)
	{
		HTERROR = SIZE_ERROR;
		return NULL;
	}

	tHashTable *table = (tHashTable *)malloc(sizeof(tHashTable));
	if (!table)
	{
		HTERROR = TABLE_CREATE_ERROR;
		return NULL;
	}
	static const tHashTableVTable vtable = { ht_insert, ht_delete, ht_get, ht_print };
	table->vtable = &vtable;
	table->size = size;
	table->buckets = (tNode **)calloc(size, sizeof(tNode *));
	if (!table->buckets)
	{
		free(table);
		HTERROR = TABLE_CREATE_ERROR;
		return NULL;
	}
	table->count = 0;
	table->node_key_type_size = key_size;
	table->node_val_type_size = val_size;
	table->key_type = key_type;
	table->val_type = val_type;
	table->key_print_fn = NULL;
	table->val_print_fn = NULL;
	return table;
}

void ht_free(tHashTable *table)
{
	HTERROR = SUCCESS;
	if (!table)
	{
		HTERROR = NULL_TABLE_ERROR;
		return;
	}
	for (size_t i = 0; i < table->size; i++)
	{
		tNode *curr = table->buckets[i];
		while (curr)
		{
			tNode *next = curr->next;
			free((void *)curr->key);
			free(curr->value);
			free(curr);
			curr = next;
		}
	}
	free(table->buckets);
	free(table);
}

const char* ht_get_error_msg(void)
{
	switch (HTERROR)
	{
		case SUCCESS: return "Success";
		case TABLE_CREATE_ERROR: return "Failed to create the hash table";
		case DIVBYZERO_INSERT_ERROR: return "Table size is zero — operation would divide by zero";
		case INSERT_ERROR: return "Failed to insert value into table";
		case RESIZE_ERROR: return "Failed to resize table";
		case KEY_ERROR: return "Key not found in table";
		case SIZE_ERROR: return "Invalid type/size argument to ht_create";
		case DUP_ERROR: return "Failed to copy a value or key";
		case NULL_TABLE_ERROR: return "The table passed is NULL";
		case NULL_KEY_ERROR: return "The key passed is NULL or has length 0";
		case NULL_VALUE_ERROR: return "The value passed is NULL or has length 0";
		default: return "Unknown internal system error";
	}
}
#endif

#endif
