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

    static const QString NUCLEO_FOUND = "Nucleo found at 0x%1: %2";
    static const QString ARDUINO_FOUND = "Arduino found at 0x%1: %2";
    static const QString XG_FOUND = "Accelerometer/Gyroscope found: %1";
    static const QString MAG_FOUND = "Magnetometer found: %1";

    static const QString XG_ID_STR =  "XG id:  0x%1";
    static const QString MAG_ID_STR = "Mag id: 0x%1";

    static const QString FILE_DESCRIPTORS = "fdXG: %1, fdMag: %2, fdArduino: %3, fdNucleo[0]: %4, fdNucleo[1]: %5";

    static const QString CORE_INIT_COMPLETE = "Initialization complete";
    static const QString SET_TARGET_DEST = "Setting Target Destination to %1, %2";
    static const QString MAP_INTERVAL_CHANGED = "Map Update Interval changed.";

    static const QString MAG_DATA =   "  Mag Data: X: %1 Y: %2 Z: %3 gauss";
    static const QString ACCEL_DATA = "Accel Data: X: %1 Y: %2 Z: %3 m/s/s";
    static const QString GYRO_DATA =  " Gyro Data: X: %1 Y: %2 Z: %3 rad/s";
    static const QString MAG_GUI_DATA = "X: %1 gauss\n"
                                        "Y: %2 gauss\n"
                                        "Z: %3 gauss";
    static const QString ACCEL_GUI_DATA = "X: %1 m/s/s\n"
                                          "Y: %2 m/s/s\n"
                                          "Z: %3 m/s/s";
    static const QString GYRO_GUI_DATA = "X: %1 rad/s\n"
                                         "Y: %2 rad/s\n"
                                         "Z: %3 rad/s";
    static const QString GPS_DATA =   "Latitude:  %1 \n"
                                      "Longitude: %2";
    static const QString TOF_READING_STR = "[0]: %1\t[1]: %2\t[2]: %3\t[3]: %4 \n"
                                           "[4]: %5\t[5]: %6\t[6]: %7\t[7]: %8";
    static const QString US_READING_STR = " [0]: %1\t[1]: %2 \n"
                                          " [2]: %3\t[3]: %4";
    // Time format: HH:MM::SS::ms
    static const QString GUI_UPTIME_STR = "GUI Uptime: %1:%2:%3:%4";
    static const QString TRAVEL_UPTIME_STR = "Travel time: %1:%2:%3:%4";

    static const QString TOF_I2C_RESPONSE = "Time of Flight reading from 0x%1: %2";

    static const QString START_EQUAL_DEST = "Current location is the same as destination!";
    static const QString END_OF_PATH = "End of path reached. Returning to idle mode.";
    static const QString COORD_MISMATCH = "Current Node does not match current GPS location.";
    static const QString INVALID_DESTINATION = "Destination is an invalid Coordinate";
    static const QString IOR = "Index out of range.";
    static const QString NAV_ABORT = "Navigation mode stopped by user.";

    static const QString I2C_MOTOR    = "I2C Motor command:  %1";
    static const QString I2C_GPS_SEND = "I2C GPS Command:  0x%1";
    static const QString I2C_GPS_RESP = "I2C GPS response: 0x%1";
    static const QString I2C_SERVO_SEND  = "I2C Servo Command:   0x%1";
    static const QString I2C_SERVO_ANGLE = "I2C Servo Angle:   %1 deg";

    static const QString NO_TRANS_MAT = "Transition Matrix not found. Using default.";
    static const QString NO_DECISION_MAT = "Decision Matrix not found.";
    static const QString TRANS_MAT_SAVE_ERR = "Could not open Transition Matrix save file for writing.";
    static const QString TRANS_MAT_SAVE_PROG = "Saving Transition Matrix...";
    static const QString DECISION_MAT_SAVE_ERR = "Could not open Decision Matrix save file for writing.";
    static const QString DECISION_MAT_SAVE_PROG = "Saving Decision Matrix...";

    static const QString MAP_SAVE_ERR = "Could not open Map Data save file for writing.";
    static const QString MAP_SAVE_PROG = "Saving Map Data...";
    static const QString MAP_LOAD_ERR = "Map Data save file not found. Generating blank map.";
    static const QString MAP_LOAD_PROG = "Loading Map Data...";

    static const QString HAAR_LOWER_BODY_LOAD_FAIL = "Failed to load lower body Haar Cascade Classifier!";
    static const QString HAAR_UPPER_BODY_LOAD_FAIL = "Failed to load upper body Haar Cascade Classifier!";
    static const QString HAAR_FULL_BODY_LOAD_FAIL = "Failed to load full bodt Haar Cascade Classifier!";
    static const QString OD_STOPPING = "Stopping Object Detector...";
    static const QString FRAME_READ_FAIL = "No frame to read.";

    static const QString REVELES_VERSION = "0.11.0412";
    static const QString REVELES_CORE_INFO = "========================================\n"
                                             "RevelesCore            Version: %1\n"
                                             "                                        \n"
                                             "Written by Alex Portolese               \n"
                                             "Copyright 2018                          \n"
                                             "Developed under GPL-3.0                 \n"
                                             "========================================";
}

#endif // MESSAGES_H
