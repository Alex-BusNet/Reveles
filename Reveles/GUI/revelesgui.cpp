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

    mapView = new MapView();

    ui->tabWidget->addTab(sc, "Locations");
    ui->tabWidget->addTab(mapView, "Map");
    QIcon *loc, *nLoc, *settings;

    loc = new QIcon("Assets/Icons/locations.png");
    nLoc = new QIcon("Assets/Icons/newLocation.png");
    settings = new QIcon("Assets/Icons/settings.png");

    ui->locationsScreenPB->setIcon(*loc);
    ui->addLocationPB->setIcon(*nLoc);
    ui->settingsScreenPB->setIcon(*settings);

    ss = new SettingsScreen(this);
    ss->setGeometry(0,0, this->width(), this->height());
    ss->hide();

    this->setStyleSheet(menuStyle);

    this->ui->exitBtn->setShortcut(QKeySequence(Qt::Key_Escape));

    setupLocations();

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

    if(mapView != NULL)
        delete mapView;

    if(ss != NULL)
        delete ss;
}

void RevelesGui::setLocation(LocationPushButton *pb)
{
    GPSCoord dest{pb->GetIndex().latitude, pb->GetIndex().longitude};

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
    if(ss != NULL)
        ss->setCoordText(QString::number(gpsc.latitude) + ", " + QString::number(gpsc.longitude));

    TrigDispToggle();

    if(ui->tabWidget->currentIndex() == 1)
        mapView->update();
}

void RevelesGui::magStatus(bool good)
{
    if(ss != NULL)
        ss->setMagStatus(good);
}

void RevelesGui::agStatus(bool good)
{
    if(ss != NULL)
        ss->setAGStatus(good);
}

void RevelesGui::logMessage(QString msg)
{
    if(ss != NULL)
        ss->addToLog(msg);
}

void RevelesGui::setupLocations()
{
    QFile locSaveFile("Assets/Data/locations.json");
    if(!locSaveFile.open(QIODevice::ReadOnly))
    {
        if(ss != NULL)
            ss->addToLog(QString("[ RevelesGUI ] Location save data not found.\n"));
        else
            std::cout << "{NL}[ RevelesGUI ] Location save data not found." << std::endl;

        return;
    }

    if(ss != NULL)
        ss->addToLog(QString("[ RevelesGUI ] Loading saved locations...\n"));
    else
        std::cout << "{NL}[ RevelesGUI ] Loading saved locations..." << std::endl;

    QByteArray lpbByteArr = locSaveFile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(lpbByteArr);
    QJsonArray arr = doc.array();

    for(int i = 0; i < arr.size(); i++)
    {
        QJsonObject obj = arr.at(i).toObject();
        addLocation(obj["name"].toString(),
                GPSCoord{obj["latitude"].toDouble(), obj["longitude"].toDouble()});
    }

    locSaveFile.close();

    if(ss != NULL)
        ss->addToLog(QString("[ RevelesGUI ] Locations loaded.\n"));
    else
        std::cout << "{NL}[ RevelesGUI ] Locations loaded." << std::endl;
}

void RevelesGui::saveLocations()
{
    if(ss != NULL)
        ss->addToLog(QString("[ RevelesGUI ] Saving locations...\n"));
    else
        std::cout << "{NL}[ RevelesGUI ] Saving locations..." << std::endl;

    if(!QDir("Assets/Data").exists())
    {
        QDir::current().mkdir("Assets/Data");
    }

    QFile locSaveFile("Assets/Data/locations.json");
    if(!locSaveFile.open(QIODevice::WriteOnly))
    {
        if(ss != NULL)
            ss->addToLog(QString("[ RevelesGUI ] Could not open Location data save file!\n"));
        else
            qWarning("{NL}[ Reveles GUI ] Could not open Location data save file!");

        return;
    }

    QJsonDocument doc;
    QJsonObject lpb;
    QJsonArray lpbArr;

    foreach(LocationPushButton *p, lpbs)
    {
        lpb["name"] = p->GetName();
        lpb["latitude"] = p->GetIndex().latitude;
        lpb["longitude"] = p->GetIndex().longitude;
        lpbArr.push_back(lpb);
    }

    doc.setArray(lpbArr);
    locSaveFile.write(doc.toJson());
    locSaveFile.flush();
    locSaveFile.close();

    if(ss != NULL)
        ss->addToLog(QString("[ RevelesGUI ] Locations saved.\n"));
    else
        std::cout << "{NL}[ RevelesGUI ] Locations saved." << endl;
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
    connect(rci, &RevelesDBusInterface::getAGStatus, this, &RevelesGui::agStatus);
    connect(rci, &RevelesDBusInterface::getMagStatus, this, &RevelesGui::magStatus);
    connect(rci, &RevelesDBusInterface::sendLogMessage, this, &RevelesGui::logMessage);
    connect(rci, &RevelesDBusInterface::sendPathInfo, mapView, &MapView::SetPathInfo);
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

    if(ss != NULL)
        ss->setDBusStatus(good);
}

void RevelesGui::on_exitBtn_clicked()
{
    this->saveLocations();
    emit aboutToQuit();
}

//=====================================================================
// These functions are for development purposes

void RevelesGui::displayDist(float dist, QString unit)
{
    QString t = "";
    if (dist < 0)
        t = QString(QChar(0x221E));
    else
        t = (QString("%0 %1").arg(dist).arg(unit));

    if (ss != NULL)
        ss->setUSDistReading(t);
}

void RevelesGui::TrigDispToggle()
{
    if(ss != NULL)
        ss->setLocUpdateStatus(trigOn);

    trigOn = !trigOn;
}

//======================================================================

void RevelesGui::on_settingsScreenPB_clicked()
{
    ss->show();
}

void RevelesGui::on_mapPB_clicked()
{
    ui->tabWidget->setCurrentIndex(1);
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
        mapView->setScreenSize(ui->tabWidget->width(), ui->tabWidget->height());

        if(ss != NULL)
            ss->addToLog(QString("[ RevelesGUI ] mapView Screen size set to: %1, %2\n")
                         .arg(ui->tabWidget->width())
                         .arg(ui->tabWidget->height()));
    }
}

void RevelesGui::on_locationsScreenPB_clicked()
{
    ui->tabWidget->setCurrentIndex(0);
}
