#ifndef REVELESIO_H
#define REVELESIO_H

#include <Python.h>

typedef struct { PyObject_HEAD PyObject *rio_attr; } RioObject;

PyMODINIT_FUNC PyInit_RevelesIO(void);

static PyObject* RIO_write(PyObject *self, PyObject *args);

static PyObject *RIO_Error;

static void Rio_dealloc(RioObject *self);

static PyObject* Rio_GetAttr(RioObject *self, PyObject *name);

static int Rio_SetAttr(RioObject *self, const char *name, PyObject *v);

/* RevelesIO Method definition table */
static PyMethodDef RioMethods[] =
        {
                { "write", (PyCFunction)RIO_write, METH_VARARGS, "Write data to Reveles Core."},
                { NULL, NULL, 0, NULL}
        };


/* RevelesIO Python Module definition structure */
static struct PyModuleDef riomodule = {
        PyModuleDef_HEAD_INIT,
        "revelesio",
        NULL,
        -1,
        RioMethods,
        NULL,
        NULL,
        NULL,
        NULL
};

#endif // REVELESIO_H