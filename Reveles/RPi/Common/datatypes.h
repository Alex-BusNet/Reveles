#ifndef DATATYPES_H
#define DATATYPES_H
#include <QMetaType>
#include <QtDBus/QDBusArgument>
#include <cstring>
#include <QMap>

struct GPSCoord {
    double latitude;
    double longitude;

    bool operator==(GPSCoord& other)
    {
        return ((this->latitude == other.latitude)
               && (this->longitude == other.longitude));
    }

    GPSCoord operator+(std::tuple<double, double> &td)
    {
        return GPSCoord{this->latitude + std::get<0>(td), this->longitude + std::get<1>(td)};
    }

    GPSCoord operator-(std::tuple<double, double> &td)
    {
        return GPSCoord{this->latitude - std::get<0>(td), this->longitude - std::get<1>(td)};
    }
};

Q_DECLARE_METATYPE(GPSCoord)

//====================================================
// D-Bus Custom-type marshall and demarshall operator overloads.
QT_BEGIN_NAMESPACE

inline QDBusArgument &operator<<(QDBusArgument &argument, const GPSCoord& coord)
{
    argument.beginStructure();
    argument << coord.latitude;
    argument << coord.longitude;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, GPSCoord &coord)
{
    argument.beginStructure();
    argument >> coord.latitude;
    argument >> coord.longitude;
    argument.endStructure();
    return argument;
}

QT_END_NAMESPACE
//====================================================

struct AccelDirection { float x, y, z; };

struct MagDirection { float x, y, z; };

struct GyroDirection { float x, y, z; };

struct DecisionPoint {
    MagDirection face;
    GPSCoord point;
};

// The DOOR type may not be used.
enum NodeType
{
    WALL, STAIRS, DOOR, PATH, UNKNOWN,
//    R_WALL, T_WALL, B_WALL,
//    V_PATH, H_PATH,
//    L_T, R_T, B_T, T_T,
//    L_DEAD_END, R_DEAD_END, B_DEAD_END, T_DEAD_END
};

struct Node {
    GPSCoord coord;
    NodeType nt;
};

// This struct may be removed.
struct Path {
    GPSCoord coord;
    Node *child;
    Node *parent;
};

enum Direction {
    N, NE, NW,
    S, SE, SW,
    W, WN, WS,
    E, EN, ES
};

// Mounting location of HC-SR04 on Reveles.
enum UltrasonicPos {
    FORWARD, REAR, LEFT, RIGHT, STAIR
};

// Used in RevelesAnalyticalEngine::Navigate()
enum TravelMode { BLIND, WANDER };

enum SensorType {US, PIR, TOF};

// ActionState represents the detected
// and predicted actions an moving object
// would take.
enum ActionState
{
    L2R      = 0,// State 1
    R2L      = 1,// State 2
    TOWARDS  = 2,// State 3
    AWAY     = 3,// State 4
    DIAG_L2R = 4,// State 5
    DIAG_R2L = 5,// State 6
    STOP     = 6,// State 7
    NO_STATE = 7 // Initialization State.
};

static std::string DirString[] =
{
    "North", "North-East", "North-West",
    "South", "South-East", "South-West",
    "West", "West-North", "West-South",
    "East", "East-North", "East-South"
};

struct ObjectTracking
{
    int index;
    ActionState dir;
    int zone;
};

enum FutureStatus {STOPPED, RUNNING, PAUSED, FINISHED, NON_EXISTENT };

typedef enum LoggerFlag
{
    NO_LOG_FLAGS    = 0b0000,
    BOOL_AS_ALPHA   = 0b0001,
    SHOW_POINT      = 0b0010,
    FIXED_LENGTH    = 0b0100,
    SHOW_POS_NEG    = 0b1000
} LoggerFlag;

Q_DECLARE_FLAGS(LoggerFlags, LoggerFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS( LoggerFlags )

enum I2C_COMMANDS
{
    COM_CHECK   = 0xAA,
    START       = 0x00,
    END         = 0xA3,
    M_FWD       = 0x11,
    CMD_G       = 0x20,
    CMD_M       = 0x10,
	CMD_S       = 0x30,
    M_REV       = 0x12,
    M_STOP      = 0x13,
    LATITUDE    = 0x21,
    LONGITUDE   = 0x22,
	TURN_LEFT   = 0x31,
	TURN_RIGHT  = 0x32,
	RET_NEUTRAL = 0x33,
    CMD_FLUSH   = 0x4C
};

static QMap<char, qulonglong> asciiMap =
{
    {'.', 0x2E}, //46
    {'0', 0x30}, //48
    {'1', 0x31}, //49
    {'2', 0x32}, //50
    {'3', 0x33}, //51
    {'4', 0x34}, //52
    {'5', 0x35}, //53
    {'6', 0x36}, //54
    {'7', 0x37}, //55
    {'8', 0x38}, //56
    {'9', 0x39}, //57
    {':', 0x3A}, //58
    {'F', 0x46}, //70
    {'G', 0x47}, //71
    {'M', 0x4D}, //77
    {'R', 0x52}, //82
    {'S', 0x53}  //83
};

#endif // DATATYPES_H
