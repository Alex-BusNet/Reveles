#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <QWidget>

namespace Ui {
class SettingsScreen;
}

class SettingsScreen : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsScreen(QWidget *parent = 0);
    ~SettingsScreen();

    void setAGStatus(bool stat);
    void setMagStatus(bool stat);
    void setLocUpdateStatus(bool stat);
    void setDBusStatus(bool stat);
    void setCoordText(QString coord);
    void setUSDistReading(QString reading);

private slots:
    void on_buttonBox_rejected();

    void on_buttonBox_accepted();

private:
    Ui::SettingsScreen *ui;
};

#endif // SETTINGSSCREEN_H
