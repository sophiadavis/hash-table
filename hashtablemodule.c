#include <Python.h>
#include "structmember.h"
#include "hashtablemodule_helpers.h"

typedef struct {
    PyObject_HEAD
    HashTable *hashtable;
    long int size;
    long int load;
    double max_load;
    PyObject *hash_func;
} HashTablePyObject;

static int
HashTablePyObject_init(HashTablePyObject *self, PyObject *args, PyObject *kwds)
{
    self->hashtable = NULL;

    long int size = 4;
    double max_load = 0.5;
    PyObject *hash_func = NULL;

    static char *kwlist[] = {"size", "max_load", "hash_func", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|ldO", kwlist, &size, &max_load, &hash_func)) {
        PyErr_SetString(PyExc_TypeError, "Invalid parameters.");
        return -1;
    }
    if (size <= 0) {
        PyErr_SetString(PyExc_TypeError, "size parameter must be a positive integer.");
        return -1;
    }
    if ((max_load <= 0) || (max_load > 1)) {
        PyErr_SetString(PyExc_TypeError, "max_load parameter must be a float between 0.0 and 1.0.");
        return -1;
    }
    if ((hash_func != NULL) && (!PyCallable_Check(hash_func))) {
        PyErr_SetString(PyExc_TypeError, "hash_func must be callable.");
        return -1;
    }

    self->hashtable = init(size, max_load);
    self->size = size;
    self->max_load = max_load;
    self->load = self->hashtable->load;

    if (hash_func == NULL) {
        self->hash_func = default_py_hash_func();
    }
    else {
        self->hash_func = hash_func;
    }

    Py_INCREF(self->hash_func);

    return 0;
}

char size_attr__doc__[] = "Current number of bins in hashtable.";
char load_attr__doc__[] = "Current number of key-value pairs stored in hashtable.";
char max_load_attr__doc__[] = "Maximum proportion of load to size before resizing.";
char hash_func_attr__doc__[] = "Hash function used to determine which bin a key-value pair should be stored in.";

static PyMemberDef Hashtable_members[] = {
    {"size",
        T_LONG, offsetof(HashTablePyObject, size), READONLY,
        size_attr__doc__},
    {"load",
        T_LONG, offsetof(HashTablePyObject, load), READONLY,
        load_attr__doc__},
    {"max_load",
        T_DOUBLE, offsetof(HashTablePyObject, max_load), READONLY,
        max_load_attr__doc__},
    {"hash_func",
        T_OBJECT, offsetof(HashTablePyObject, hash_func), READONLY,
        hash_func_attr__doc__},
    {NULL}  /* Sentinel */
};

static void
HashTablePyObject_dealloc(HashTablePyObject* self)
{
    // DECREF DEMO
    Py_XDECREF(self->hash_func);
    printf("C: ---------> Dealloc-ing\n");
    if (self->hashtable != NULL) {
        self->ob_type->tp_free((PyObject*)self);
    }
}

static void
HashTablePyObject_free(HashTablePyObject* self)
{
    printf("C: ---------> Free-ing\n");
    free_table(self->hashtable);
}


char HashTablePy_set__doc__[] = "Add a key-value pair to the hashtable.";

static PyObject *
HashTablePy_set(HashTablePyObject *self, PyObject *args)
{
    PyObject* key_input = NULL;
    PyObject* value_input = NULL;

    if (!PyArg_ParseTuple(args, "OO", &key_input, &value_input))
        return NULL;

    union Hashable key;
    hash_type key_type = INTEGER; // default
    union Hashable value;
    hash_type value_type = INTEGER;

    if ((set_hashable_from_user_input(&key, &key_type, key_input) < 0) ||
        (set_hashable_from_user_input(&value, &value_type, value_input) < 0)) {
            return NULL;
    }

    long int hash = get_hash(key, key_type, self->hash_func);
    if (hash == LONG_MAX) { // error
        free_hashable(key, key_type);
        free_hashable(value, value_type);
        return NULL;
    }

    self->hashtable = add(hash, key, key_type, value, value_type, self->hashtable);
    self->load = self->hashtable->load;
    Py_RETURN_NONE;
}

char HashTablePy_get__doc__[] = "Lookup the value associated with the given key in the hashtable.";

static PyObject *
HashTablePy_get(HashTablePyObject *self, PyObject *args)
{
    PyObject* key_input = NULL;

    if (!PyArg_ParseTuple(args, "O", &key_input))
        return NULL;

    union Hashable key;
    hash_type key_type = INTEGER; // default

    if (set_hashable_from_user_input(&key, &key_type, key_input) < 0) {
            return NULL;
    }

    long int hash = get_hash(key, key_type, self->hash_func);
    if (hash == LONG_MAX) { // error
        free_hashable(key, key_type);
        return NULL;
    }

    Item *item = lookup_by_hash(hash, key, key_type, self->hashtable);
    PyObject* return_val = format_python_return_val_from_item(item);

    if (key_type == STRING) {
        free(key.str);
    }

    return return_val;
}

