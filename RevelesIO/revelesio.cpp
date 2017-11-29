//
// Created by Alex on 11/29/2017.
//

#include "revelesio.h"

PyMODINIT_FUNC PyInit_RevelesIO()
{
    return PyModuleDef_Init(&riomodule);
}

/*
 * RIO_write() is (intended) to
 * handle data coming FROM a python
 * script INTO Reveles.
 */
PyObject *RIO_write(PyObject *self, PyObject *args)
{
    const char *cmd;
    int sts;

    if(!PyArg_ParseTuple(args, "s", &cmd))
        return NULL;

    sts = system(cmd);

    if(sts < 0)
    {
        PyErr_SetString(RIO_Error, "RIO_write() failed");
        return NULL;
    }

    return PyLong_FromLong(sts);
}

void Rio_dealloc(RioObject *self)
{
    Py_XDECREF(self->rio_attr);
    PyObject_Del(self);
}

PyObject *Rio_GetAttr(RioObject *self, PyObject *name)
{
    if(self->rio_attr != NULL)
    {
        PyObject *v = PyDict_GetItem(self->rio_attr, name);
        if(v != NULL)
        {
            Py_INCREF(v);
            return v;
        }
    }

    return PyObject_GenericGetAttr((PyObject*)self, name);
}

int Rio_SetAttr(RioObject *self, const char *name, PyObject *v)
{
    if(self->rio_attr == NULL)
    {
        self->rio_attr = PyDict_New();
        if(self->rio_attr == NULL)
            return -1;
    }

    if(v == NULL)
    {
        int rv = PyDict_DelItemString(self->rio_attr, name);
        if(rv < 0)
        {
            PyErr_SetString(PyExc_AttributeError, "Delete non-existing RevelesIO attribute.");
        }

        return rv;
    }
    else
    {
        return PyDict_SetItemString(self->rio_attr, name, v);
    }


}
