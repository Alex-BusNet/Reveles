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

void SettingsScreen::on_buttonBox_rejected()
{
    this->close();
}

void SettingsScreen::on_buttonBox_accepted()
{
    /// TODO: Save setting changes (Once settings are added)
    this->close();
}
