#ifndef RevelesGui_H
#define RevelesGui_H

#include <vector>
#include <QWidget>
#include <QString>
#include <QScrollArea>
#include <QTimer>
#include <chrono>

#include "locationpushbutton.h"
#include "addlocationdialog.h"
#include "settingsscreen.h"
#include "mapview.h"
#include "../RPi/Common/datatypes.h"
#include "reveles_dbus_interface.h"
#include "reveles_dbus_adaptor.h"

namespace Ui {
class RevelesGui;
}

using namespace std;

class AddLocationDialog;

class RevelesGui : public QWidget
{
    Q_OBJECT

public:
    explicit RevelesGui(com::reveles::RevelesCoreInterface *iface = 0, RevelesDBusAdaptor *rda =0, QWidget *parent = 0);
    ~RevelesGui();

    static RevelesGui *instance();

    void addLocation(QString name, GPSCoord coord);

    void setDBusInterface(com::reveles::RevelesCoreInterface *iface);

    void setDBusAdaptor(RevelesDBusAdaptor *rda);

    GPSCoord getLocation();

signals:
    void sendCommCheck();
    void aboutToQuit();
    void SendDestination(GPSCoord dest);
    void SendMapUpdateInterval(int interval);
    void RequestLocation();
    void NavigationAbort();
    void StartDemoMode();

public slots:
    void commCheck(bool good);
    void updateLocation(GPSCoord gpsc);
    void magStatus(bool good);
    void agStatus(bool good);
    void logMessage(QString msg);
    void magUpdate(MagDirection md);
    void accelUpdate(AccelDirection ad);
    void gyroUpdate(GyroDirection gd);
    void ArduinoStatus(bool good);
    void NucleoStatus(bool good, int idx);
    void PIRStatus(bool stat, bool front);

private slots:

    void setLocation(LocationPushButton *pb);

    void on_addLocationPB_clicked();

    void on_exitBtn_clicked();

    void displayDist(float dist, QString unit);

    void TrigDispToggle();

    void on_settingsScreenPB_clicked();

    void on_mapPB_clicked();

    void commTimeout();

    void on_locationsScreenPB_clicked();
    void draw();

    void on_endNavigationPB_clicked();

    void on_removeLocationPB_clicked();

    void on_editLocationPB_clicked();

    void on_startNavigationPB_clicked();

    void on_demoPB_clicked();

    void tofUpdate(int idx, float val);
    void usUpdate(int idx, float val);
    void servoStat(bool front, uint8_t dir);
    void motorStat(uint8_t dir);

private:
    Ui::RevelesGui *ui;
    QScrollArea *sa;
    QGridLayout *gl;
    QWidget *scrollWidget, *sc;
    MapView *mapView;
    SettingsScreen *ss;
    AddLocationDialog *ald;
    std::chrono::steady_clock::time_point guiUptime, travelUptime;
    QTimer *commTimer, *updateTimer;

    GPSCoord currentLoc;

    com::reveles::RevelesCoreInterface *rci;
    RevelesDBusAdaptor *rdba;
    bool trigOn, hasComms, demoOn;
    int selectedPBIdx;

    void setupLocations();
    void saveLocations();

    QVector<LocationPushButton*> lpbs;

};

#endif // RevelesGui_H
