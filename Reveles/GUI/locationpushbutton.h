#ifndef LOCATIONPUSHBUTTON_H
#define LOCATIONPUSHBUTTON_H

#include <QToolButton>
#include "../RPi/Common/datatypes.h"

class LocationPushButton : public QToolButton
{
    Q_OBJECT
public:
    LocationPushButton(QString label, GPSCoord coord);
    ~LocationPushButton();

    GPSCoord GetIndex();
    QString GetName();

    void SetName(QString name);
    void SetIndex(GPSCoord coord);

private:
    GPSCoord location;

signals:
    void lpb_clicked(LocationPushButton*);

private slots:
    void lpb_click_handler();
};

#endif // LOCATIONPUSHBUTTON_H

inline LocationPushButton::LocationPushButton(QString label, GPSCoord coord) :
    QToolButton()
{
    this->setText(label);
    this->setCheckable(true);
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    location = coord;
    connect(this, SIGNAL(clicked(bool)), this, SLOT(lpb_click_handler()));
}

inline LocationPushButton::~LocationPushButton()
{
    disconnect(this, SIGNAL(clicked(bool)), this, SLOT(lpb_click_handler()));
}

inline GPSCoord LocationPushButton::GetIndex()
{
    return location;
}

inline QString LocationPushButton::GetName()
{
    return this->text();
}

inline void LocationPushButton::SetName(QString name)
{
    this->setText(name);
}

inline void LocationPushButton::SetIndex(GPSCoord coord)
{
    this->location = coord;
}

inline void LocationPushButton::lpb_click_handler()
{
    emit lpb_clicked(this);
}

