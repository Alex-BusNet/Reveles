#include "settingsscreen.h"
#include "ui_settingsscreen.h"
#include "../RPi/Common/messages.h"

SettingsScreen::SettingsScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScreen)
{
    ui->setupUi(this);
    ui->infoLabel->setText(ui->infoLabel->text()
                           + QString("Version: %1").arg(Reveles::REVELES_VERSION));

    ui->usDistLabel->setText(QChar(0x221E));
    ui->tabWidget->setCurrentIndex(1);

    tofHistory[0] = tofHistory[1] = tofHistory[2] = tofHistory[3] = tofHistory[4] = tofHistory[5] = tofHistory[6] = tofHistory[7] = -1.0f;
    usHistory[0] = usHistory[1] = usHistory[2] = usHistory[3] = -1.0f;

    ui->coordinateLabel->setText(Reveles::TOF_READING_STR.arg(tofHistory[0], 4, 'f', 2, QChar('0')).arg(tofHistory[1], 4, 'f', 2, QChar('0')).arg(tofHistory[2], 4, 'f', 2, QChar('0')).arg(tofHistory[3], 4, 'f', 2, QChar('0'))
            .arg(tofHistory[4], 4, 'f', 2, QChar('0')).arg(tofHistory[5], 4, 'f', 2, QChar('0')).arg(tofHistory[6], 4, 'f', 2, QChar('0')).arg(tofHistory[7], 4, 'f', 2, QChar('0')));

    ui->usDistLabel->setText(Reveles::US_READING_STR.arg(usHistory[0], 4, 'f', 1, QChar('0')).arg(usHistory[1], 4, 'f', 1, QChar('0')).arg(usHistory[2], 4, 'f', 1, QChar('0')).arg(usHistory[3], 4, 'f', 1, QChar('0')));

    ui->guiUptimeLabel->setText(Reveles::GUI_UPTIME_STR.arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')));
    ui->travelUptimeLabel->setText(Reveles::TRAVEL_UPTIME_STR.arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')).arg(0, 2, DEC, QChar('0')));
    QNetworkInterface wlan0 = QNetworkInterface::interfaceFromName("wlan0");
    QNetworkInterface eth0 = QNetworkInterface::interfaceFromName("eth0");
    QString wirelessIP, eth0IP;

    QList<QNetworkAddressEntry> entries = wlan0.addressEntries();
    if(!entries.isEmpty())
        wirelessIP = entries.first().ip().toString();

    entries = eth0.addressEntries();
    if(!entries.empty())
        eth0IP = entries.first().ip().toString();

    ui->ipLabel->setText(QString("wlan0: %1\neth0: %2").arg(wirelessIP).arg(eth0IP));

    QString settingsStyle = "QWidget { background-color: #787878; }";
    settingsStyle += "QLabel { background-color: transparent; }";
    settingsStyle += "QPushButton:enabled { background-color: #0d0d0d; border-radius: 4px; border: 2px solid #5ac5cc; color: white; }";
    settingsStyle += "QScrollBar:vertical { width: 30px; } QScrollBar::up-arrow:vertical, QScrollBar::down-arrow:vertical { width: 50px; height: 50px; border: 2px solid #5ac5cc; }";

    this->setStyleSheet(settingsStyle);
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
}

void SettingsScreen::setAGStatus(bool stat)
{
    if(stat)
    {
        ui->accelStatusLabel->setStyleSheet("QLabel { background-color: green; }");
        ui->gyroStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
    else
    {
        ui->accelStatusLabel->setStyleSheet("QLabel { background-color: red; }");
        ui->gyroStatusLabel->setStyleSheet("QLabel { background-color: red; }");
    }
}

void SettingsScreen::setMagStatus(bool stat)
{
    if(stat)
        ui->magStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->magStatusLabel->setStyleSheet("QLabel { background-color: red; }");
}

void SettingsScreen::setLocUpdateStatus(bool stat)
{
    if(stat)
        ui->locationUpdateLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->locationUpdateLabel->setStyleSheet("QLabel { background-color: transparent; }");
}

void SettingsScreen::setDBusStatus(bool stat)
{
    if(stat)
        ui->dbusStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->dbusStatusLabel->setStyleSheet("QLabel { background-color: red; }");
}

void SettingsScreen::setCoordText(QString coord)
{
//    ui->coordinateLabel->setText(coord);
}

void SettingsScreen::setUSDistReading(int idx, double value)
{
    usHistory[idx] = value;
    ui->usDistLabel->setText( Reveles::US_READING_STR.arg(usHistory[0], 5, 'f', 2, QChar('0')).arg(usHistory[1], 5, 'f', 2, QChar('0')).arg(usHistory[2], 5, 'f', 2, QChar('0')).arg(usHistory[3], 5, 'f', 2, QChar('0')));
}

void SettingsScreen::setToFReading(int idx, double value)
{
    tofHistory[idx] = value;
    ui->coordinateLabel->setText(Reveles::TOF_READING_STR.arg(tofHistory[0], 5, 'f', 2, QChar('0')).arg(tofHistory[1], 5, 'f', 2, QChar('0')).arg(tofHistory[2], 5, 'f', 2, QChar('0')).arg(tofHistory[3], 5, 'f', 2, QChar('0'))
            .arg(tofHistory[4], 5, 'f', 2, QChar('0')).arg(tofHistory[5], 5, 'f', 2, QChar('0')).arg(tofHistory[6], 5, 'f', 2, QChar('0')).arg(tofHistory[7], 5, 'f', 2, QChar('0')));
}

void SettingsScreen::setMotorStatus(uint8_t dir)
{
    ui->motorFwdStatus->setStyleSheet("QLabel { background-color: transparent; }");
    ui->motorRevStatus->setStyleSheet("QLabel { background-color: transparent; }");
    ui->motorStopStatus->setStyleSheet("QLabel { background-color: transparent; }");

    if(dir == M_FWD)
    {
        ui->motorFwdStatus->setStyleSheet("QLabel { background-color: green; }");
    }
    else if(dir == M_STOP)
    {
        ui->motorStopStatus->setStyleSheet("QLabel { background-color: green; }");
    }
    else if(dir == M_REV)
    {
        ui->motorRevStatus->setStyleSheet("QLabel { background-color: green; }");
    }
}

void SettingsScreen::setServoStatus(bool front, uint8_t dir)
{
    if(front)
    {
        ui->fServoLeftStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");
        ui->fServoRightStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");
        ui->fServoNStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");

        if(dir == TURN_LEFT)
            ui->fServoLeftStatusLabel->setStyleSheet("QLabel { background-color: green; } ");
        else if(dir == TURN_RIGHT)
            ui->fServoRightStatusLabel->setStyleSheet("QLabel { background-color: green; }");
        else if(dir == RET_NEUTRAL)
            ui->fServoNStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
    else
    {
        ui->rServoLeftStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");
        ui->rServoRightStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");
        ui->rServoNStatusLabel->setStyleSheet("QLabel { background-color: transparent; }");

        if(dir == TURN_LEFT)
            ui->rServoLeftStatusLabel->setStyleSheet("QLabel { background-color: green; } ");
        else if(dir == TURN_RIGHT)
            ui->rServoRightStatusLabel->setStyleSheet("QLabel { background-color: green; }");
        else if(dir == RET_NEUTRAL)
            ui->rServoNStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
}

void SettingsScreen::UpdateGUIUptime(int guiMs)
{
    float mT = guiMs / 60;
    float hT = mT / 60;

    float deltaH = hT - std::floor(hT);
    mT = deltaH * 60;

    float deltaM = mT - std::floor(mT);

    int s = deltaM * 60;
    int h = hT;
    int m = mT;


//    ui->guiUptimeLabel->setText(Reveles::TRAVEL_UPTIME_STR.arg(h, 2, DEC, QChar('0')).arg(m, 2, DEC, QChar('0')).arg(s, 2, DEC, QChar('0')).arg(0, 3, DEC, QChar('0')));
}

void SettingsScreen::UpdateTravelUptime(int travelMs)
{
    float mT = travelMs / 60;
    float hT = mT / 60;

    float deltaH = hT - std::floor(hT);
    mT = deltaH * 60;

    float deltaM = mT - std::floor(mT);

    int s = deltaM * 60;
    int h = hT;
    int m = mT;


//    ui->travelUptimeLabel->setText(Reveles::TRAVEL_UPTIME_STR.arg(h, 2, DEC, QChar('0')).arg(m, 2, DEC, QChar('0')).arg(s, 2, DEC, QChar('0')).arg(0, 3, DEC, QChar('0')));
}

void SettingsScreen::addToLog(QString str)
{
    if(str.endsWith(QChar('\n')))
        str.truncate(str.length() - 1);

    ui->logView->append(str);
}

void SettingsScreen::on_buttonBox_rejected()
{
    this->close();
}

void SettingsScreen::on_buttonBox_accepted()
{
    /// TODO: Save setting changes (Once settings are added)
    this->close();
}

void SettingsScreen::MagReading(MagDirection md)
{
//    addToLog(QString("MagUpdate"));
    ui->magReadingLabel->setText(Reveles::MAG_GUI_DATA.arg(md.x, 5, 'g', 5, QChar('0')).arg(md.y, 5, 'g', 5, QChar('0')).arg(md.z, 5, 'g', 5, QChar('0')));
}

void SettingsScreen::AccelReading(AccelDirection ad)
{
//    addToLog(QString("AccelUpdate"));
    ui->accelReadingLabel->setText(Reveles::ACCEL_GUI_DATA.arg(ad.x, 5, 'g', 5, QChar('0')).arg(ad.y, 5, 'g', 5, QChar('0')).arg(ad.z, 5, 'g', 5, QChar('0')));
}

void SettingsScreen::GyroReading(GyroDirection gd)
{
//    addToLog(QString("GyroUpdate"));
    ui->gyroReadingLabel->setText(Reveles::GYRO_GUI_DATA.arg(gd.x, 5, 'g', 5, QChar('0')).arg(gd.y, 5, 'g', 5, QChar('0')).arg(gd.z, 5, 'g', 5, QChar('0')));
}

void SettingsScreen::setArduino(bool good)
{
    if(good)
        ui->arduinoStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->arduinoStatusLabel->setStyleSheet("QLabel { background-color: red; }");
}

void SettingsScreen::setNucleo(bool good, int idx)
{
    if(good)
    {
        if(idx == 0)
            ui->frontNucleoStatusLabel->setStyleSheet("QLabel { background-color: green; }");
        else if(idx == 1)
            ui->rearNucleoStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
    else
    {
        if(idx == 0)
            ui->frontNucleoStatusLabel->setStyleSheet("QLabel { background-color: red; }");
        else if(idx == 1)
                ui->rearNucleoStatusLabel->setStyleSheet("QLabel { background-color: red; }");
    }
}

void SettingsScreen::setPIR(bool stat, bool front)
{
    if(front)
    {
        if(stat)
            ui->pirFStatusLabel->setStyleSheet("QLabel { background-color: red; }");
        else
            ui->pirFStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
    else
    {
        if(stat)
            ui->pirRStatusLabel->setStyleSheet("QLabel { background-color: red; }");
        else
            ui->pirRStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    }
}

void SettingsScreen::on_settingsPB_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}

void SettingsScreen::on_logPB_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
}

void SettingsScreen::on_statusPB_clicked()
{
    ui->tabWidget->setCurrentIndex(2);
}

void SettingsScreen::on_aboutPB_clicked()
{
    ui->tabWidget->setCurrentIndex(3);
}
