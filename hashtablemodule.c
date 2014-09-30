#include <Python.h>
#include "structmember.h"
#include "hashtable.h"

typedef struct {
    PyObject_HEAD
    HashTable *hashtable;
    int size;
    float max_load;
} HashTablePyObject;

static int
HashTablePyObject_init(HashTablePyObject *self, PyObject *args, PyObject *kwds)
{
    int size = 4;
    float max_load = 0.5;

    static char *kwlist[] = {"size", "max_load", NULL};

    if (! PyArg_ParseTupleAndKeywords(args, kwds, "|if", kwlist, &size, &max_load)) {
        printf("Invalid size and max load proportion. Using defaults.\n");
    }

    printf("Size and max load proportion: %i, %f\n", size, max_load);

    self->hashtable = init(size, max_load);
    self->size = size;
    self->max_load = max_load;

    return 0;
}

static PyMemberDef Hashtable_members[] = {
    {"size", T_INT, offsetof(HashTablePyObject, size), 0,
     "Current number of bins in hashtable."},
    {"max_load", T_FLOAT, offsetof(HashTablePyObject, max_load), 0,
     "Maximum proportion of number of items to number of bins before resizing."},
    {NULL}  /* Sentinel */
};

static void
HashTablePyObject_dealloc(HashTablePyObject* self)
{
    printf("~~~~~~~~~~~~Dealloc-ing~~~~~~~~~~~~\n");
    self->ob_type->tp_free((PyObject*)self);
}

static void
HashTablePyObject_free(HashTablePyObject* self)
{
    printf("~~~~~~~~~~~~Free-ing~~~~~~~~~~~~\n");
    free_table(self->hashtable);
}

static HashTablePyObject *
HashTablePy_set(HashTablePyObject *self, PyObject *args)
{
    printf("setting\n");
    Py_INCREF(self);

    union Hashable key;
    hash_type key_type = INTEGER; // default
    union Hashable value;
    hash_type value_type = INTEGER;

    PyObject* key_input = NULL;
    PyObject* value_input = NULL;

    if (!PyArg_ParseTuple(args, "OO", &key_input, &value_input))
        return NULL;

    if (PyInt_Check(key_input)) {
        key.i = PyInt_AsLong(key_input);
    }
    else if (PyFloat_Check(key_input)) {
        key.f = PyFloat_AsDouble(key_input);
        key_type = FLOAT;
    }
    else if (PyString_Check(key_input)) {
        key.str = PyString_AsString(key_input);
        key_type = STRING;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Key must be integer, float, or string.");
    }

    if (PyInt_Check(value_input)) {
        value.i = PyInt_AsLong(value_input);
    }
    else if (PyFloat_Check(value_input)) {
        value.f = PyFloat_AsDouble(value_input);
        value_type = FLOAT;
    }
    else if (PyString_Check(value_input)) {
        value.str = PyString_AsString(value_input);
        value_type = STRING;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Value must be integer, float, or string.");
    }

    self->hashtable = add(HUGE_VAL, key, key_type, value, value_type, self->hashtable);
    return self;
}

static PyObject *
HashTablePy_get(HashTablePyObject *self, PyObject *args)
{
    printf("getting\n");
    union Hashable key;
    hash_type key_type = INTEGER; // default

    PyObject* key_input = NULL;

    if (!PyArg_ParseTuple(args, "O", &key_input))
        return NULL;

    if (PyInt_Check(key_input)) {
        key.i = PyInt_AsLong(key_input);
    }
    else if (PyFloat_Check(key_input)) {
        key.f = PyFloat_AsDouble(key_input);
        key_type = FLOAT;
    }
    else if (PyString_Check(key_input)) {
        key.str = PyString_AsString(key_input);
        key_type = STRING;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Key must be integer, float, or string.");
    }

    Item *item = lookup(key, key_type, self->hashtable);

    PyObject* return_val = NULL;

    if (!item) {
        Py_RETURN_NONE;
    }

    switch(item->value_type) {
        case INTEGER:
            return_val = Py_BuildValue("i", item->value.i);
            break;
        case FLOAT:
            return_val = Py_BuildValue("f", item->value.f);
            break;
        case STRING:
            return_val = Py_BuildValue("s", item->value.str);
            break;
        default:
            Py_RETURN_NONE;
    }

    return return_val;
}

static int
HashTablePy_print(HashTablePyObject *self, PyObject *args) {
    print_table(self->hashtable);
    return 0;
}

static PyMethodDef HashTablePy_methods[] = {
    {"set", (PyCFunction)HashTablePy_set, METH_VARARGS,
     "Add a key-value pair to the hashtable."
    },
    {"get", (PyCFunction)HashTablePy_get, METH_VARARGS,
     "Lookup value associated with given key in the hashtable."
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject HashTablePyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "hashtable.HashTable",             /*tp_name*/
    sizeof(HashTablePyObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)HashTablePyObject_dealloc, /*tp_dealloc*/
    (printfunc)HashTablePy_print,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT,        /*tp_flags*/
    "Customizeable HashTable!",       /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    HashTablePy_methods,    /* tp_methods */
    Hashtable_members,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)HashTablePyObject_init,      /* tp_init */
    0,                          /* tp_alloc */
    0,                          /* tp_new */
    (freefunc)HashTablePyObject_free, /* tp_free */
};

// static PyMethodDef module_methods[] = {
//     {NULL}  /* Sentinel */
// };


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

    m = Py_InitModule3("hashtable", HashTablePy_methods,
                       "Customizable hashtables.");

    Py_INCREF(&HashTablePyType);
    PyModule_AddObject(m, "HashTable", (PyObject *)&HashTablePyType);
}
