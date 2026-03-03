#include "hashes.h"

int main(int argc, char *argv[])
{
        HashTable *table;

        table = ft_create_table(TABLE_SIZE);
        ft_insert(table, "sword", "4");
        ft_insert(table, "armor", "1");
        printf("%s amount: %s\n", "armor", ft_get(table, "armor"));
        ft_free_shit(table);
        return 0;
}
