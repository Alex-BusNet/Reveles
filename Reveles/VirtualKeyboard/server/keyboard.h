#ifndef KEYBOARD_H
#define KEYBOARD_H

/*
 * This class was written with reference to
 * Qt Input Method - Virtual Keyboard at
 * www.kdab.com/qt-input-method-virtual-keyboard
 */

#include <QWidget>

#define BKSP    0xD83EDCA4
#define ENTER   0x2386
#define SHIFT   0xD83EDC2D

struct KeyboardItem { int key; const char *label; };
static KeyboardItem kbLayoutArray[5][11] =
{
    {{Qt::Key_1, "1"}, {Qt::Key_2, "2"}, {Qt::Key_3, "3"}, {Qt::Key_4, "4"}, {Qt::Key_5, "5"}, {Qt::Key_6, "6"}, {Qt::Key_7, "7"}, {Qt::Key_8, "8"}, {Qt::Key_9, "9"}, {Qt::Key_0, "0"}, {Qt::Key_hyphen, "-"}},
    {{Qt::Key_Q, "q"}, {Qt::Key_W, "w"}, {Qt::Key_E, "e"}, {Qt::Key_R, "r"}, {Qt::Key_T, "t"}, {Qt::Key_Y, "y"}, {Qt::Key_U, "u"}, {Qt::Key_I, "i"}, {Qt::Key_O, "o"}, {Qt::Key_P, "p"}, {Qt::Key_Backspace, (const char*)QChar(BKSP).toLatin1()}},
    {{Qt::Key_A, "a"}, {Qt::Key_S, "s"}, {Qt::Key_D, "d"}, {Qt::Key_F, "f"}, {Qt::Key_G, "g"}, {Qt::Key_H, "h"}, {Qt::Key_J, "j"}, {Qt::Key_K, "k"}, {Qt::Key_L, "l"}, {Qt::Key_Semicolon, ";"}, {Qt::Key_Apostrophe, "'"}},
    {{Qt::Key_Shift, (const char*)QChar(SHIFT).toLatin1()}, {Qt::Key_Z, "z"}, {Qt::Key_X, "x"}, {Qt::Key_C, "c"}, {Qt::Key_V, "v"}, {Qt::Key_B, "b"}, {Qt::Key_N, "n"}, {Qt::Key_M, "m"}, {Qt::Key_Comma, ","}, {Qt::Key_Period, "."}, {Qt::Key_Enter, (const char*)QChar(ENTER).toLatin1()}},
    {{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""}}
};

static KeyboardItem specialKbLayoutArray[5][11] =
{
    {{Qt::Key_Exclam, "!"}, {Qt::Key_At, "@"}, {Qt::Key_NumberSign, "#"}, {Qt::Key_Dollar, "$"}, {Qt::Key_Percent, "%"}, {Qt::Key_AsciiCircum, "^"}, {Qt::Key_Ampersand, "&"}, {Qt::Key_Asterisk, "*"}, {Qt::Key_ParenLeft, "("}, {Qt::Key_ParenRight, ")"}, {Qt::Key_Underscore, "_"}},
    {{Qt::Key_Q, "Q"}, {Qt::Key_W, "W"}, {Qt::Key_E, "E"}, {Qt::Key_R, "R"}, {Qt::Key_T, "T"}, {Qt::Key_Y, "Y"}, {Qt::Key_U, "U"}, {Qt::Key_I, "I"}, {Qt::Key_O, "O"}, {Qt::Key_P, "P"}, {Qt::Key_Backspace, (const char*)QChar(BKSP).toLatin1()}},
    {{Qt::Key_A, "A"}, {Qt::Key_S, "S"}, {Qt::Key_D, "D"}, {Qt::Key_F, "F"}, {Qt::Key_G, "G"}, {Qt::Key_H, "H"}, {Qt::Key_J, "J"}, {Qt::Key_K, "K"}, {Qt::Key_L, "L"}, {Qt::Key_Colon, ":"}, {Qt::Key_QuoteDbl, "\""}},
    {{Qt::Key_Shift, (const char*)QChar(SHIFT).toLatin1()}, {Qt::Key_Z, "Z"}, {Qt::Key_X, "X"}, {Qt::Key_C, "C"}, {Qt::Key_V, "V"}, {Qt::Key_B, "B"}, {Qt::Key_N, "N"}, {Qt::Key_M, "M"}, {Qt::Key_Less, "<"}, {Qt::Key_Greater, ">"}, {Qt::Key_Enter,  (const char*)QChar(ENTER).toLatin1()}},
    {{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""},{Qt::Key_Space, ""}}
};

class Keyboard : public QWidget
{
    Q_OBJECT

public:
    explicit Keyboard(QWidget *parent = Q_NULLPTR);

private:
    int rowOffset = 5, colOffset = 2; // px
    int width = 800 / 11;      // px

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
