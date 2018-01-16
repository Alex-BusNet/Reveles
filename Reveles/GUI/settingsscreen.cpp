#include "settingsscreen.h"
#include "ui_settingsscreen.h"

SettingsScreen::SettingsScreen(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsScreen)
{
    ui->setupUi(this);
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
