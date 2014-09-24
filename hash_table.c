#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***
*
***/

/***
* Structs and Unions
***/
union Hashable { // TODO Do I want pointers?
   int i;
   float f;
   char str[50];
};

typedef struct item {
    int hash;
    union Hashable key;
    union Hashable value;
} Item;

typedef struct node {
    Item *data;
    struct node *next;
} Node;
// Last item in list will have next set to NULL

typedef struct hash_table {
    int size;
    int load;
    Node *items_list[];
} HashTable;

/***
* Function declarations
***/
HashTable* init(int size);
void print_table(HashTable *hash_table);
void print_item(Item *item);


/***
* Program
***/
int main() {
    HashTable *hash_table = init(8);
    print_table(hash_table);
    return 0;
}

HashTable* init(int size) {
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->load = 0;

    int i;
    for (i = 0; i < size; i++) {
        printf("Creating bin %i.\n", i);
        Node *null_node = malloc(sizeof(Node));
        null_node->data = malloc(sizeof(Item));
        null_node->data = NULL;
        null_node->next = malloc(sizeof(Node));
        null_node->next = NULL;
        hash_table->items_list[i] = null_node;
    }
    return hash_table;
}

// add

// remove

// hash

// resize

void print_table(HashTable *hash_table) {
    printf("\nTable--------\n-Array size: %i\n-Load: %i\n", hash_table->size, hash_table->load);

    int i;
    for (i = 0; i < hash_table->size; i++) {
        printf("*Bin %i\n", i);
        Node *current_node = hash_table->items_list[i];
        printf("%i\n", current_node->next == NULL);
        if (current_node->next == NULL) {
            printf("(empty)\n");
        }
        else {
            while (current_node->next != NULL) {
                print_item(current_node->data);
                current_node = current_node->next;
            }
        }
    }
}

void print_item(Item *item) {
    printf("-Hash: %i\n-Key: %i\n-Value: %s\n", item->hash, item->key.i, item->value.str); // TODO change to print union
}
