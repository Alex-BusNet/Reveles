#ifndef GPS_H
#define GPS_H

#include <tuple>
#include "../Common/datatypes.h"
#include <time.h>
#include <string>

using namespace std;

class GPS
{
public:
    GPS();
    bool update();
    tuple<int,int> parseSentence();
    void parseGPGGA(string args);
    void parseGPRMC(string args);

private:
    tm timestampUTC;
    GPSCoord currentCoord;
    float fixQuality;
    int satellites;
    float horizontalDilusion;
    float altitude;
    float velocityKnots;
    float trackAngleDeg;
};

#endif // GPS_H
