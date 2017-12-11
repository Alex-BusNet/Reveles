#include "addlocationdialog.h"
#include <random>
#include <time.h>

AddLocationDialog::AddLocationDialog(QWidget *parent) : QWidget(parent)
{
    srand(time(0));

    this->setFixedSize(400, 300);

    exit = new QPushButton("Add");
    cancel = new QPushButton("Cancel");
    currentLoc = new QPushButton("Use current Location");

    exit->setFixedHeight(75);
    cancel->setFixedHeight(75);
    currentLoc->setFixedHeight(50);

    locName = new QLineEdit();
    lat = new QLineEdit();
    lon = new QLineEdit();

    locName->setFixedHeight(75);
    lat->setFixedHeight(75);
    lon->setFixedHeight(75);

    name = new QLabel("Location Name: ");
    coordLat = new QLabel("Lat: ");
    coordLong = new QLabel("Long: ");

    exit->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    cancel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    currentLoc->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    connect(currentLoc, SIGNAL(clicked(bool)), this, SLOT(getLocation()));
    connect(exit, SIGNAL(clicked(bool)), this, SLOT(addNewLoc()));
    connect(cancel, SIGNAL(clicked(bool)), this, SLOT(close()));

    ctrlBtns = new QHBoxLayout();
    nameLayout = new QHBoxLayout();
    coordLayout = new QHBoxLayout();
    vLayout = new QVBoxLayout();

    nameLayout->addWidget(name);
    nameLayout->addWidget(locName);

    coordLayout->addWidget(coordLat);
    coordLayout->addWidget(lat);
    coordLayout->addWidget(coordLong);
    coordLayout->addWidget(lon);
    coordLayout->addWidget(currentLoc);

    ctrlBtns->addWidget(exit);
    ctrlBtns->addWidget(cancel);

    vLayout->addLayout(nameLayout);
    vLayout->addLayout(coordLayout);
    vLayout->addLayout(ctrlBtns);

    this->setLayout(vLayout);
}

void AddLocationDialog::addNewLoc()
{
    RevelesGui::instance()->addLocation(locName->text(), GPSCoord{lat->text().toFloat(), lon->text().toFloat()});
    this->close();
}

void AddLocationDialog::getLocation()
{
    lat->setText(QString::number((static_cast<double>(rand()) / RAND_MAX) * 180.0));
    lon->setText(QString::number((static_cast<double>(rand()) / RAND_MAX) * 180.0));
}
