#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/***
* Definitions
***/

// For keeping track of Item key and value types
typedef enum {INTEGER, DOUBLE, STRING} hash_type;

union Hashable {
   long int i;
   double f;
   char *str;
};

typedef struct item {
    long int hash;
    union Hashable key;
    hash_type key_type;
    union Hashable value;
    hash_type value_type;
} Item;

typedef struct node {
    Item *item;
    struct node *next;
} Node;

typedef struct hashtable {
    long int size;
    long int load;
    double max_load_proportion;
    Node **bin_list;
} HashTable;

/***
* Function declarations
***/
HashTable *init(long int size, double max_load_proportion);
void print_table(HashTable *hashtable);
void print_item(Item *item);
void free_table(HashTable *hashtable);
void free_item(Item *item);

long int calculate_hash(union Hashable key, hash_type key_type);
long int calculate_bin_index(long int hash, long int size);
int max_load_reached(HashTable *hashtable);
int hashable_equal(union Hashable h1, hash_type type1, union Hashable h2, hash_type type2);

HashTable *add(long int hash, union Hashable key, hash_type key_type, union Hashable value, hash_type value_type, HashTable *hashtable);
HashTable *add_item_to_table(Item *item, HashTable *hashtable);
Node *add_item_to_bin(Item *item, Node *bin_list, HashTable *hashtable);

Item *lookup_by_hash(long int hash, union Hashable key, hash_type key_type, HashTable *hashtable);
Item *lookup(union Hashable key, hash_type key_type, HashTable *hashtable);

Item *remove_item_from_table_by_hash(long int hash, union Hashable key, hash_type key_type, HashTable *hashtable);
Item *remove_item_from_table(union Hashable key, hash_type key_type, HashTable *hashtable);
Node *remove_item_from_bin(union Hashable key, hash_type key_type, Node *bin_list);

HashTable *resize(HashTable *hashtable);
