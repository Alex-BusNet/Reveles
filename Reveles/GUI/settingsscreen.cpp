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
    ui->tabWidget->setCurrentIndex(0);
    ui->coordinateLabel->setText("Latitude: ? \nLongitude: ? \n");

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
    //{ background-color: red; } QLabel#ipLabel,#accelReadingLabel,#magReadingLabel,#gyroReadingLabel,#infoLabel

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
    ui->coordinateLabel->setText(coord);
}

void SettingsScreen::setUSDistReading(QString reading)
{
    ui->usDistLabel->setText(reading);
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

void SettingsScreen::setPIR(bool stat)
{
    if(stat)
        ui->pirStatusLabel->setStyleSheet("QLabel { background-color: red; }");
    else
        ui->pirStatusLabel->setStyleSheet("QLabel { background-color: green; }");
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
