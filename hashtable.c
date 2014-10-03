#include "hashtable.h"

/***
* Creates a new hash table, with all bins initialized to NULL
***/
HashTable *init(long int size, double max_load_proportion) {
    HashTable *hashtable = malloc(sizeof(HashTable));
    hashtable->size = size;
    hashtable->max_load_proportion = max_load_proportion;
    hashtable->load = 0;
    hashtable->bin_list = malloc(size*sizeof(Node*));

    long int i;
    for (i = 0; i < size; i++) {
        Node *null_node;
        null_node = NULL;
        hashtable->bin_list[i] = null_node;
    }
    return hashtable;
}

long int calculate_hash(union Hashable key, hash_type key_type) {
    long int hash;
    switch (key_type) {
        case INTEGER:
            hash = key.i;
            break;
        case DOUBLE:
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

long int calculate_bin_index(long int hash, long int size) {
    long int bin_index;
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

int max_load_reached(HashTable *hashtable) {
    return (((double)(hashtable->load + 1) / (double)hashtable->size) > hashtable->max_load_proportion);
}

int hashable_equal(union Hashable h1, hash_type type1, union Hashable h2, hash_type type2) {
    if (type1 != type2) {
        return 0;
    }
    else {
        switch (type1) {
            case INTEGER:
                return (h1.i == h2.i);
            case DOUBLE:
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
HashTable *add(long int hash, union Hashable key, hash_type key_type, union Hashable value, hash_type value_type, HashTable *hashtable) {
    if (max_load_reached(hashtable)){
        hashtable = resize(hashtable);
    }

    if (hash == (long int)HUGE_VAL) {
        hash = calculate_hash(key, key_type);
    }

    Item *item = malloc(sizeof(Item));
    item->hash = hash;
    item->key = key;
    item->key_type = key_type;
    item->value = value;
    item->value_type = value_type;

    hashtable = add_item_to_table(item, hashtable);

    return hashtable;
}

/***
* Determines which bin a new item should be added to.
***/
HashTable *add_item_to_table(Item *item, HashTable *hashtable) {
    int bin_index = calculate_bin_index(item->hash, hashtable->size);

    Node *bin_list = hashtable->bin_list[bin_index];
    hashtable->bin_list[bin_index] = add_item_to_bin(item, bin_list, hashtable);

    return hashtable;
}

/***
* Adds item to the linked list at the given bin
*   If the new item has the same key as an existing item, the value is updated.
***/
Node *add_item_to_bin(Item *item, Node *bin_list, HashTable *hashtable) {
    Node *head = bin_list;
    Node *prev_node;
    Node *current_node = bin_list;
    if (current_node == NULL) {
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        hashtable->load++;
        return new;
    }
    else {
        while (current_node != NULL) {
            Item *current_item = current_node->item;
            if (hashable_equal(current_item->key, current_item->key_type, item->key, item->key_type)) {
                // keys are equal -- replace
                free_item(current_item);
                current_node->item = item;
                return head;
            }
            prev_node = current_node;
            current_node = current_node->next;
        }
        Node *new = malloc(sizeof(Node));
        new->item = item;
        new->next = current_node;
        prev_node->next = new;
        hashtable->load++;
        return head;
    }
}

/***
* Returns item associated with the given hash and key, or NULL if no such item exists.
***/
Item *lookup_by_hash(long int hash, union Hashable key, hash_type key_type, HashTable *hashtable) {
    long int bin_index = calculate_bin_index(hash, hashtable->size);

    Node *current_node = hashtable->bin_list[bin_index];

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
* Returns item associated with the given key, or NULL if no such item exists.
***/
Item *lookup(union Hashable key, hash_type key_type, HashTable *hashtable) {
    long int hash = calculate_hash(key, key_type);
    return lookup_by_hash(hash, key, key_type, hashtable);
}

/***
* Removes and returns item with given hash and key from hashtable, or NULL if no such item exists.
***/
Item *remove_item_from_table_by_hash(long int hash, union Hashable key, hash_type key_type, HashTable *hashtable) {
    long int bin_index = calculate_bin_index(hash, hashtable->size);

    Node *bin_list = hashtable->bin_list[bin_index];

    Item *removed = lookup_by_hash(hash, key, key_type, hashtable);
    if (removed != NULL) {
        hashtable->bin_list[bin_index] = remove_item_from_bin(key, key_type, bin_list);
        hashtable->load--;
    }
    return removed;
}

/***
* Removes and returns item with given key from hashtable, or NULL if no such item exists.
***/
Item *remove_item_from_table(union Hashable key, hash_type key_type, HashTable *hashtable) {
    long int hash = calculate_hash(key, key_type);
    return remove_item_from_table_by_hash(hash, key, key_type, hashtable);
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
HashTable *resize(HashTable *old_hashtable) {
    HashTable *new_hashtable = init(2*old_hashtable->size, old_hashtable->max_load_proportion);
    new_hashtable->load = 0;

    long int i;
    for (i = 0; i < old_hashtable->size; i++) {
        Node *current_node = old_hashtable->bin_list[i];
        while (current_node != NULL) {
            new_hashtable = add_item_to_table(current_node->item, new_hashtable);
            Node *temp = current_node->next;
            free(current_node);
            current_node = temp;
        }
    }
    free(old_hashtable->bin_list);
    free(old_hashtable);
    return new_hashtable;
}


/***
* Helper functions to print hashtables and data items
***/
void print_table(HashTable *hashtable) {
    printf("\n********************\n--------HashTable--------\n-Array size: "
           "%li -Load: %li -Max Load Prop: %f -Current Load Prop: %f\n",
           hashtable->size,
           hashtable->load,
           hashtable->max_load_proportion,
           ((double)hashtable->load / (double)hashtable->size));

    long int i;
    for (i = 0; i < hashtable->size; i++) {
        printf("*Bin %li\n", i);
        Node *current_node = hashtable->bin_list[i];
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
        printf("---------Hash: %li---Key: ", item->hash);
        switch (item->key_type) {
            case INTEGER:
                printf("%li", item->key.i);
                break;
            case DOUBLE:
                printf("%f", item->key.f);
                break;
            case STRING:
                printf("%s", item->key.str);
                break;
        }
        printf("---Value: ");
        switch (item->value_type) {
            case INTEGER:
                printf("%li", item->value.i);
                break;
            case DOUBLE:
                printf("%f", item->value.f);
                break;
            case STRING:
                printf("%s", item->value.str);
                break;
        }
        printf("------\n");
    }
}

char *stringify_table(HashTable *hashtable) {
    int max_len = 200 + 200 * hashtable->load; // a guess
    int len = 0;
    char *hashtable_string = malloc(max_len);
    char *item_string;

    len = len + snprintf(hashtable_string, max_len,
        "\n********************\n--------HashTable--------\n-Array size: "
        "%li -Load: %li -Max Load Prop: %f -Current Load Prop: %f\n",
        hashtable->size,
        hashtable->load,
        hashtable->max_load_proportion,
        ((double)hashtable->load / (double)hashtable->size));

    long int i;
    for (i = 0; i < hashtable->size; i++) {
        len = len + snprintf(hashtable_string + len, max_len - len, "*Bin %li\n", i);
        Node *current_node = hashtable->bin_list[i];
        if (current_node == NULL) {
            len = len + snprintf(hashtable_string + len, max_len - len, "(empty)\n");
        }
        else {
            while (current_node != NULL) {
                item_string = stringify_item(current_node->item);
                len = len + snprintf(hashtable_string + len, max_len - len, "%s", item_string);
                free(item_string);
                current_node = current_node->next;
            }
        }
    }
    len = len + snprintf(hashtable_string + len, max_len - len, "********************\n");
    return hashtable_string;
}

char *stringify_item(Item *item) {
    int max_len = 200;
    int len = 0;
    char *item_string = malloc(max_len);
    if (item == NULL) {
        len = len + snprintf(item_string + len, max_len - len, "------NULL\n");
    }
    else {
        len = len + snprintf(item_string + len, max_len - len, "---------Hash: %li---Key: ", item->hash);
        switch (item->key_type) {
            case INTEGER:
                len = len + snprintf(item_string + len, max_len - len, "%li", item->key.i);
                break;
            case DOUBLE:
                len = len + snprintf(item_string + len, max_len - len, "%f", item->key.f);
                break;
            case STRING:
                len = len + snprintf(item_string + len, max_len - len, "%s", item->key.str);
                break;
        }
        len = len + snprintf(item_string + len, max_len - len, "---Value: ");
        switch (item->value_type) {
            case INTEGER:
                len = len + snprintf(item_string + len, max_len - len, "%li", item->value.i);
                break;
            case DOUBLE:
                len = len + snprintf(item_string + len, max_len - len, "%f", item->value.f);
                break;
            case STRING:
                len = len + snprintf(item_string + len, max_len - len, "%s", item->value.str);
                break;
        }
        len = len + snprintf(item_string + len, max_len - len, "------\n");
    }
    return item_string;
}

void free_table(HashTable *hashtable) {
    long int i;
    for (i = 0; i < hashtable->size; i++) {
        Node *current_node = hashtable->bin_list[i];
        while (current_node != NULL) {
            free_item(current_node->item);
            Node *temp_node = current_node->next;
            free(current_node);
            current_node = temp_node;
        }
    }
    free(hashtable->bin_list);
    free(hashtable);
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
* Examples
***/
int main() {
    /***********
    * Creating a hashtable
    ***********/
    long int initial_load = 4;
    double max_load_proportion = 0.5;
    HashTable *hashtable = init(initial_load, max_load_proportion);

    /***********
    * Add some key-value pairs
    ***********/
    printf("\n################## ADDING ##################\n");
    union Hashable key;
    union Hashable value;

    // Add some integers
    key.i = 0;
    value.i = 0;
    int key_type = INTEGER;
    int value_type = INTEGER;
    long int hash = HUGE_VAL; // pass in HUGE_VAL to use pitiful default hash function

    printf("\n~~~~~Adding %li -- %li (hash: %li)\n", key.i, value.i, calculate_hash(key, key_type));
    hashtable = add(hash, key, key_type, value, value_type, hashtable);

    key.i = 2;
    value.i = 10;
    printf("\n~~~~~Adding %li -- %li (hash: %li)\n", key.i, value.i, calculate_hash(key, key_type));
    hashtable = add(hash, key, key_type, value, value_type, hashtable);

    // We can use this handy "print_table" function to print the table.
    print_table(hashtable);

    // AND NOW IT'S GONNA RESIZE!! (We'll have added 3 items, for a load proportion of 3/4.
    //                                  We specified our max_load_proportion to be 1/2.)
    print_table(hashtable);

    // Add some DOUBLEs
    key.f = 2.85;
    key_type = DOUBLE;

    printf("\n~~~~~Adding %f -- %li (hash: %li)\n", key.f, value.i, calculate_hash(key, key_type));
    hashtable = add(hash, key, key_type, value, value_type, hashtable);

    // We can use this handy "print_table" function to print the table.
    print_table(hashtable);

    // AND NOW IT'S GONNA RESIZE!! (We'll have added 3 items, for a load proportion of 3/4.
    //                                  We specified our max_load_proportion to be 1/2.)
    // Add some strings
    // If we add a key-value pair and the key already exists in the hashtable,
    //      the old value will be overwritten with the new value.
    value.str = malloc(10);
    value_type = STRING;
    sprintf(value.str, "%s", "hello!");
    printf("\n~~~~~Adding %f -- %s (hash: %li)\n", key.f, value.str, calculate_hash(key, key_type));
    hashtable = add(hash, key, key_type, value, STRING, hashtable);

    union Hashable str_key2;
    union Hashable str_value2;

    str_key2.str = malloc(50);
    snprintf(str_key2.str, 50, "чебурашка");
    str_value2.str = malloc(50);
    snprintf(str_value2.str, 50, "крокодил гена");

    printf("\n~~~~~Adding %s -- %s (hash: %li)\n", str_key2.str, str_value2.str, calculate_hash(str_key2, 2));
    hashtable = add(HUGE_VAL, str_key2, STRING, str_value2, STRING, hashtable);

    // We can write the results of print_hashtable to a string.
    char *str = stringify_table(hashtable);
    printf("Here's a string representing our hashtable:\n %s", str);
    // Now you've gotta free the string, though:
    free(str);


    /***********
    * Let's look some things up!
    ***********/
    printf("\n################## LOOKUP ##################\n");
    key.f = 2.85;
    key_type = DOUBLE;

    Item *looked_up = lookup(key, key_type, hashtable);

    // We can use the print_item function to see what we got:
    print_item(looked_up);
    // There's also the function stringify_item, which returns a string.


    // Notice that you don't need to free items that you looked up --
    //      the hashtable is still storing them.

    /***********
    * Finally, we can remove things.
    * You will need to free every item removed from the hashtable.
    * I've provided a nice "free_item" function!
    ***********/
    printf("\n################## REMOVAL ##################\n");
    key_type = INTEGER;
    key.i = 0;
    Item *removed = remove_item_from_table(key, key_type, hashtable);
    print_item(removed);
    free_item(removed);

    key.i = 2;
    removed = remove_item_from_table(key, key_type, hashtable);
    print_item(removed);
    free_item(removed);

    key.f = 2.85;
    key_type = DOUBLE;
    removed = remove_item_from_table(key, key_type, hashtable);
    print_item(removed);
    free_item(removed);

    key.str = "чебурашка";
    key_type = STRING;
    removed = remove_item_from_table(key, key_type, hashtable);
    print_item(removed);
    free_item(removed);

    // And don't forget to
    free_table(hashtable);
    return 0;
}
