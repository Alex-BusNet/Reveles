#ifndef MESSAGES_H
#define MESSAGES_H

#include <iostream>
#include <QString>

namespace Reveles
{
    static const QString CORE_DBUS_SERVICE_REGISTER_FAIL = "[ RevelesCore ] Failed to register com.reveles.core with D-Bus SessionBus.";
    static const QString GUI_DBUS_SERVICE_REGISTER_FAIL = "[ RevelesGUI ] Failed to register com.reveles.gui with D-Bus SessionBus.";
    static const QString DBUS_OBJECT_REGISTER_FAIL = "Failed to register object %1 with D-Bus SessionBus.";

    static const QString GUI_COMM_CHECK = "Sending Comm check to RevelesCore.";
    static const QString CORE_COMM_RECIEVED = "Comm check recieved from RevelesGUI.";
    static const QString GUI_COMM_RECIEVED = "Comm repsonse recieved from RevelesCore.";

    static const QString XG_FOUND = "Accelerometer/Gyroscope found: %1";
    static const QString MAG_FOUND = "Magnetometer found: %1";

    static const QString XG_ID_STR =  "XG id:  0x%1";
    static const QString MAG_ID_STR = "Mag id: 0x%1";

    static const QString CORE_INIT_COMPLETE = "Initialization complete";
    static const QString SET_TARGET_DEST = "Setting Target Destination to %1, %2";
    static const QString MAP_INTERVAL_CHANGED = "Map Update Interval changed.";

    static const QString MAG_DATA =   "Mag Data:   X: %1 Y: %2 Z: %3 gauss";
    static const QString ACCEL_DATA = "Accel Data: X: %1 Y: %2 Z: %3 m/s/s";
    static const QString GYRO_DATA =  "Gyro Data:  X: %1 Y: %2 Z: %3 rad/s";

    static const QString I2C_MOTOR = "I2C Motor command: %1";
    static const QString I2C_GPS_SEND = "I2C GPS Command: 0x%1";
    static const QString I2C_GPS_RESP = "I2C GPS response: 0x%1";

    static const QString NO_TRANS_MAT = "Transition Matrix not found. Using default.";
    static const QString NO_DECISION_MAT = "Decision Matrix not found.";
    static const QString TRANS_MAT_SAVE_ERR = "Could not open Transition Matrix save file for writing.";
    static const QString TRANS_MAT_SAVE_PROG = "Saving Transition Matrix...";
    static const QString DECISION_MAT_SAVE_ERR = "Could not open Decision Matrix save file for writing.";
    static const QString DECISION_MAT_SAVE_PROG = "Saving Decision Matrix...";

    static const QString HAAR_LOWER_BODY_LOAD_FAIL = "Failed to load lower body Haar Cascade Classifier!";
    static const QString HAAR_UPPER_BODY_LOAD_FAIL = "Failed to load upper body Haar Cascade Classifier!";
    static const QString HAAR_FULL_BODY_LOAD_FAIL = "Failed to load full bodt Haar Cascade Classifier!";
    static const QString OD_STOPPING = "Stopping Object Detector...";
    static const QString FRAME_READ_FAIL = "No frame to read.";

    static const QString REVELES_VERSION = "0.6.0209";
    static const QString REVELES_CORE_INFO = "========================================\n"
                                             "RevelesCore            Version: %1\n"
                                             "                                        \n"
                                             "Written by Alex Portolese               \n"
                                             "Copyright 2018                          \n"
                                             "Developed under GPL-3.0                 \n"
                                             "========================================";
}

#endif // MESSAGES_H
