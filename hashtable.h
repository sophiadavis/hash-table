#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***
* Definitions
***/

union Hashable {
   int i;
   float f;
   char *str;
};

typedef struct item {
    int hash;
    union Hashable key;
    int key_type;
    union Hashable value;
    int value_type;
} Item;

// For keeping track of Item key and value types
// const int 0 = 0;
// const int 1 = 1;
// const int 2 = 2;

typedef struct node {
    Item *item;
    struct node *next;
} Node;

typedef struct hash_table {
    int size;
    int load;
    float max_load_proportion;
    Node **bin_list;
} HashTable;

/***
* Function declarations
***/
HashTable *init(int size, float max_load_proportion);
void print_table(HashTable *hash_table);
void print_item(Item *item);

int calculate_hash(union Hashable key, int key_type);
int max_load_reached(HashTable *hashtable);
int hashable_equal(union Hashable h1, int type1, union Hashable h2, int type2);

HashTable *add_item_to_table(int hash, union Hashable key, int key_type, union Hashable value, int value_type, HashTable *hash_table);
Node *add_item_to_bin(Item *item, Node *bin_list);

Item *lookup(union Hashable key, int key_type, HashTable *hash_table);

Item *remove_item_from_table(union Hashable key, int key_type, HashTable *hash_table);
Node *remove_item_from_bin(union Hashable key, int key_type, Node *bin_list);

HashTable *resize(HashTable *hash_table);
