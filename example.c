#define HTABLE_IMPLEMENTATION
#include "hashes.h"

int main(int argc, char *argv[])
{
        tHashTable *table;

        table = create_table(TABLE_SIZE, H_INT, H_INT);
		const int *key = (const int [3]){5, 3, 11};
		int val = 3;
		HT_INSERT(table, key, 3, CAST_GEN(&val), 1);
		if (HERROR != 0)
			printf("%s\n", get_error_msg());
        printf("%s amount: %d\n", "arr key", CAST_INT(HT_GET(table, key, 3)));
        free_table(table);
        return 0;
}
