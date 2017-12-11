#ifndef RevelesGui_H
#define RevelesGui_H

#include <vector>
#include <QWidget>
#include <QString>
#include <QScrollArea>

#include "locationpushbutton.h"
#include "addlocationdialog.h"
#include "datatypes.h"


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

private:
    Ui::RevelesGui *ui;
    QScrollArea *sa;
    QGridLayout *gl;
    QWidget *scrollWidget, *sc;

    void setupLocations();

    vector<LocationPushButton*> lpbs;

};

#endif // RevelesGui_H
