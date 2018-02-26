#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QWidget>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <../RPi/Common/datatypes.h>
namespace Ui {
class SettingsScreen;
}

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = 0);
    ~SettingsScreen();

    void setLocUpdateStatus(bool stat);
    void setDBusStatus(bool stat);
    void setCoordText(QString coord);
    void setUSDistReading(QString reading);
    void addToLog(QString str);


private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

public slots:
    void setAGStatus(bool stat);
    void setMagStatus(bool stat);
    void MagReading(MagDirection md);
    void AccelReading(AccelDirection ad);
    void GyroReading(GyroDirection gd);

private:
    Ui::SettingsScreen *ui;
};

#endif // SETTINGSSCREEN_H
