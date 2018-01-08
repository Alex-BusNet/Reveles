#ifndef KEYBOARD_H
#define KEYBOARD_H

/*
 * This class was written with reference to
 * Qt Input Method - Virtual Keyboard at
 * www.kdab.com/qt-input-method-virtual-keyboard
 */

#include <QWidget>

class Keyboard
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = Q_NULLPTR);

private slots:
    void showKeyboard();
    void hideKeyboard();
    bool keyboardVisible() const;
    void buttonClicked(int key);

signals:
    void specialKeyClicked(int key);
    void keyClicked(const QString &text);

};

#endif // KEYBOARD_H
