#include "gps.h"

GPS::GPS()
{

}

bool GPS::update()
{
    return false;
}

tuple<int, int> GPS::parseSentence()
{
    return make_tuple(0, 0);
}

void GPS::parseGPGGA(string args)
{

}

void GPS::parseGPRMC(string args)
{

}
