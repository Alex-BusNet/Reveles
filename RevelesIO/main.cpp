#import <Python.h>
#include <iostream>
#include "revelesio.h"

using namespace std;

int main(int argc, char *argv[])
{
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL)
    {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }

    PyImport_AppendInittab("revelesio", PyInit_RevelesIO);
    Py_SetProgramName(program);
    Py_Initialize();
    PyImport_ImportModule("revelesio");

    // Some Code Here

    PyMem_RawFree(program);
    return 0;
}