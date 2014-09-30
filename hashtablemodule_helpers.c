#include <Python.h>
#include "structmember.h"
#include "hashtable.h"
#include "hashtablemodule_helpers.h"

void set_hashable_from_user_input(union Hashable *to_set, hash_type *type, PyObject* input) {
    if (PyInt_Check(input)) {
        to_set->i = PyInt_AsLong(input);
    }
    else if (PyFloat_Check(input)) {
        to_set->f = PyFloat_AsDouble(input);
        *type = FLOAT;
    }
    else if (PyString_Check(input)) {
        to_set->str = PyString_AsString(input);
        *type = STRING;
    }
    else {
        PyErr_SetString(PyExc_TypeError, "Parameter must be integer, float, or string.");
    }
}


PyObject* format_python_return_val_from_item(Item *item) {
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
