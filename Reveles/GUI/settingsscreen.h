#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QWidget>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <../RPi/Common/datatypes.h>
#include <QString>
#include <chrono>

namespace Ui {
class SettingsScreen;
}

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = 0);
    ~SettingsScreen();

    void addToLog(QString str);

    void setLocUpdateStatus(bool stat);
    void setDBusStatus(bool stat);
    void setCoordText(QString coord);

    void setUSDistReading(int idx, double value);
    void setToFReading(int idx, double value);

    void setMotorStatus(uint8_t dir);
    void setServoStatus(bool front, uint8_t dir);

    void UpdateGUIUptime(int guiMs);
    void UpdateTravelUptime(int travelMs);

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

    void on_settingsPB_clicked();

    void on_logPB_clicked();

    void on_statusPB_clicked();

    void on_aboutPB_clicked();

public slots:
    void setAGStatus(bool stat);
    void setMagStatus(bool stat);
    void MagReading(MagDirection md);
    void AccelReading(AccelDirection ad);
    void GyroReading(GyroDirection gd);

    void setArduino(bool good);
    void setNucleo(bool good, int idx);
    void setPIR(bool stat, bool front);

private:
    double tofHistory[8] = {-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f};
    double usHistory[4] = {171.0f, 171.0f, 171.0f, 171.0f};

    Ui::SettingsScreen *ui;
};

#endif // SETTINGSSCREEN_H
