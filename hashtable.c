#include "hashtable.h"

/***
* Creates a new hash table, with all bins initialized to NULL
***/
HashTable *init(int size, float max_load_proportion) {
    HashTable *hash_table = malloc(sizeof(HashTable));
    hash_table->size = size;
    hash_table->max_load_proportion = max_load_proportion;
    hash_table->load = 0;
    hash_table->bin_list = malloc(size*sizeof(Node*));

    int i;
    for (i = 0; i < size; i++) {
        Node *null_node;
        null_node = NULL;
        hash_table->bin_list[i] = null_node;
    }
    return hash_table;
}

int calculate_hash(union Hashable key, hash_type key_type) {
    int hash;
    switch (key_type) {
        case INTEGER:
            hash = key.i;
            break;
        case FLOAT:
            hash = floor(key.f);
            break;
        case STRING:
            hash = strlen(key.str);
            break;
        default:
            hash = 0;
            break;
    }
    return hash;
     // TODO actually hash the keys
}

int calculate_bin_index(int hash, int size) {
    int bin_index;
    if (hash < 0) {
        bin_index = (hash % size) + size;
    }
    else if (hash == 0) {
        bin_index = 0;
    }
    else {
        bin_index = hash % size;
    }
    return bin_index;
}

int max_load_reached(HashTable *hash_table) {
    return (((float)(hash_table->load + 1) / (float)hash_table->size) > hash_table->max_load_proportion);
}

int hashable_equal(union Hashable h1, hash_type type1, union Hashable h2, hash_type type2) {
    if (type1 != type2) {
        return 0;
    }
    else {
        switch (type1) {
            case INTEGER:
                return (h1.i == h2.i);
            case FLOAT:
                return (h1.f == h2.f);
            case STRING:
                return (strcmp(h1.str, h2.str) == 0);
            default:
                return 0;
        }
    }
}

/***
* Adds a key, value pair to hashtable, resizing if hashtable's max_load has been reached
*   If a key's hash has not yet been computed, the hash should be set to HUGE_VAL.
*   This seems hacky, but it's to avoid re-hashing keys when the hashtable is resized.
***/
HashTable *add(int hash, union Hashable key, hash_type key_type, union Hashable value, hash_type value_type, HashTable *hash_table) {
    if (max_load_reached(hash_table)){
        hash_table = resize(hash_table);
    }

    if (hash == (int)HUGE_VAL) {
        hash = calculate_hash(key, key_type);
    }

    Item *item = malloc(sizeof(Item));
    item->hash = hash;
    item->key = key;
    item->key_type = key_type;
    item->value = value;
    item->value_type = value_type;

    hash_table = add_item_to_table(item, hash_table);

    return hash_table;
}

/***
* Determines which bin a new item should be added to.
***/
HashTable *add_item_to_table(Item *item, HashTable *hash_table) {
    int bin_index = calculate_bin_index(item->hash, hash_table->size);

    Node *bin_list = hash_table->bin_list[bin_index];
    hash_table->bin_list[bin_index] = add_item_to_bin(item, bin_list);

    hash_table->load++;
    return hash_table;
}

/***
* Adds item to the linked list at the given bin
*   If the new item has the same key as an existing item, the value is updated.
***/
Node *add_item_to_bin(Item *item, Node *bin_list) {
    Node *head = bin_list;
    Node *prev_node;
    Node *current_node = bin_list;
    if (current_node == NULL) {
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        return new;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (hashable_equal(current_item->key, current_item->key_type, item->key, item->key_type)) {
                current_item->value = item->value;
                return head;
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        prev_node->next = new;
        return head;
    }
}

/***
* Returns item associated with the given key, or NULL if no such item exists.
***/
Item *lookup(union Hashable key, hash_type key_type, HashTable *hash_table) {
    int hash = calculate_hash(key, key_type);
    int bin_index = calculate_bin_index(hash, hash_table->size);

    Node *current_node = hash_table->bin_list[bin_index];

    if (current_node == NULL) {
        return NULL;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (hashable_equal(current_item->key, current_item->key_type, key, key_type)) {
                return current_item;
            }
            current_node = current_node->next;
        }
    }
    return NULL;
}

/***
* Removes and returns item with given key from hashtable, or NULL if no such item exists.
***/
Item *remove_item_from_table(union Hashable key, hash_type key_type, HashTable *hash_table) {
    int hash = calculate_hash(key, key_type);
    int bin_index = calculate_bin_index(hash, hash_table->size);

    Node *bin_list = hash_table->bin_list[bin_index];

    Item *removed = lookup(key, key_type, hash_table);
    if (removed != NULL) {
        hash_table->bin_list[bin_index] = remove_item_from_bin(key, key_type, bin_list);
        hash_table->load--;
    }
    return removed;
}

