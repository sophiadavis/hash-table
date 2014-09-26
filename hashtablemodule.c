#include <Python.h>
#include "structmember.h"
#include "hashtable.h"

typedef struct {
    PyObject_HEAD
    HashTable *hashtable;
} HashTablePyObject;

static int
HashTablePyObject_init(HashTablePyObject *self, PyObject *args, PyObject *kwds)
{
    // int *value = NULL;
    // PyObject *next = NULL;
    // PyObject *tmp;
    //
    // static char *kwlist[] = {"value", "next", NULL};
    //
    // if (! PyArg_ParseTupleAndKeywords(args, kwds, "|OO", kwlist,
    //                                   &value, &next))
    //     return -1;
    //
    // if (value) {
    //     self->value = *value;
    // }
    //
    // if (next) {
    //     tmp = self->next;
    //     Py_INCREF(next);
    //     self->next = next;
    //     Py_XDECREF(tmp);
    // }
    self->hashtable = init(4, 0.5);

    return 0;
}

static HashTablePyObject *
HashTablePy_set(HashTablePyObject *self, PyObject *args)
{
    int hash;
    int key;
    int value;

    printf("about to print table\n");
    print_table(self->hashtable);
    printf("printed table\n");

    if (!PyArg_ParseTuple(args, "iii", &hash, &key, &value))
        return NULL;

    union Hashable k;
    k.i = key;

    union Hashable v;
    v.i = value;

    self->hashtable = add_item_to_table(hash, k, 0, v, 0, self->hashtable);

    printf("about to print table\n");
    print_table(self->hashtable);
    printf("printed table\n");

    return self;
}

static PyMethodDef HashTablePy_methods[] = {
    {"set", (PyCFunction)HashTablePy_set, METH_VARARGS,
     "Add a key-value pair to the hashtable."
    },
    {NULL}  /* Sentinel */
};

static PyTypeObject HashTablePyType = {
    PyObject_HEAD_INIT(NULL)
    0,                         /*ob_size*/
    "hashtable.HashTable",             /*tp_name*/
    sizeof(HashTablePyObject), /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    0,                         /*tp_dealloc*/
    0,                         /*tp_print*/
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
    "HashTable objects",       /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    HashTablePy_methods,    /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    (initproc)HashTablePyObject_init,      /* tp_init */
};

// static PyMethodDef module_methods[] = {
//     {NULL}  /* Sentinel */
// };

//Dealloc???

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
