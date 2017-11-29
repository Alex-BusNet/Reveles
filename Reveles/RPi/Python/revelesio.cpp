#include "revelesio.h"

//PyObject *PyInit_RevelesIO()
//{
//    PyObject *m = PyModule_Create(&revelesio);
//    if(m == NULL)
//        return NULL;
//
//    RIO_Error = PyErr_NewException("rio.error", NULL, NULL);
//    Py_INCREF(RIO_Error);
//    PyModule_AddObject(m, "error", RIO_Error);
//    return m;
//}
//
///*
// * RIO_write() is (intended) to
// * handle data coming FROM a python
// * script INTO Reveles.
// */
//PyObject *RIO_write(PyObject *self, PyObject *args)
//{
//    const char *cmd;
//    int sts;
//
//    if(!PyArg_ParseTuple(args, "s", &cmd))
//        return NULL;
//
//    sts = system(cmd);
//
//    if(sts < 0)
//    {
//        PyErr_SetString(RIO_Error, "RIO_write() failed");
//        return NULL;
//    }
//
//    return PyLong_FromLong(sts);
//}


