#include <Python.h>
#include "structmember.h"
#include "hashtablemodule_helpers.h"

void
free_hashable(union Hashable hashable, hash_type type)
{
    if (type == STRING) {
        free(hashable->str);
    }
}

int
set_hashable_from_user_input(union Hashable *to_set, hash_type *type, PyObject* input)
{
    size_t str_size;
    char *temp_str;
    if (PyInt_Check(input)) {
        to_set->i = PyInt_AsLong(input);
    }
    else if (PyFloat_Check(input)) {
        to_set->f = PyFloat_AsDouble(input);
        *type = DOUBLE;
    }
    else if (PyString_Check(input)) {
        str_size = (size_t)PyString_GET_SIZE(input) + 1;
        to_set->str = malloc(str_size);
        temp_str = PyString_AsString(input);
        snprintf(to_set->str, str_size, "%s", temp_str);
        *type = STRING;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Parameter must be integer, float, or string.");
        return -1;
    }
    return 0;
}


PyObject*
format_python_return_val_from_item(Item *item)
{
    PyObject* return_val = NULL;

    if (!item) {
        Py_RETURN_NONE;
    }

    switch(item->value_type) {
        case INTEGER:
            return_val = Py_BuildValue("i", item->value.i);
            break;
        case DOUBLE:
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

long int
get_hash(union Hashable key, hash_type type, PyObject *hash_func)
{
    PyObject *py_key_arg;
    PyObject *py_hash;
    long int hash;

    switch(type) {
        case INTEGER:
            py_key_arg = Py_BuildValue("(i)", key.i);
            break;
        case DOUBLE:
            py_key_arg = Py_BuildValue("(f)", key.f);
            break;
        case STRING:
            py_key_arg = Py_BuildValue("(s)", key.str);
            break;
        default:
            PyErr_SetString(PyExc_RuntimeError, "Invalid key type.");
            return HUGE_VAL;
    }

    py_hash = PyObject_CallObject(hash_func, py_key_arg);
    printf("Here is the tuple: %s\n", PyString_AsString(py_key_arg->ob_type->tp_repr(py_key_arg)));
    Py_DECREF(py_key_arg);

    if ((py_hash == NULL) || (! PyInt_Check(py_hash))) {
        PyErr_SetString(PyExc_ValueError, "Invalid hash function -- all keys must hash to an integer.");
        return HUGE_VAL;
    }

    hash = PyInt_AsLong(py_hash);
    Py_DECREF(py_hash);

    return hash;
}

PyObject *
default_py_hash_funcs(void)
{
    PyObject* built_ins = PyEval_GetBuiltins();

    PyObject* hash_key = Py_BuildValue("s", "hash");
    PyObject* built_in_hash_func = PyDict_GetItem(built_ins, hash_key);

    Py_DECREF(built_ins);
    Py_DECREF(hash_key);

    return built_in_hash_func;
}
