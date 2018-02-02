#include "revelescamera.h"
#include <iostream>

using namespace std;

Q_GLOBAL_STATIC(RevelesCamera, cam)

RevelesCamera::RevelesCamera()
{
    /*
     * Empty C'tor needed for Q_GLOBAL_STATIC macro.
     * Place anything in the Init() function that would
     * normally go in the c'tor.
     */
}

RevelesCamera::~RevelesCamera()
{
    if(rcam != NULL)
    {
        rcam->release();
        delete rcam;
    }
}

RevelesCamera *RevelesCamera::instance()
{
    return cam;
}

void RevelesCamera::Init()
{
    rcam = new RaspiCam_Cv();

    width = 320;
    height = 240;
    framerate = 15;

    rcam->set(CV_CAP_PROP_FORMAT, CV_8UC3);
    rcam->set(CV_CAP_PROP_FPS, framerate);
    rcam->set(CV_CAP_PROP_FRAME_WIDTH, width);
    rcam->set(CV_CAP_PROP_FRAME_HEIGHT, height);

    if(!rcam->open())
    {
        qFatal("[ RevelesCamera ] Could not open Raspberry Pi Camera!");
        return;
    }

    // Wait for camera to stabilize.
    sleep(3);
}

void RevelesCamera::setFramerate(int fps)
{
    this->framerate = fps;
    rcam->set(CV_CAP_PROP_FPS, framerate);
}

void RevelesCamera::setResolution(int w, int h)
{
    this->width = w;
    this->height = h;
    rcam->set(CV_CAP_PROP_FRAME_WIDTH, width);
    rcam->set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

bool RevelesCamera::ReadFrame(cv::Mat &img)
{
    if(rcam->grab())
        rcam->retrieve(img);

    if(!img.empty())
    {
        return true;
    }

    return false;
}
