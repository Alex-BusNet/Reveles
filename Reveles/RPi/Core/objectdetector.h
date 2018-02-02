#ifndef OBJECTDETECTOR_H
#define OBJECTDETECTOR_H

#include <QObject>
#include <iostream>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <vector>

#include "Common/datatypes.h"
#include "Common/detectionqueue.h"
#include <IO/revelescamera.h>

#include <opencv2/core.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

struct ObjContainer { Point screenLoc; bool updated; int zone; ActionState dir; };

class ObjectDetector : public QObject
{
    Q_OBJECT

public:
    ObjectDetector();
    static ObjectDetector *instance();

    void Init();
    void TeachHOG();
    void Run();
    FutureStatus GetFutureStatus();

public slots:
    void aboutToQuit();

private:
//    Point objects[16];
//    bool frameUpdated[16];
//    ActionState objectDirection[16];
    QFuture<void> future;
    ObjContainer objects[16];
    bool exiting;

    CascadeClassifier upperBody;
    CascadeClassifier lowerBody;
    CascadeClassifier fullBody;


    void PeopleDetect();
    void DetectAndDraw(const HOGDescriptor &hog, Mat &img);
    void FaceFinder(Mat &img);
};

#endif // OBJECTDETECTOR_H
