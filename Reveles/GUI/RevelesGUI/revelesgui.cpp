#include "revelesgui.h"
#include "ui_revelesgui.h"
#include <QDebug>

Q_GLOBAL_STATIC(RevelesGui, rGui)


RevelesGui *RevelesGui::instance()
{
    return rGui;
}

QString menuStyle = "QWidget { background-color: white; } QToolButton { background-color: #CECECE; } QToolButton:checked { background-color: #bdfd96 }";

RevelesGui::RevelesGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RevelesGui)
{
    ui->setupUi(this);

    scrollWidget = new QWidget();
    sa = new QScrollArea();
    sa->setWidgetResizable(true);
    sa->setWidget(scrollWidget);
    gl = new QGridLayout();
    scrollWidget->setLayout(gl);

    sc = new QWidget;
    sc->setLayout(new QVBoxLayout);
    sc->layout()->addWidget(sa);
    ui->tabWidget->addTab(sc, "Locations");
    QIcon *loc, *nLoc, *settings;

    loc = new QIcon("Assets/Icons/locations.png");
    nLoc = new QIcon("Assets/Icons/newLocation.png");
    settings = new QIcon("Assets/Icons/settings.png");

    ui->locationsScreenPB->setIcon(*loc);
    ui->addLocationPB->setIcon(*nLoc);
    ui->settingsScreenPB->setIcon(*settings);

    this->setLayout(ui->horizontalLayout);
    this->setStyleSheet(menuStyle);
}

RevelesGui::~RevelesGui()
{
    delete ui;

    if(lpbs.size() > 0)
    {
        for(int i = 0; i < lpbs.size(); i++)
        {
            disconnect(lpbs.at(i), SIGNAL(lpb_clicked(LocationPushButton*)), this, SLOT(setLocation(LocationPushButton*)));
            delete lpbs.at(i);
        }
    }

    delete gl;
    delete scrollWidget;
    delete sa;
}

void RevelesGui::setLocation(LocationPushButton *pb)
{
    ui->testingLabel->setText(QString::number(pb->GetIndex().latitude) + ", " + QString::number(pb->GetIndex().longitude));

    // Deselect any other active buttons.
    for(int j = 0; j < lpbs.size(); j++)
    {
        if(lpbs[j] != NULL && lpbs[j] != pb)
        {
            if(lpbs[j]->isChecked())
                lpbs[j]->setChecked(false);
        }
    }
}

void RevelesGui::setupLocations()
{
    for(int row = 0; row < 1; row++)
    {
        for(int col = 0; col < 1; col++)
        {
            LocationPushButton *pb = new LocationPushButton(QString::number(lpbs.size()), GPSCoord{0.0, 0.0});
            connect(pb, SIGNAL(lpb_clicked(LocationPushButton*)), this, SLOT(setLocation(LocationPushButton*)));
            gl->addWidget(pb, row, col);
            pb->setFixedSize(pb->width() / 3, pb->height() / 3);
            lpbs.push_back(pb);
        }
    }
}

void RevelesGui::on_addLocationPB_clicked()
{
    AddLocationDialog *adl = new AddLocationDialog();
    adl->show();
}

void RevelesGui::addLocation(QString name, GPSCoord coord)
{
    LocationPushButton *pb = new LocationPushButton(name, coord);
    connect(pb, SIGNAL(lpb_clicked(LocationPushButton*)), this, SLOT(setLocation(LocationPushButton*)));
    int row = gl->rowCount();
    int col = 0;

    if(lpbs.size() % 3 == 0)
        row++;
    else
        col = lpbs.size() % 3;

    gl->addWidget(pb, row-1, col);
    pb->setFixedSize(pb->width() / 3, pb->height() / 3);
    lpbs.push_back(pb);
}
