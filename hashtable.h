#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***
* Definitions
***/

// For keeping track of Item key and value types
typedef enum {INTEGER, FLOAT, STRING} hash_type;

union Hashable {
   int i;
   float f;
   char *str;
};

typedef struct item {
    int hash;
    union Hashable key;
    hash_type key_type;
    union Hashable value;
    hash_type value_type;
} Item;

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
void free_table(HashTable *hash_table);

int calculate_hash(union Hashable key, hash_type key_type);
int calculate_bin_index(int hash, int size);
int max_load_reached(HashTable *hashtable);
int hashable_equal(union Hashable h1, hash_type type1, union Hashable h2, hash_type type2);

HashTable *add(int hash, union Hashable key, hash_type key_type, union Hashable value, hash_type value_type, HashTable *hash_table);
HashTable *add_item_to_table(Item *item, HashTable *hash_table);
Node *add_item_to_bin(Item *item, Node *bin_list);

Item *lookup(union Hashable key, hash_type key_type, HashTable *hash_table);

Item *remove_item_from_table(union Hashable key, hash_type key_type, HashTable *hash_table);
Node *remove_item_from_bin(union Hashable key, hash_type key_type, Node *bin_list);

HashTable *resize(HashTable *hash_table);
