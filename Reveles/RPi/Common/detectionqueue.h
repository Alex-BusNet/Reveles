#ifndef DETECTIONQUEUE_H
#define DETECTIONQUEUE_H

#include "datatypes.h"
#include <QQueue>]
#include <opencv2/core.hpp>

static QQueue<ObjectTracking> movementUpdate;

class DetectionQueue
{
public:
    DetectionQueue();
    static void enqueue(ObjectTracking ot) { movementUpdate.enqueue(ot); }
    static ObjectTracking dequeue() { return movementUpdate.dequeue(); }
    static bool isEmpty() { return movementUpdate.isEmpty(); }
};

#endif // DETECTIONQUEUE_H
