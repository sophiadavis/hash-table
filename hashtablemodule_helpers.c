#include <Python.h>
#include "structmember.h"
#include "hashtablemodule_helpers.h"

int
set_hashable_from_user_input(union Hashable *to_set, hash_type *type, PyObject* input) {
    size_t str_size;
    char *temp_str;
    if (PyInt_Check(input)) {
        to_set->i = PyInt_AsLong(input);
    }
    else if (PyFloat_Check(input)) {
        to_set->f = PyFloat_AsDouble(input);
        *type = FLOAT;
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
format_python_return_val_from_item(Item *item) {
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
