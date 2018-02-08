#include "addlocationdialog.h"
#include <random>
#include <time.h>
#include <iostream>
#include <QSignalMapper>
#include <QGuiApplication>
#include <QInputMethodEvent>

static QString keyToCharacter(int key)
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 11; j++)
        {
            if(kbLayoutArray[i][j].key == key)
                return QString(kbLayoutArray[i][j].label);
        }
    }

    return QString();
}

AddLocationDialog::AddLocationDialog(QWidget *parent) : QWidget(parent)
{
    QString menuStyle = "QWidget { background-color: #0d0d0d; color: white; }";
    menuStyle += "QPushButton { background-color: #0d0d0d; border-radius: 4px; border: 2px solid #5ac5cc; color: white; }";

    this->setStyleSheet(menuStyle);
    
    srand(time(0));

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

    kNumRow = new QHBoxLayout();
    kQRow = new QHBoxLayout();
    kARow = new QHBoxLayout();
    kZRow = new QHBoxLayout();
    kSpace = new QHBoxLayout();

    kLayout = new QVBoxLayout();

    generateKeyboard();

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
    vLayout->addLayout(kLayout);

    this->setLayout(vLayout);
}

void AddLocationDialog::generateKeyboard()
{
    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), SLOT(buttonClicked(int)));

    for (int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 11; j++)
        {
            QPushButton *b = new QPushButton(this);
            b->setFixedWidth(800/11);
            b->setFixedHeight(30);
            b->setText(QString(kbLayoutArray[i][j].label));

            mapper->setMapping(b, kbLayoutArray[i][j].key);
            connect(b, SIGNAL(clicked(bool)), mapper, SLOT(map()));

            if(i == 0)
                kNumRow->addWidget(b);
            else if (i == 1)
                kQRow->addWidget(b);
            else if(i == 2)
                kARow->addWidget(b);
            else if(i == 3)
                kZRow->addWidget(b);
        }
    }

    QPushButton *b = new QPushButton(this);
    b->setFixedWidth(800);
    b->setFixedHeight(30);
    b->setText(" ");

    mapper->setMapping(b, Qt::Key_Space);
    connect(b, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    kSpace->addWidget(b);

    kLayout->addLayout(kNumRow);
    kLayout->addLayout(kQRow);
    kLayout->addLayout(kARow);
    kLayout->addLayout(kZRow);
    kLayout->addLayout(kSpace);
}

void AddLocationDialog::addNewLoc()
{
    RevelesGui::instance()->addLocation(locName->text(), GPSCoord{lat->text().toFloat(), lon->text().toFloat()});
    this->close();
}

void AddLocationDialog::getLocation()
{
    emit RevelesGui::instance()->RequestLocation();
    GPSCoord g = RevelesGui::instance()->getLocation();
    lat->setText(QString::number(g.latitude));
    lon->setText(QString::number(g.longitude));
//    lat->setText(QString::number((static_cast<double>(rand()) / RAND_MAX) * 180.0));
//    lon->setText(QString::number((static_cast<double>(rand()) / RAND_MAX) * 180.0));
}

void AddLocationDialog::buttonClicked(int key)
{
    locName->setText(locName->text() + keyToCharacter(key));
}
