#ifndef DATATYPES_H
#define DATATYPES_H
#include <QMetaType>
#include <QtDBus/QDBusArgument>
#include <cstring>
#include <QMap>
#include <tuple>

#define TUPI(a,b) (std::tuple<int, int>(a, b))
#define TUPD(a,b) (std::tuple<double, double>(a,b))
#define TUPF(a,b) (std::tuple<float, float>(a,b))

#define BIN  2
#define HEX 16
#define OCT  8
#define DEC 10

#define MOTOR_MAX_SPEED  156
#define MOTOR_HALF_SPEED  78

#define THRESHOLD_LATITUDE  (0.0000103651 / 2)
#define THRESHOLD_LONGITUDE (0.0000038167 / 2)

// Passive Infrared Select bits
#define PIR_FL 0b000 // Front-left
#define PIR_FR 0b001 // Front-right
#define PIR_BL 0b010 // Back-left
#define PIR_BR 0b011 // Back-right

// Ultrasonic Select bits
#define US_FRONT        0b000
#define US_FRONT_STAIR  0b001
#define US_BACK         0b010
#define US_BACK_STAIR   0b011
//#define US_BACK  0b100

#define I2C_TRANSMIT_DELAY 100

#define B2STR( x ) (x ? "True" : "False")

struct GPSCoord {
    double latitude;
    double longitude;

    bool operator==(GPSCoord& other)
    {
        return ((this->latitude == other.latitude)
               && (this->longitude == other.longitude));
    }
    bool operator==(std::tuple<double, double> td)
    {
        return((this->latitude == std::get<0>(td)) && (this->longitude == std::get<1>(td)));
    }

    bool operator!=(GPSCoord& other)
    {
        return ((this->latitude != other.latitude) || (this->longitude != other.longitude));
    }

    bool operator!=(std::tuple<double, double> &td)
    {
        return((this->latitude != std::get<0>(td)) || (this->longitude != std::get<1>(td)));
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

struct AccelDirection { double x, y, z; };

struct MagDirection { double x, y, z; };

struct GyroDirection { double x, y, z; };

Q_DECLARE_METATYPE(GPSCoord)
Q_DECLARE_METATYPE(AccelDirection)
Q_DECLARE_METATYPE(MagDirection)
Q_DECLARE_METATYPE(GyroDirection)

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


inline QDBusArgument &operator<<(QDBusArgument &argument, const AccelDirection& ad)
{
    argument.beginStructure();
    argument << ad.x;
    argument << ad.y;
    argument << ad.z;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, AccelDirection &ad)
{
    argument.beginStructure();
    argument >> ad.x;
    argument >> ad.y;
    argument >> ad.z;
    argument.endStructure();
    return argument;
}

inline QDBusArgument &operator<<(QDBusArgument &argument, const MagDirection& md)
{
    argument.beginStructure();
    argument << md.x;
    argument << md.y;
    argument << md.z;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, MagDirection &md)
{
    argument.beginStructure();
    argument >> md.x;
    argument >> md.y;
    argument >> md.z;
    argument.endStructure();
    return argument;
}

inline QDBusArgument &operator<<(QDBusArgument &argument, const GyroDirection& gd)
{
    argument.beginStructure();
    argument << gd.x;
    argument << gd.y;
    argument << gd.z;
    argument.endStructure();
    return argument;
}

inline const QDBusArgument &operator>>(const QDBusArgument &argument, GyroDirection &gd)
{
    argument.beginStructure();
    argument >> gd.x;
    argument >> gd.y;
    argument >> gd.z;
    argument.endStructure();
    return argument;
}

QT_END_NAMESPACE
//====================================================

#define FA_SW_CORNER GPSCoord{41.631906, -85.006082}
#define FA_SE_CORNER GPSCoord{41.631910, -85.005670}
#define FA_NE_CORNER GPSCoord{41.632562, -85.005680}
#define FA_NW_CORNER GPSCoord{41.632559, -85.005982}
#define FA_MAP_NW 	 GPSCoord{41.632559, -85.006123}
#define FA_MAP_SW 	 GPSCoord{41.631906, -85.006123}
#define FA_MAP_SE 	 GPSCoord{41.631906, -85.005665}

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

class MapNode
{
public:
    MapNode()
    {
        nt = UNKNOWN;
        mapX = -1;
        mapY = -1;
    }

    GPSCoord coord;
    NodeType nt;
    MapNode* parent = NULL;
    MapNode *child = NULL;
    int mapX, mapY;
    int gCost = 0, hCost = 0;
    int fCost() { return gCost + hCost; }

    bool operator==(MapNode &other)
    {
        return this->coord == other.coord;
    }

    bool operator!=(MapNode &other)
    {
        return this->coord != other.coord;
    }
};

// This struct may be removed.
struct Path {
    MapNode *child;
    MapNode *parent;
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
    CMD_T       = 0x40,
    CMD_P       = 0x50,
    M_REV       = 0x12,
    M_STOP      = 0x13,
    LATITUDE    = 0x21,
    LONGITUDE   = 0x22,
	TURN_LEFT   = 0x31,
	TURN_RIGHT  = 0x32,
	RET_NEUTRAL = 0x33,
    CMD_FLUSH   = 0x44
};

enum IOSelectBits
{
    FRONT_US_PIR = 0x00,
    REAR_US_PIR  = 0x01,
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

enum IOCommand {IO_MOTOR, IO_SERVO, IO_GPS, IO_TOF};

struct RIOData{ IOCommand cmd; uint8_t data; int specData; };

#endif // DATATYPES_H
