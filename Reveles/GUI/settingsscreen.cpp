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

    this->setStyleSheet("QWidget:active { background-color: #787878; } QLabel { background-color: red; } QLabel#ipLabel { background-color: transparent; }");
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
}

void SettingsScreen::setAGStatus(bool stat)
{
    if(stat)
        ui->agStatusLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->agStatusLabel->setStyleSheet("QLabel { background-color: red; }");
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
        ui->locationUpdateLabel->setStyleSheet("QLabel { background-color: red; }");
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
