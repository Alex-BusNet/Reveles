#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include "../Core/revelescore.h"

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0, int argc = 0, char *argv[] = {});
    ~MainWidget();

private:
    Ui::MainWidget *ui;

    RevelesCore *rc;
};

#endif // MAINWIDGET_H
