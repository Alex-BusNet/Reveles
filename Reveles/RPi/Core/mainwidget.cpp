#include "mainwidget.h"
//#include "ui_mainwidget.h"

MainWidget::MainWidget(/*QWidget *parent, */ int argc, char *argv[])// :
    //QWidget(parent),
    //ui(new Ui::MainWidget)
{
    //ui->setupUi(this);

    rc = new RevelesCore();
    rc->InitPyModule(argc, argv);
}

MainWidget::~MainWidget()
{
    rc->FreePyModule();
    delete rc;
//    delete ui;
}