/***
* Removes item from the linked list at the given bin
*   If the new item has the same key as an existing item, the value is updated.
***/
Node *remove_item_from_bin(union Hashable key, hash_type key_type, Node *bin_list) {
    Node *head = bin_list;
    Node *prev_node = bin_list;
    Node *current_node = bin_list;
    if (current_node != NULL) {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (hashable_equal(current_item->key, current_item->key_type, key, key_type)) {
                if (current_node == head) {
                    Node *temp = current_node->next;
                    free(current_node);
                    return temp;
                }
                else {
                    prev_node->next = current_node->next;
                    free(current_node);
                    return head;
                }
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
    }
    printf("Item not found\n");
    return head;
}

/***
* Creates a new hashtable with twice as many bins as the initial hashtable.
*   All items are transferred to the new hashtable.
***/
HashTable *resize(HashTable *old_hash_table) {
    printf("Resizing from %i to %i.\n", old_hash_table->size, 2*old_hash_table->size);
    HashTable *new_hash_table = init(2*old_hash_table->size, old_hash_table->max_load_proportion);
    new_hash_table->load = 0;

    int i;
    for (i = 0; i < old_hash_table->size; i++) {
        Node *current_node = old_hash_table->bin_list[i];
        while (current_node != NULL) {
            new_hash_table = add_item_to_table(current_node->item, new_hash_table);
            Node *temp = current_node->next;
            free(current_node);
            current_node = temp;
        }
    }
    free(old_hash_table->bin_list);
    free(old_hash_table);
    return new_hash_table;
}


/***
* Helper functions to print hashtables and data items
***/
void print_table(HashTable *hash_table) {
    printf("\n********************\n--------HashTable--------\n-Array size: %i -Load: %i -Max Load Prop: %f -Current Load Prop: %f\n", hash_table->size, hash_table->load, hash_table->max_load_proportion, ((float)hash_table->load / (float)hash_table->size));

    int i;
    for (i = 0; i < hash_table->size; i++) {
        printf("*Bin %i\n", i);
        Node *current_node = hash_table->bin_list[i];
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
        printf("---------Hash: %i---Key: ", item->hash);
        switch (item->key_type) {
            case INTEGER:
                printf("%i", item->key.i);
                break;
            case FLOAT:
                printf("%f", item->key.f);
                break;
            case STRING:
                printf("%s", item->key.str);
                break;
            default:
                printf("oops");
                break;
        }
        printf("---Value: ");
        switch (item->value_type) {
            case INTEGER:
                printf("%i", item->value.i);
                break;
            case FLOAT:
                printf("%f", item->value.f);
                break;
            case STRING:
                printf("%s", item->value.str);
                break;
            default:
                printf("oops");
                break;
        }
        printf("------\n");
    }
}

void free_table(HashTable *hash_table) {
    int i;
    for (i = 0; i < hash_table->size; i++) {
        Node *current_node = hash_table->bin_list[i];
        while (current_node != NULL) {
            free_item(current_node->item);
            Node *temp_node = current_node->next;
            free(current_node);
            current_node = temp_node;
        }
    }
    free(hash_table->bin_list);
    free(hash_table);
}

void free_item(Item *item) {
    if (item->key_type == STRING) {
        free(item->key.str);
    }
    if (item->value_type == STRING) {
        free(item->value.str);
    }
    free(item);
}


/***
* Woo printf testing
***/
int main() {
    // HashTable *hash_table = init(4, 0.5);
    // print_table(hash_table);


    /***
    * Tests for add, lookup, removal, linked_lists, and resizing with integer keys and values
    ***/
    // union Hashable key;
    // union Hashable value;
    // value.str = malloc(50);
    // snprintf(value.str, 50, "hello");

    // printf("~~~~~~~~~~~~~~~ Testing Add ~~~~~~~~~~~~~~~\n");
    // key.i = 0;
    // value.i = 0;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 1;
    // value.i = 1;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 2;
    // value.i = 2;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 3;
    // value.i = 3;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 7;
    // value.i = 4;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 277;
    // value.i = 4;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 277;
    // value.i = 5;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // printf("~~~~~~~~~~~~~~~ Testing Lookup ~~~~~~~~~~~~~~~\n");
    // printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // Item *found = lookup(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(found);
    //
    // key.i = 8;
    // printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // Item *not_found = lookup(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(not_found);
    //
    // printf("~~~~~~~~~~~~~~~ Testing Removal ~~~~~~~~~~~~~~~\n");
    // key.i = 7;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // Item *removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = lookup(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 3;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // removed = lookup(key, INTEGER, hash_table);
    // printf("\n~~~~~Looked up key %i: ", key.i);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 2;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // key.i = 1;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // key.i = 0;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 0;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // print_item(removed);
    //
    // printf("~~~~~~~~~~~~~~~ Testing LinkedLists ~~~~~~~~~~~~~~~\n");
    // key.i = 5;
    // value.i = 5;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 21;
    // value.i = 21;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 53;
    // value.i = 53;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 37;
    // value.i = 37;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    //
    // key.i = 53;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 37;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 277;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 5;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);
    //
    // key.i = 21;
    // printf("\n~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    // removed = remove_item_from_table(key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);



    //
    // printf("~~~~~~~~~~~~~~~ Testing Strings and Mixing Types ~~~~~~~~~~~~~~~\n");
    //
    // union Hashable str_key;
    // union Hashable str_value;
    //
    // str_key.str = malloc(50);
    // snprintf(str_key.str, 50, "hello");
    //
    // str_value.str = malloc(50);
    // snprintf(str_value.str, 50, "world");
    //
    // printf("\n~~~~~Adding %s -- %s (hash: %i)\n", str_key.str, str_value.str, calculate_hash(str_key, STRING));
    // hash_table = add(HUGE_VAL, str_key, STRING, str_value, STRING, hash_table);
    // print_table(hash_table);
    //
    // union Hashable str_key2;
    // union Hashable str_value2;
    //
    // str_key2.str = malloc(50);
    // snprintf(str_key2.str, 50, "Sophia");
    //
    // str_value2.str = malloc(50);
    // snprintf(str_value2.str, 50, "is still at Hacker School");
    //
    //
    // printf("\n~~~~~Adding %s -- %s (hash: %i)\n", str_key2.str, str_value2.str, calculate_hash(str_key2, STRING));
    // hash_table = add(HUGE_VAL, str_key2, STRING, str_value2, STRING, hash_table);
    // print_table(hash_table);
    //
    // union Hashable float_key;
    //
    // float_key.f = 2.2;
    //
    // printf("\n~~~~~Adding %f -- %s (hash: %i)\n", float_key.f, str_value2.str, calculate_hash(float_key, FLOAT));
    // hash_table = add(HUGE_VAL, float_key, FLOAT, str_value2, STRING, hash_table);
    // print_table(hash_table);
    //
    // union Hashable int_key;
    //
    // int_key.i = 23;
    //
    // printf("\n~~~~~Adding %i -- %s (hash: %i)\n", int_key.i, str_value2.str, calculate_hash(int_key, INTEGER));
    // hash_table = add(HUGE_VAL, int_key, INTEGER, str_value2, STRING, hash_table);
    // print_table(hash_table);
    //
    // printf("\n~~~~~Looking for %s (hash: %i)\n", str_key.str, calculate_hash(str_key, STRING));
    // Item *found = lookup(str_key, STRING, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(found);
    //
    // printf("\n~~~~~Looking for %f (hash: %i)\n", float_key.f, calculate_hash(float_key, FLOAT));
    // found = lookup(float_key, FLOAT, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(found);
    //
    // printf("\n~~~~~Looking for %i (hash: %i)\n", int_key.i, calculate_hash(int_key, INTEGER));
    // found = lookup(int_key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(found);
    //
    // printf("\n~~~~~Removing %s (hash: %i)\n", str_key.str, calculate_hash(str_key, STRING));
    // Item *removed = remove_item_from_table(str_key, STRING, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // printf("\n~~~~~Removing %f (hash: %i)\n", float_key.f, calculate_hash(float_key, FLOAT));
    // removed = remove_item_from_table(float_key, FLOAT, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // printf("\n~~~~~Removing %i (hash: %i)\n", int_key.i, calculate_hash(int_key, INTEGER));
    // removed = remove_item_from_table(int_key, INTEGER, hash_table);
    // printf("\n~~~~~Got: ");
    // print_item(removed);
    //
    // print_table(hash_table);

    printf("~~~~~~~~~~~~~~~ Testing Freeing Memory ~~~~~~~~~~~~~~~\n");

    // make a hashtable and free it -- no memory leaks
    // HashTable *hash_table = init(4, 0.5);
    // print_table(hash_table);
    // free_table(hash_table);

    /*
    * test adding one element (no resize) and freeing -- no memory leaks
    */
    // HashTable *hash_table = init(4, 0.5);
    // union Hashable key;
    // union Hashable value;
    //
    // key.i = 0;
    // value.i = 0;
    // printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    // hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);
    // print_table(hash_table);
    // free_table(hash_table);

    /*
    * test adding one string (no resize) and freeing -- no memory leaks
    */
    // HashTable *hash_table = init(4, 0.5);
    // union Hashable str_key;
    // union Hashable str_value;
    //
    // str_key.str = malloc(50);
    // snprintf(str_key.str, 50, "hello");
    // str_value.str = malloc(50);
    // snprintf(str_value.str, 50, "world");
    //
    // printf("\n~~~~~Adding %s -- %s (hash: %i)\n", str_key.str, str_value.str, calculate_hash(str_key, 2));
    // hash_table = add(HUGE_VAL, str_key, STRING, str_value, STRING, hash_table);
    // print_table(hash_table);
    // free_table(hash_table);

    /*
    * test adding enough to resize and freeing -- no memory leaks
    */
    HashTable *hash_table = init(4, 0.5);
    union Hashable key;
    union Hashable value;

    key.i = 0;
    value.i = 0;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);

    key.i = 3;
    value.i = 3;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);

    key.i = 2;
    value.i = 2;
    printf("\n~~~~~Adding %i -- %i (hash: %i)\n", key.i, value.i, calculate_hash(key, INTEGER));
    hash_table = add(HUGE_VAL, key, INTEGER, value, INTEGER, hash_table);

    union Hashable str_key;
    union Hashable str_value;

    str_key.str = malloc(50);
    snprintf(str_key.str, 50, "hello");
    str_value.str = malloc(50);
    snprintf(str_value.str, 50, "world");

    printf("\n~~~~~Adding %s -- %s (hash: %i)\n", str_key.str, str_value.str, calculate_hash(str_key, STRING));
    hash_table = add(HUGE_VAL, str_key, STRING, str_value, STRING, hash_table);

    str_key.str = malloc(50);
    snprintf(str_key.str, 50, "чебурашка");
    str_value.str = malloc(50);
    snprintf(str_value.str, 50, "крокодил гена");

    printf("\n~~~~~Adding %s -- %s (hash: %i)\n", str_key.str, str_value.str, calculate_hash(str_key, 2));
    hash_table = add(HUGE_VAL, str_key, STRING, str_value, STRING, hash_table);

    print_table(hash_table);
    // free_table(hash_table);

    /*
    * test lookup and removal -- no memory leaks (use dictionary from previous test)
    */
    key.i = 0;
    printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    Item *found = lookup(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    printf("~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    Item *removed = remove_item_from_table(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(removed);
    free_item(removed);

    key.i = 2;
    printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    found = lookup(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    printf("~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    removed = remove_item_from_table(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(removed);
    free_item(removed);

    key.i = 3;
    printf("\n~~~~~Looking for %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    found = lookup(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    printf("~~~~~Removing %i (hash: %i)\n", key.i, calculate_hash(key, INTEGER));
    removed = remove_item_from_table(key, INTEGER, hash_table);
    printf("~~~~~Got: ");
    print_item(removed);
    free_item(removed);


    union Hashable str_key_lookup;

    str_key_lookup.str = malloc(50);
    snprintf(str_key_lookup.str, 50, "hello");

    printf("\n~~~~~Looking for %s (hash: %i)\n", str_key_lookup.str, calculate_hash(str_key_lookup, STRING));
    found = lookup(str_key_lookup, STRING, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    printf("~~~~~Removing %s (hash: %i)\n", str_key_lookup.str, calculate_hash(str_key_lookup, STRING));
    removed = remove_item_from_table(str_key_lookup, STRING, hash_table);
    printf("~~~~~Got: ");
    print_item(removed);
    free_item(removed);
    free(str_key_lookup.str);

    str_key_lookup.str = malloc(50);
    snprintf(str_key_lookup.str, 50, "чебурашка");

    printf("\n~~~~~Looking for %s (hash: %i)\n", str_key_lookup.str, calculate_hash(str_key_lookup, STRING));
    found = lookup(str_key_lookup, STRING, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    printf("~~~~~Removing %s (hash: %i)\n", str_key_lookup.str, calculate_hash(str_key_lookup, STRING));
    removed = remove_item_from_table(str_key_lookup, STRING, hash_table);
    printf("~~~~~Got: ");
    print_item(removed);
    free_item(removed);
    free(str_key_lookup.str);

    printf("\n~~~~~Looking for %s (hash: %i)\n", str_key_lookup.str, calculate_hash(str_key_lookup, STRING));
    found = lookup(str_key_lookup, STRING, hash_table);
    printf("~~~~~Got: ");
    print_item(found);

    print_table(hash_table);
    free_table(hash_table);

    return 0;
}
