# Hash Table

A generic hash table library written in C. It maps keys to values using a polynomial byte hash and separate chaining to handle collisions. The table copies key and value data into its own allocations and automatically expands its bucket array as its load factor grows.

## Features

- Supports built-in scalar types, fixed-width integers, strings, and pointers.
- Supports custom key and value types through explicit element sizes.
- Uses separate chaining for collision resolution.
- Automatically doubles the bucket count when the load factor exceeds the implementation's resize threshold.
- Supports insertion, lookup, deletion, and printing.
- Replaces the value when an existing key is inserted again.
- Supports custom key and value printing callbacks.
- Reports errors through a thread-local error state when supported by the compiler.

## Files

- `hashes.h` — library interface and implementation.
- `example.c` — examples using integer keys, string keys, custom structures, and pointer keys.

## Requirements

- A C compiler with support for C11 is recommended.
- Standard C library headers.

## Integration

This is a single-header library. Define `HTABLE_IMPLEMENTATION` in **one** C translation unit before including the header. Do not define it in multiple translation units, or you may get multiple-definition linker errors.

```c
#define HTABLE_IMPLEMENTATION
#include "hashes.h"
```

For a multi-file project, put the implementation in one source file and include the header without defining `HTABLE_IMPLEMENTATION` in the other files.

## Quick start

```c
#define HTABLE_IMPLEMENTATION
#include "hashes.h"

#include <stdio.h>

int main(void)
{
    tHashTable *table = ht_create(50, HT_INT, 0, HT_INT, 0);
    if (!table) {
        fprintf(stderr, "Could not create table: %s\n", ht_get_error_msg());
        return 1;
    }

    int key = 42;
    int value = 100;

    if (!HT_INSERT(table, &key, 1, &value, 1)) {
        fprintf(stderr, "Insert failed: %s\n", ht_get_error_msg());
        ht_free(table);
        return 1;
    }

    int *stored_value = HT_GET(table, &key, 1);
    if (stored_value)
        printf("%d -> %d\n", key, *stored_value);

    HT_PRINT(table);
    ht_free(table);
    return 0;
}
```

Compile:

```sh
cc -std=c11 -Wall -Wextra -Wpedantic example.c -o example
```

Run:

```sh
./example
```

## Creating a table

Use:

```c
tHashTable *table = ht_create(
    initial_bucket_count,
    key_type, key_custom_size,
    value_type, value_custom_size
);
```

- `initial_bucket_count` must be at least `1`.
- For a built-in type, pass its `HT_*` type and `0` for the corresponding custom size.
- For a custom type, pass `HT_UNKNOWN` and `sizeof(your_type)` for the corresponding custom size.
- The key and value types are configured independently.

Examples:

```c
/* int -> int */
tHashTable *numbers = ht_create(50, HT_INT, 0, HT_INT, 0);

/* C-string content -> int */
tHashTable *words = ht_create(50, HT_STR, 0, HT_INT, 0);

/* custom structure -> custom structure */
tHashTable *points = ht_create(
    50, HT_UNKNOWN, sizeof(vec2),
    HT_UNKNOWN, sizeof(vec2)
);
```

## API

| Function / macro | Purpose |
|---|---|
| `ht_create(...)` | Creates a table with the specified key and value types. |
| `HT_INSERT(table, key, key_len, value, value_len)` / `ht_insert` | Inserts a key/value pair. If the key exists, replaces its value. |
| `HT_GET(table, key, key_len)` / `ht_get` | Returns a pointer to the stored value, or `NULL` if the key is not found or an error occurs. |
| `HT_DEL(table, key, key_len)` / `ht_delete` | Removes a key/value pair. |
| `HT_PRINT(table)` / `ht_print` | Prints the table. |
| `ht_set_print_fns(table, key_fn, value_fn)` | Sets optional custom key and value printing callbacks. |
| `ht_free(table)` | Frees all nodes, copied keys and values, buckets, and the table itself. |
| `ht_get_error_msg()` | Returns a message for the most recent public operation's error state. |

The convenience macros check whether the table pointer is `NULL` before dispatching through its function table. The corresponding functions can also be called directly.

## Supported key and value types

The `tHTType` enum includes:

- `HT_CHAR`, `HT_STR`, `HT_BOOL`
- `HT_INT`, `HT_UINT`, `HT_LONG`, `HT_ULONG`
- `HT_LLONG`, `HT_ULLONG`, `HT_SHORT`, `HT_USHORT`
- `HT_FLOAT`, `HT_DOUBLE`, `HT_SIZE`, `HT_PTR`
- `HT_I8`, `HT_I16`, `HT_I32`, `HT_I64`
- `HT_U8`, `HT_U16`, `HT_U32`, `HT_U64`
- `HT_UNKNOWN` for caller-sized custom types

## Length arguments

`key_len` and `value_len` are element counts, not byte counts. The implementation multiplies each count by the configured size of that key or value type.

- For scalar keys and values, use `1`.
- For a string key using `HT_STR`, pass the number of characters to hash/store, typically `strlen(text)`. The terminating null byte is not included unless you explicitly include it in the length.
- For custom structures, use `1` when storing one structure.
- For arrays of a configured type, pass the number of elements.

`HT_STR` is treated as character content, not as a stored `char *`. Pass the string itself as the key pointer.

## Pointer keys

`HT_PTR` stores pointer values and compares their byte representation. This gives pointer-identity lookup: two different addresses are different keys even if the objects they point to contain equal data.

For example:

```c
int object = 7;
void *key = &object;

tHashTable *table = ht_create(50, HT_PTR, 0, HT_INT, 0);
int value = 123;

HT_INSERT(table, &key, 1, &value, 1);

int *found = HT_GET(table, &key, 1);
```

The key argument is the address of the pointer variable (`&key`), because the table copies the pointer value itself.

## Custom printing

Set callbacks to control how custom keys or values are displayed:

```c
typedef struct {
    float x;
    float y;
} vec2;

static void print_vec2(const void *item)
{
    const vec2 *point = item;
    printf("(%g, %g)", point->x, point->y);
}

/* ... */
ht_set_print_fns(table, print_vec2, print_vec2);
```

Each callback must have the signature `void (*)(const void *)`. A callback overrides the built-in type-based printer for its corresponding key or value.

## Implementation notes

- The hash function processes the key's bytes using `hash = hash * 31 + byte`, then reduces the result modulo the bucket count.
- Collisions are stored in linked lists attached to buckets (separate chaining).
- The table doubles its bucket count when the load factor calculated before an insertion is greater than `0.75`.
- Keys and values are copied into separately allocated memory when inserted. Inserting an existing key replaces and frees the old stored value.
- `HT_GET` returns a pointer to the table's stored value. Do not free it; its lifetime is tied to the entry, and it becomes invalid when that entry is replaced, deleted, or the table is freed.

## Error handling

After a failed operation, call `ht_get_error_msg()` to retrieve the message associated with the most recent public `ht_*` call. The error state is reset at the beginning of each public operation.

The implementation uses thread-local error storage when supported by the compiler. On toolchains without thread-local storage support, the fallback error state is process-global and is not thread-safe.

## License

This project is licensed under the MIT License.
See the [LICENSE](LICENSE) file for details.