char HashTablePy_pop__doc__[] = "Delete the key-value pair associated with given key from the hashtable. The value is returned.";

static PyObject *
HashTablePy_pop(HashTablePyObject *self, PyObject *args)
{
    PyObject* key_input = NULL;

    if (!PyArg_ParseTuple(args, "O", &key_input))
        return NULL;

    union Hashable key;
    hash_type key_type = INTEGER; // default

    if (set_hashable_from_user_input(&key, &key_type, key_input) < 0) {
            return NULL;
    }

    long int hash = get_hash(key, key_type, self->hash_func);
    if (hash == LONG_MAX) { // error
        free_hashable(key, key_type);
        return NULL;
    }

    Item *item = remove_item_from_table_by_hash(hash, key, key_type, self->hashtable);
    PyObject* return_val = format_python_return_val_from_item(item);

    if (key_type == STRING) {
        free(key.str);
    }
    free_item(item);

    self->load = self->hashtable->load;
    return return_val;
}

static int
HashTablePy_print(HashTablePyObject *self, PyObject *args)
{
    print_table_simple(self->hashtable);
    return 0;
}

static PyObject *
HashTablePy_repr(HashTablePyObject *self, PyObject *args)
{
    char *repr = stringify_table_simple(self->hashtable);
    PyObject* py_repr = Py_BuildValue("s", repr);
    free(repr);
    return py_repr;
}

static PyMethodDef HashTablePy_methods[] = {
    {"set", (PyCFunction)HashTablePy_set, METH_VARARGS, HashTablePy_set__doc__},
    {"get", (PyCFunction)HashTablePy_get, METH_VARARGS, HashTablePy_get__doc__},
    {"pop", (PyCFunction)HashTablePy_pop, METH_VARARGS, HashTablePy_pop__doc__},
    {NULL}  /* Sentinel */
};

static PyTypeObject HashTablePyType = {
    PyObject_HEAD_INIT(NULL)
    0,                                           /* ob_size */
    "hashtable.HashTable",                       /* tp_name */
    sizeof(HashTablePyObject),                   /* tp_basicsize */
    0,                                           /* tp_itemsize */
    (destructor)HashTablePyObject_dealloc,       /* tp_dealloc */
    (printfunc)HashTablePy_print,                /* tp_print */
    0,                                           /* tp_getattr */
    0,                                           /* tp_setattr */
    0,                                           /* tp_compare */
    (reprfunc)HashTablePy_repr,                  /* tp_repr */
    0,                                           /* tp_as_number */
    0,                                           /* tp_as_sequence */
    0,                                           /* tp_as_mapping */
    0,                                           /* tp_hash */
    0,                                           /* tp_call */
    0,                                           /* tp_str */
    0,                                           /* tp_getattro */
    0,                                           /* tp_setattro */
    0,                                           /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,                          /* tp_flags */
    "Customizeable HashTable!",                  /* tp_doc */
    0,                                           /* tp_traverse */
    0,                                           /* tp_clear */
    0,                                           /* tp_richcompare */
    0,                                           /* tp_weaklistoffset */
    0,                                           /* tp_iter */
    0,                                           /* tp_iternext */
    HashTablePy_methods,                         /* tp_methods */
    Hashtable_members,                           /* tp_members */
    0,                                           /* tp_getset */
    0,                                           /* tp_base */
    0,                                           /* tp_dict */
    0,                                           /* tp_descr_get */
    0,                                           /* tp_descr_set */
    0,                                           /* tp_dictoffset */
    (initproc)HashTablePyObject_init,            /* tp_init */
    0,                                           /* tp_alloc */
    0,                                           /* tp_new */
    (freefunc)HashTablePyObject_free,            /* tp_free */
};

#ifndef PyMODINIT_FUNC	/* declarations for DLL import/export */
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
inithashtable(void)
{
    PyObject* m;

    HashTablePyType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&HashTablePyType) < 0)
        return;

    static char hashtable__doc__[] = "This module enables users to create "
    "hashtables, specifying the initial number of bins, "
    "the maximum load proportion, and hash function.";

    m = Py_InitModule3("hashtable", HashTablePy_methods, hashtable__doc__);

    Py_INCREF(&HashTablePyType);
    PyModule_AddObject(m, "HashTable", (PyObject *)&HashTablePyType);
}
