#ifndef ADDLOCATIONDIALOG_H
#define ADDLOCATIONDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "revelesgui.h"
#include "datatypes.h"

class RevelesGui;

class AddLocationDialog : public QWidget
{
    Q_OBJECT
public:
    explicit AddLocationDialog(QWidget *parent = nullptr);

signals:

private slots:
    void addNewLoc();
    void getLocation();

public slots:

private:
    QPushButton *exit;
    QPushButton *cancel;
    QPushButton *currentLoc;
    QLabel *name, *coordLat, *coordLong;
    QLineEdit *locName, *lat, *lon;

    QHBoxLayout *ctrlBtns, *nameLayout, *coordLayout;
    QVBoxLayout *vLayout;
};

#endif // ADDLOCATIONDIALOG_H
