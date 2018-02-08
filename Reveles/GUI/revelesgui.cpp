#include "revelesgui.h"
#include "ui_revelesgui.h"
#include <iostream>

Q_GLOBAL_STATIC(RevelesGui, rGui)

RevelesGui *RevelesGui::instance()
{
    return rGui;
}

RevelesGui::RevelesGui(com::reveles::RevelesCoreInterface *iface, RevelesDBusAdaptor *rda, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RevelesGui),
    rci(iface),
    rdba(rda)
{
//    instance()->setObjectName("RevelesGUI");

    qRegisterMetaType<GPSCoord>("GPSCoord");
    qDBusRegisterMetaType<GPSCoord>();

    QString menuStyle = "QWidget { background-color: #0d0d0d; color: white; }";
    menuStyle += "QToolButton, QPushButton { background-color: #0d0d0d; border-radius: 4px; border: 2px solid #5ac5cc; color: white; }";
    menuStyle += "QToolButton:checked { background-color: #bdfd96 }";

    ui->setupUi(this);
    ss = NULL;
    ald = NULL;
    trigOn = false;
    currentLoc = GPSCoord{0,0};
    hasComms = false;

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

    this->setStyleSheet(menuStyle);

    this->ui->exitBtn->setShortcut(QKeySequence(Qt::Key_Escape));

    this->ui->distLabel->setText(QChar(0x221E));

    commTimer = new QTimer();
    commTimer->setInterval(500);
    connect(commTimer, &QTimer::timeout, this, &RevelesGui::commTimeout);
    commTimer->start();
}

RevelesGui::~RevelesGui()
{
    delete ui;

    if(commTimer != NULL)
    {
        if(commTimer->isActive())
            commTimer->stop();

        delete commTimer;
    }

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
    GPSCoord dest{pb->GetIndex().latitude, pb->GetIndex().longitude};
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

    /// TODO: Add Confirm selection dialog box.

    if(hasComms)
        emit SendDestination(dest);
}

void RevelesGui::updateLocation(GPSCoord gpsc)
{
    currentLoc = gpsc;
    ui->testingLabel->setText(QString::number(gpsc.latitude) + ", " + QString::number(gpsc.longitude));
    TrigDispToggle();    
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
    if(ald != NULL)
        delete ald;

    ald = new AddLocationDialog();
    ald->setGeometry((this->width() / 2) - 400, this->height() - 500, 800, 300);
    ald->show();
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

void RevelesGui::setDBusInterface(com::reveles::RevelesCoreInterface *iface)
{
    rci = iface;

    connect(rci, &RevelesDBusInterface::commResponse, this, &RevelesGui::commCheck);
    connect(rci, &RevelesDBusInterface::locationUpdate, this, &RevelesGui::updateLocation);
    connect(rci, &RevelesDBusInterface::aboutToQuit, this, &RevelesGui::close);
}

void RevelesGui::setDBusAdaptor(RevelesDBusAdaptor *rda)
{
    rdba = rda;

    connect(this, &RevelesGui::sendCommCheck, rdba, &RevelesDBusAdaptor::commQuery);
    connect(this, &RevelesGui::aboutToQuit, rdba, &RevelesDBusAdaptor::aboutToQuit);
    connect(this, &RevelesGui::RequestLocation, rdba, &RevelesDBusAdaptor::requestCurrentLocation);
    connect(this, &RevelesGui::SendDestination, rdba, &RevelesDBusAdaptor::setDestination);
    connect(this, &RevelesGui::SendMapUpdateInterval, rdba, &RevelesDBusAdaptor::setMapUpdateInterval);
}

GPSCoord RevelesGui::getLocation()
{
    return currentLoc;
}

void RevelesGui::commCheck(bool good)
{
    hasComms = good;

    if(good)
        ui->EchoLabel->setStyleSheet("QLabel { background-color: green; }");
    else
        ui->EchoLabel->setStyleSheet("QLabel { background-color: red; }");
}

void RevelesGui::on_exitBtn_clicked()
{
    emit aboutToQuit();
}

//=====================================================================
// These functions are for development purposes

void RevelesGui::displayDist(float dist, QString unit)
{
    if (dist < 0)
        this->ui->distLabel->setText(QChar(0x221E));
    else
        this->ui->distLabel->setText(QString("%0 %1").arg(dist).arg(unit));

    this->ui->TrigLabel->setStyleSheet("QLabel { background-color: transparent; } ");
}

void RevelesGui::TrigDispToggle()
{
    if( !trigOn )
        this->ui->TrigLabel->setStyleSheet("QLabel { background-color: green; } ");
    else
        this->ui->TrigLabel->setStyleSheet("QLabel { background-color: red; } ");

    trigOn = !trigOn;
}

//======================================================================

void RevelesGui::on_settingsScreenPB_clicked()
{
    if(ss != NULL)
        delete ss;

    ss = new SettingsScreen(this);
    ss->setGeometry(0,0, 800, 400);
    ss->show();
}

void RevelesGui::on_mapPB_clicked()
{

}

void RevelesGui::commTimeout()
{
    if(!hasComms)
    {
        emit sendCommCheck();
    }
    else
    {
        commTimer->stop();
    }
}
