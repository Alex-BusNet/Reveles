#include "revelesgui.h"
#include "ui_revelesgui.h"
#include "../RPi/Common/messages.h"
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
    menuStyle += "QToolButton:enabled, QPushButton:enabled { background-color: #0d0d0d; border-radius: 4px; border: 2px solid #5ac5cc; color: white; }";
    menuStyle += "QToolButton:checked { background-color: #bdfd96 }";
    menuStyle += "QToolButton:disabled, QPushButton:disabled { color: black; background-color: #bde0e3; }";

    ui->setupUi(this);
    ss = NULL;
    ald = NULL;
    trigOn = false;
    currentLoc = GPSCoord{0,0};
    hasComms = false;
    demoOn = false;
    selectedPBIdx = -1;

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
    ui->startNavigationPB->setEnabled(false);
    ui->editLocationPB->setEnabled(false);
    ui->endNavigationPB->setEnabled(false);
    ui->removeLocationPB->setEnabled(false);

    ss = new SettingsScreen();
    ss->setGeometry(0,0, this->width(), this->height());
    ss->hide();

    this->setStyleSheet(menuStyle);

    this->ui->exitBtn->setShortcut(QKeySequence(Qt::Key_Escape));

    setupLocations();

    commTimer = new QTimer();
    commTimer->setInterval(500);
    connect(commTimer, &QTimer::timeout, this, &RevelesGui::commTimeout);

    updateTimer = new QTimer();
    updateTimer->setInterval(2000);
    connect(updateTimer, &QTimer::timeout, this, &RevelesGui::draw);

    guiUptime = std::chrono::steady_clock::now();

    updateTimer->start();
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
    int deselectCount = 0;
    // Deselect any other active buttons.
    for(int j = 0; j < lpbs.size(); j++)
    {
        if(lpbs[j] != NULL && lpbs[j] != pb)
        {
            if(lpbs[j]->isChecked())
                lpbs[j]->setChecked(false);

            deselectCount++;
        }
        else if(lpbs[j] == pb && pb != NULL) { selectedPBIdx = j; }
    }

    if(deselectCount == lpbs.size())
    {
        ui->removeLocationPB->setEnabled(false);
        ui->editLocationPB->setEnabled(false);
        ui->startNavigationPB->setEnabled(false);
        selectedPBIdx = -1;
    }
    else
    {
        ui->removeLocationPB->setEnabled(true);
        ui->editLocationPB->setEnabled(true);
        ui->startNavigationPB->setEnabled(true);
    }

}

void RevelesGui::updateLocation(GPSCoord gpsc)
{
    currentLoc = gpsc;
    if(ss != NULL)
        ss->setCoordText(Reveles::GPS_DATA.arg((gpsc.latitude)).arg(QString::number(gpsc.longitude)));

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

void RevelesGui::magUpdate(MagDirection md)
{
    if(ss != NULL)
    {
        ss->MagReading(md);
        ss->UpdateGUIUptime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - guiUptime).count());
    }
}

void RevelesGui::accelUpdate(AccelDirection ad)
{
    if(ss != NULL)
    {
        ss->AccelReading(ad);
        ss->UpdateGUIUptime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - guiUptime).count());
    }
}

void RevelesGui::gyroUpdate(GyroDirection gd)
{
    if(ss != NULL)
    {
        ss->GyroReading(gd);
        ss->UpdateGUIUptime(std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - guiUptime).count());
    }
}

void RevelesGui::ArduinoStatus(bool good)
{
    if(ss != NULL)
        ss->setArduino(good);
}

void RevelesGui::NucleoStatus(bool good, int idx)
{
    if(ss != NULL)
        ss->setNucleo(good, idx);
}

void RevelesGui::PIRStatus(bool stat, bool front)
{
    if(ss != NULL)
    {
        ss->setPIR(stat, front);
        ss->UpdateGUIUptime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - guiUptime).count());
    }
}

void RevelesGui::setupLocations()
{
    if(lpbs.size() != 0)
    {
        foreach(LocationPushButton *lpb, lpbs)
        {
            gl->removeWidget(lpb);
        }

        lpbs.clear();
    }

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
    connect(rci, &RevelesDBusInterface::AccelUpdate, this, &RevelesGui::accelUpdate);
    connect(rci, &RevelesDBusInterface::MagUpdate, this, &RevelesGui::magUpdate);
    connect(rci, &RevelesDBusInterface::GyroUpdate, this, &RevelesGui::gyroUpdate);
    connect(rci, &RevelesDBusInterface::ArduinoFound, this, &RevelesGui::ArduinoStatus);
    connect(rci, &RevelesDBusInterface::NucleoFound, this, &RevelesGui::NucleoStatus);
    connect(rci, &RevelesDBusInterface::PIRStatus, this, &RevelesGui::PIRStatus);
    connect(rci, &RevelesDBusInterface::USReadings, this, &RevelesGui::usUpdate);
    connect(rci, &RevelesDBusInterface::TOFReadings, this, &RevelesGui::tofUpdate);
    connect(rci, &RevelesDBusInterface::motorUpdate, this, &RevelesGui::motorStat);
    connect(rci, &RevelesDBusInterface::servoUpdate, this, &RevelesGui::servoStat);
}

