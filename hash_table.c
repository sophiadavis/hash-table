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
    Item *item;
    struct node *next;
} Node;

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
int calculate_hash(union Hashable key);
void add_item_to_table(union Hashable key, union Hashable value, HashTable *hash_table);
Node *update_bin(Item *item, Node *items_list);


/***
* Program
***/
int main() {
    HashTable *hash_table = init(4);
    print_table(hash_table);

    union Hashable key;
    union Hashable value;

    key.i = 0;
    value.i = 0;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    key.i = 1;
    value.i = 1;
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    key.i = 2;
    value.i = 2;
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    key.i = 3;
    value.i = 3;
    add_item_to_table(key, value, hash_table);
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

        null_node->item = malloc(sizeof(Item));
        null_node->item = NULL;
        null_node->next = malloc(sizeof(Node));
        null_node->next = NULL;

        hash_table->items_list[i] = null_node;
        printf("%i\n", (hash_table->items_list[i]->next == NULL));
        printf("%i\n", (hash_table->items_list[i]->item == NULL));
    }
    return hash_table;
}

int calculate_hash(union Hashable key) {
    return key.i; // TODO actually hash the keys
}

void add_item_to_table(union Hashable key, union Hashable value, HashTable *hash_table) {
    int hash = calculate_hash(key);

    Item *item = malloc(sizeof(Item));
    item->hash = hash;
    item->key = key;
    item->value = value;

    int bin_index = hash < hash_table->size ? hash : hash_table->size % hash;
    Node *items_list = hash_table->items_list[bin_index];

    hash_table->items_list[bin_index] = update_bin(item, items_list);

    hash_table->load++;
}

Node *update_bin(Item *item, Node *items_list) {
    Node *head = items_list;
    Node *prev_node;
    Node *current_node = items_list;
    if (current_node->next == NULL) {
        printf("Adding to empty bin\n");
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        return new;
    }
    else {
        while (current_node->next != NULL) {
            Item *current_item = current_node->item;
            print_item(current_item);
            if (current_item->key.i == item->key.i) {
                printf("Replacing value for key %i\n", item->key.i);
                return head;
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
        printf("Adding to end of bin\n");
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        prev_node->next = new;
        return head;
    }

}

// remove

// hash

// resize

void print_table(HashTable *hash_table) {
    printf("\n********************\nTable--------\n-Array size: %i\n-Load: %i\n", hash_table->size, hash_table->load);

    int i;
    for (i = 0; i < hash_table->size; i++) {
        printf("*Bin %i\n", i);
        Node *current_node = hash_table->items_list[i];
        if (current_node->next == NULL) {
            printf("(empty)\n");
        }
        else {
            while (current_node->next != NULL) {
                print_item(current_node->item);
                current_node = current_node->next;
            }
        }
    }
    printf("********************\n");
}

void print_item(Item *item) {
    printf("---Hash: %i\n---Key: %i\n---Value: %s\n", item->hash, item->key.i, item->value.str); // TODO change to print union
}
