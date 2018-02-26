#ifndef RevelesGui_H
#define RevelesGui_H

#include <vector>
#include <QWidget>
#include <QString>
#include <QScrollArea>
#include <QTimer>

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

public slots:
    void commCheck(bool good);
    void updateLocation(GPSCoord gpsc);
    void magStatus(bool good);
    void agStatus(bool good);
    void logMessage(QString msg);
    void magUpdate(MagDirection md);
    void accelUpdate(AccelDirection ad);
    void gyroUpdate(GyroDirection gd);

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

private:
    Ui::RevelesGui *ui;
    QScrollArea *sa;
    QGridLayout *gl;
    QWidget *scrollWidget, *sc;
    MapView *mapView;
    SettingsScreen *ss;
    AddLocationDialog *ald;

    QTimer *commTimer, *updateTimer;

    GPSCoord currentLoc;

    com::reveles::RevelesCoreInterface *rci;
    RevelesDBusAdaptor *rdba;
    bool trigOn, hasComms;

    void setupLocations();
    void saveLocations();

    vector<LocationPushButton*> lpbs;

};

#endif // RevelesGui_H
