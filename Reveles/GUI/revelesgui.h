#ifndef RevelesGui_H
#define RevelesGui_H

#include <vector>
#include <QWidget>
#include <QString>
#include <QScrollArea>

#include "locationpushbutton.h"
#include "addlocationdialog.h"
#include "settingsscreen.h"
#include "../RPi/Common/datatypes.h"
#include "reveles_dbus_interface.h"

namespace Ui {
class RevelesGui;
}

using namespace std;

class AddLocationDialog;

class RevelesGui : public QWidget
{
    Q_OBJECT

public:
    explicit RevelesGui(com::reveles::RevelesCoreInterface *iface = 0, QWidget *parent = 0);
    ~RevelesGui();

    static RevelesGui *instance();

    void addLocation(QString name, GPSCoord coord);

    void setDBusInterface(com::reveles::RevelesCoreInterface *iface);

public slots:
    void commCheck(bool good);

    void updateLocation(GPSCoord gpsc);

private slots:

    void setLocation(LocationPushButton *pb);

    void on_addLocationPB_clicked();

    void on_exitBtn_clicked();

    void displayDist(float dist, QString unit);

    void TrigDispToggle();

    void on_settingsScreenPB_clicked();

private:
    Ui::RevelesGui *ui;
    QScrollArea *sa;
    QGridLayout *gl;
    QWidget *scrollWidget, *sc;
    SettingsScreen *ss;
    AddLocationDialog *ald;
    com::reveles::RevelesCoreInterface *rci;
    bool trigOn;

    void setupLocations();

    vector<LocationPushButton*> lpbs;

};

#endif // RevelesGui_H
