#ifndef ADDLOCATIONDIALOG_H
#define ADDLOCATIONDIALOG_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "revelesgui.h"
#include "../../RPi/Common/datatypes.h"

class RevelesGui;

//struct KeyboardItem { int key; const QChar *label; };
//static KeyboardItem kbLayoutArray[5][11] =
//{
//    {{Qt::Key_Escape, new QChar(ESC)}, {Qt::Key_1, new QChar('1')}, {Qt::Key_2, new QChar('2')}, {Qt::Key_3, new QChar('3')}, {Qt::Key_4, new QChar('4')}, {Qt::Key_5, new QChar('5')}, {Qt::Key_6, new QChar('6')}, {Qt::Key_7, new QChar('7')}, {Qt::Key_8, new QChar('8')}, {Qt::Key_9, new QChar('9')}, {Qt::Key_0, new QChar('0')}},// {Qt::Key_hyphen, '-'}},
//    {{Qt::Key_Q, new QChar('q')}, {Qt::Key_W, new QChar('w')}, {Qt::Key_E, new QChar('e')}, {Qt::Key_R, new QChar('r')}, {Qt::Key_T, new QChar('t')}, {Qt::Key_Y, new QChar('y')}, {Qt::Key_U, new QChar('u')}, {Qt::Key_I, new QChar('i')}, {Qt::Key_O, new QChar('o')}, {Qt::Key_P, new QChar('p')}, {Qt::Key_Backspace, new QChar(BKSP)}},
//    {{Qt::Key_A, new QChar('a')}, {Qt::Key_S, new QChar('s')}, {Qt::Key_D, new QChar('d')}, {Qt::Key_F, new QChar('f')}, {Qt::Key_G, new QChar('g')}, {Qt::Key_H, new QChar('h')}, {Qt::Key_J, new QChar('j')}, {Qt::Key_K, new QChar('k')}, {Qt::Key_L, new QChar('l')}, {Qt::Key_Semicolon, new QChar(';')}, {Qt::Key_Apostrophe, new QChar('\'')}},
//    {{Qt::Key_Shift, new QChar(SHIFT)}, {Qt::Key_Z, new QChar('z')}, {Qt::Key_X, new QChar('x')}, {Qt::Key_C, new QChar('c')}, {Qt::Key_V, new QChar('v')}, {Qt::Key_B, new QChar('b')}, {Qt::Key_N, new QChar('n')}, {Qt::Key_M, new QChar('m')}, {Qt::Key_Comma, new QChar(',')}, {Qt::Key_Period, new QChar('.')}, {Qt::Key_Enter, new QChar(ENTER)}},
//    {{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')},{Qt::Key_Space, new QChar(' ')}}
//};

class AddLocationDialog : public QWidget
{
    Q_OBJECT
public:
    explicit AddLocationDialog(QWidget *parent = nullptr);

private:
    void generateKeyboard();

signals:

private slots:
    void addNewLoc();
    void getLocation();
    void buttonClicked(int key);

public slots:

private:
    QPushButton *exit;
    QPushButton *cancel;
    QPushButton *currentLoc;
    QLabel *name, *coordLat, *coordLong;
    QLineEdit *locName, *lat, *lon;

    QHBoxLayout *ctrlBtns, *nameLayout, *coordLayout;
    QHBoxLayout *kNumRow, *kQRow, *kARow, *kZRow, *kSpace;
    QVBoxLayout *vLayout, *kLayout;
};

#endif // ADDLOCATIONDIALOG_H
