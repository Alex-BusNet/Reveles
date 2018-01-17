#include "analyticalengine.h"
#include <QtConcurrent/QtConcurrent>

Q_GLOBAL_STATIC(AnalyticalEngine, rae)

AnalyticalEngine *AnalyticalEngine::instance()
{
    return rae;
}

/*
 * Initialize variables for the analytical engine
 */
void AnalyticalEngine::Init()
{

}

AnalyticalEngine::AnalyticalEngine()
{
    /*
     * Empty C'tor needed for Q_GLOBAL_STATIC macro.
     * Place anything in the Init() function that would
     * normally go in the c'tor.
     */
}

void AnalyticalEngine::CheckEnv()
{

}

void AnalyticalEngine::ProcessEnv()
{

}

void AnalyticalEngine::AdjustPath_Inanimate()
{

}

void AnalyticalEngine::AdjustPath_Animate()
{

}
