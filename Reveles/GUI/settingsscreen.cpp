#include "settingsscreen.h"
#include "ui_settingsscreen.h"
//#include "../RPi/Common/messages.h"

SettingsScreen::SettingsScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScreen)
{
    ui->setupUi(this);
//    ui->infoLabel->text().append(QString("Version: %1").arg(Reveles::REVELES_VERSION));
}

SettingsScreen::~SettingsScreen()
{
    delete ui;
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
