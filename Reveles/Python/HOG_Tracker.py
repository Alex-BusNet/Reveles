#!/usr/bin/env python
# -*- coding: utf-8 -*-
import cv2
from imutils.object_detection import non_max_suppression
from imutils import paths
import numpy as np
import argparse
import imutils


def DetectAndDraw(hog, img):
    found = []
    found_filtered = []

    t = cv2.getTickCount()
    hog.

hog = cv2.HOGDescriptor()
hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())

vc = cv2.VideoCapture(0)

while True:
    (grabbed, frame) = vc.read()
    cv2.imshow("Frame", frame)

    key = cv2.waitKey(1)
    if key == ord('q'):
        break

vc.release()
cv2.destroyAllWindows()

