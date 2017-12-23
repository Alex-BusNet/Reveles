#include "mainwidget.h"
#include "ui_mainwidget.h"

MainWidget::MainWidget(QWidget *parent, int argc, char *argv[]) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);

    rc = new RevelesCore();
}

MainWidget::~MainWidget()
{
    delete rc;
    delete ui;
}
