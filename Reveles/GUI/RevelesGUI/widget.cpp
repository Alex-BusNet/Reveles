#include "widget.h"
#include "ui_widget.h"


QString menuStyle = "QWidget { background-color: #5E5E5E; } QPushButton { background-color: #CECECE; } QPushButton:pressed { background-color: #bdfd96 }";

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setStyleSheet(menuStyle);
}

Widget::~Widget()
{
    delete ui;
}
