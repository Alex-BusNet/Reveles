#ifndef REVELESCAMERA_H
#define REVELESCAMERA_H

#include <QObject>
#include <unistd.h>
#include <cstdio>
#include <raspicam/raspicam_cv.h>
#include <opencv2/core.hpp>

using namespace raspicam;

class RevelesCamera : public QObject
{
    Q_OBJECT
public:
    RevelesCamera();
    ~RevelesCamera();
    static RevelesCamera *instance();

    void Init();
    void setFramerate(int fps);
    void setResolution(int w, int h);
    bool ReadFrame(cv::Mat &img);

private:
    RaspiCam_Cv *rcam;
    int framerate, width, height;
    int fCount;

    int tmpbuff[400*300*4];
};


#endif // REVELESCAMERA_H