void RevelesGui::setDBusAdaptor(RevelesDBusAdaptor *rda)
{
    rdba = rda;

    connect(this, &RevelesGui::sendCommCheck, rdba, &RevelesDBusAdaptor::commQuery);
    connect(this, &RevelesGui::aboutToQuit, rdba, &RevelesDBusAdaptor::aboutToQuit);
    connect(this, &RevelesGui::RequestLocation, rdba, &RevelesDBusAdaptor::requestCurrentLocation);
    connect(this, &RevelesGui::SendDestination, rdba, &RevelesDBusAdaptor::setDestination);
    connect(this, &RevelesGui::SendMapUpdateInterval, rdba, &RevelesDBusAdaptor::setMapUpdateInterval);
    connect(this, &RevelesGui::NavigationAbort, rdba, &RevelesDBusAdaptor::EndNavigation);
    connect(this, &RevelesGui::StartDemoMode, rdba, &RevelesDBusAdaptor::StartDemo);
}

GPSCoord RevelesGui::getLocation()
{
    return currentLoc;
}

void RevelesGui::commCheck(bool good)
{
    hasComms = good;

    if(ss != NULL)
    {
        ss->setDBusStatus(good);
        ss->UpdateGUIUptime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - guiUptime).count());
    }
}

void RevelesGui::on_exitBtn_clicked()
{
    this->saveLocations();
    emit aboutToQuit();

    if(!hasComms)
        this->close();

    // This primarily to allow the GUI to close in the
    // event the CORE is hung up and can't send the kill
    // signal back to the GUI.
    hasComms = false;
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

//    if (ss != NULL)
//        ss->setUSDistReading(t);
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

void RevelesGui::draw()
{
    this->update();
}

void RevelesGui::on_endNavigationPB_clicked()
{
    if(ss != NULL)
        ss->UpdateTravelUptime(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - travelUptime).count());

    emit NavigationAbort();
    setLocation(NULL);
    ui->endNavigationPB->setEnabled(false);
    ui->demoPB->setEnabled(true);

    if(demoOn)
    {
        if(ss != NULL)
            ss->addToLog(QString("Ending demo..."));

        demoOn = false;
    }
}

void RevelesGui::on_removeLocationPB_clicked()
{
    if(selectedPBIdx != -1)
    {
        LocationPushButton *lpb = lpbs.at(selectedPBIdx);
        disconnect(lpb, SIGNAL(lpb_clicked(LocationPushButton*)), this, SLOT(setLocation(LocationPushButton*)));

        gl->removeWidget(lpb);
        lpbs.removeAt(selectedPBIdx);

        saveLocations();
        setupLocations();
    }
}

void RevelesGui::on_editLocationPB_clicked()
{

}

void RevelesGui::on_startNavigationPB_clicked()
{
    if(selectedPBIdx != -1)
    {
        GPSCoord dest{lpbs.at(selectedPBIdx)->GetIndex().latitude, lpbs.at(selectedPBIdx)->GetIndex().longitude};

        if(hasComms)
        {
            travelUptime = std::chrono::steady_clock::now();

            emit SendDestination(dest);

            // Switch the Screen to show the map
            ui->tabWidget->setCurrentIndex(1);
            ui->endNavigationPB->setEnabled(true);
            ui->startNavigationPB->setEnabled(false);
            ui->demoPB->setEnabled(false);
        }
    }
}

void RevelesGui::on_demoPB_clicked()
{
    if(!demoOn)
    {
        if(ss != NULL)
        {
            ss->addToLog(QString("Starting demo mode..."));
        }

        demoOn = true;
        ui->tabWidget->setCurrentIndex(0);
        ui->endNavigationPB->setEnabled(true);
        ui->startNavigationPB->setEnabled(false);
        ui->demoPB->setEnabled(false);
        emit StartDemoMode();
    }
}

void RevelesGui::tofUpdate(int idx, float val)
{
    if(ss != NULL)
        ss->setToFReading(idx, val);
}

void RevelesGui::usUpdate(int idx, float val)
{
    if(ss != NULL)
        ss->setUSDistReading(idx, val);
}

void RevelesGui::servoStat(bool front, uint8_t dir)
{
    if(ss != NULL)
        ss->setServoStatus(front, dir);
}

void RevelesGui::motorStat(uint8_t dir)
{
    if(ss != NULL)
        ss->setMotorStatus(dir);
}
