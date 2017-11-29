#ifndef REVELESIO_H
#define REVELESIO_H

#include "../../Libraries/pybind11/include/pybind11/pybind11.h"

namespace py = pybind11;

/* This Function is currently used to test PyBind11 */
int add(int i, int j)
{
    return i + j;
}

/* Syntax: PYBIND11_MODULE(<<Module name>>, interface variable) */
PYBIND11_MODULE(example, m)
{
    m.doc() = "pybind11 example plugin";
    m.def("add", &add, "A function which adds two numbers");
}



//#include <Python.h>
//
//PyMODINIT_FUNC PyInit_RevelesIO(void);
//
//static PyObject* RIO_write(PyObject *self, PyObject *args);
//
//static PyObject *RIO_Error;
//
///* RevelesIO Method definition table */
//static PyMethodDef RioMethods[] =
//{
//    { "write", RIO_write, METH_VARARGS, "Write data to Reveles Core."},
//    { NULL, NULL, 0, NULL}
//};
//
//
///* RevelesIO Python Module definition structure */
//static struct PyModuleDef revelesio = {
//    PyModuleDef_HEAD_INIT,
//    "rio",
//    NULL,
//    -1,
//    RioMethods,
//    NULL,
//    NULL,
//    NULL,
//    NULL
//};

#endif // REVELESIO_H
