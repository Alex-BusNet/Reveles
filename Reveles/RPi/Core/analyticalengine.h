#ifndef REVELES_ANALYTICALENGINE_H
#define REVELES_ANALYTICALENGINE_H
#include <QObject>
#include <QList>
#include "Common/datatypes.h"
#include "revelescore.h"

/*
 * The AnalyticalEngine class is intended to process
 * the environment and make adjustments to Reveles' path
 * as needed. This class falls under the NEPU subsystem
 * discussed in the documentation.
 */
class AnalyticalEngine : public QObject
{
    Q_OBJECT

public:
    static AnalyticalEngine *instance();
    AnalyticalEngine();

    void Init();

private:
    void CheckEnv();
    void ProcessEnv();

    void AdjustPath_Inanimate();
    void AdjustPath_Animate();

    // Variables
};


#endif //REVELES_ANALYTICALENGINE_H
