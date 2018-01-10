#ifndef RevelesGui_H
#define RevelesGui_H

#include <vector>
#include <QWidget>
#include <QString>
#include <QScrollArea>
#include <QtDBus/QDBusConnection>

#include "locationpushbutton.h"
#include "addlocationdialog.h"
#include "../../RPi/Common/datatypes.h"
#include "RPi/Core/revelescore.h"
#include "keyboard.h"

namespace Ui {
class RevelesGui;
}

using namespace std;

class RevelesGui : public QWidget
{
    Q_OBJECT

public:
    explicit RevelesGui(QWidget *parent = 0);
    ~RevelesGui();

    static RevelesGui *instance();

    void addLocation(QString name, GPSCoord coord);

private slots:

    void setLocation(LocationPushButton *pb);

    void on_addLocationPB_clicked();

    void on_exitBtn_clicked();

    void displayDist(float dist, QString unit);

    void TrigDispToggle();

private:
    Ui::RevelesGui *ui;
    QScrollArea *sa;
    QGridLayout *gl;
    QWidget *scrollWidget, *sc;
    RevelesCore *rc;
    bool trigOn;
    void setupLocations();

    vector<LocationPushButton*> lpbs;

};

#endif // RevelesGui_H
