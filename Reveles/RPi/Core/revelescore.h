#ifndef REVELESCORE_H
#define REVELESCORE_H

#include "../Common/datatypes.h"
#include "../Common/gpio.h"

class RevelesCore
{
public:
    RevelesCore();

    int InitPyModule(int argc, char *argv[]);
    int FreePyModule();

private:

    wchar_t *program;

};

#endif // REVELESCORE_H
