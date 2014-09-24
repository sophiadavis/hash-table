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
   char *str;
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
    Node **items_list;
} HashTable;

/***
* Function declarations
***/
HashTable* init(int size);
void print_table(HashTable *hash_table);
void print_item(Item *item);

int calculate_hash(union Hashable key);

void add_item_to_table(union Hashable key, union Hashable value, HashTable *hash_table);
Node *add_item_to_bin(Item *item, Node *items_list);

Item *lookup(union Hashable key, HashTable *hash_table);

Item *remove_item_from_table(union Hashable key, HashTable *hash_table);
Node *remove_item_from_bin(union Hashable key, Node *items_list);


/***
* Program
***/
int main() {
    HashTable *hash_table = init(4);
    print_table(hash_table);

    union Hashable key;
    union Hashable value;
    // value.str = malloc(50);
    // snprintf(value.str, 50, "hello");

    printf("~~~~~~~~~~~~~~~ Testing Add ~~~~~~~~~~~~~~~\n");
    key.i = 0;
    value.i = 0;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    // key.i = 1;
    // value.i = 1;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    // add_item_to_table(key, value, hash_table);
    // print_table(hash_table);

    key.i = 2;
    value.i = 2;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    key.i = 3;
    value.i = 3;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    key.i = 7;
    value.i = 4;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key));
    add_item_to_table(key, value, hash_table);
    print_table(hash_table);

    printf("~~~~~~~~~~~~~~~ Testing Lookup ~~~~~~~~~~~~~~~\n");
    Item *found = lookup(key, hash_table);
    print_item(found);

    key.i = 8;
    Item *not_found = lookup(key, hash_table);
    print_item(not_found);

    printf("~~~~~~~~~~~~~~~ Testing Removal ~~~~~~~~~~~~~~~\n");
    key.i = 7;
    Item *removed = remove_item_from_table(key, hash_table);
    print_item(removed);

    removed = lookup(key, hash_table);
    print_item(removed);

    print_table(hash_table);

    key.i = 3;
    removed = remove_item_from_table(key, hash_table);
    print_item(removed);

    removed = lookup(key, hash_table);
    print_item(removed);

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

        Node *null_node;
        null_node = NULL;

        hash_table->items_list[i] = null_node;
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

    int bin_index = hash < hash_table->size ? hash : hash % hash_table->size;
    Node *items_list = hash_table->items_list[bin_index];

    hash_table->items_list[bin_index] = add_item_to_bin(item, items_list);

    hash_table->load++;
}

Node *add_item_to_bin(Item *item, Node *items_list) {
    Node *head = items_list;
    Node *prev_node;
    Node *current_node = items_list;
    if (current_node == NULL) {
        printf("Adding to empty bin\n");
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        return new;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
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

Item *lookup(union Hashable key, HashTable *hash_table) {
    int hash = calculate_hash(key);
    int bin_index = hash < hash_table->size ? hash : hash % hash_table->size;

    Node *current_node = hash_table->items_list[bin_index];

    if (current_node == NULL) {
        printf("Item not found\n");
        return NULL;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (current_item->key.i == key.i) {
                printf("Found value %i for key %i\n", current_item->value.i, key.i);
                return current_item;
            }
            current_node = current_node->next;
        }
    }
    printf("Item not found\n");
    return NULL;
}

Item *remove_item_from_table(union Hashable key, HashTable *hash_table) {
    int hash = calculate_hash(key);
    int bin_index = hash < hash_table->size ? hash : hash % hash_table->size;

    Node *items_list = hash_table->items_list[bin_index];

    Item *removed = lookup(key, hash_table);
    if (removed != NULL) {
        hash_table->items_list[bin_index] = remove_item_from_bin(key, items_list);
        hash_table->load--;
    }

    return removed;
}

Node *remove_item_from_bin(union Hashable key, Node *items_list) {
    Node *head = items_list;
    Node *prev_node = items_list;
    Node *current_node = items_list;
    if (current_node == NULL) {
        printf("Item not found\n");
        return head;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (current_item->key.i == key.i) {
                printf("Removing key %i (value %i)\n", key.i, current_item->value.i);
                if (current_node == head) {
                    return current_node->next;
                }
                else {
                    prev_node->next = current_node->next;
                    return head;
                }
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
        printf("Item not found\n");
        return head;
    }
}
// hash

// resize

void print_table(HashTable *hash_table) {
    printf("\n********************\n--------HashTable--------\n-Array size: %i\n-Load: %i\n", hash_table->size, hash_table->load);

    int i;
    for (i = 0; i < hash_table->size; i++) {
        printf("*Bin %i\n", i);
        Node *current_node = hash_table->items_list[i];
        if (current_node == NULL) {
            printf("(empty)\n");
        }
        else {
            while (current_node != NULL) {
                print_item(current_node->item);
                current_node = current_node->next;
            }
        }
    }
    printf("********************\n");
}

void print_item(Item *item) {
    if (item == NULL) {
        printf("------NULL\n");
    }
    else {
        printf("---------Hash: %i---Key: %i---Value: %i------\n", item->hash, item->key.i, item->value.i); // TODO change to print union
    }
}
