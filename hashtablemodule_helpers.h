#include "hashtable.h"

int set_hashable_from_user_input(union Hashable *to_set, hash_type *type, PyObject* input);
void free_hashable(union Hashable hashable, hash_type type);
PyObject* format_python_return_val_from_item(Item *item);
long int get_hash(union Hashable key, hash_type type, PyObject *hash_func);
PyObject *default_py_hash_funcs(void);
