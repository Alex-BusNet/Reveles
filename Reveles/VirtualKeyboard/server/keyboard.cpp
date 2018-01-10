#include "keyboard.h"

#include "keyboard.h"
#include <QGridLayout>
#include <QSignalMapper>
#include <QPushButton>

static QString keyToCharacter(int key)
{
    for(int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 11; j++)
        {
            if(kbLayoutArray[i][j].key == key)
                return QString::fromLatin1(kbLayoutArray[i][j].label);
        }
    }

    return QString();
}

Keyboard::Keyboard(QWidget *parent)
{
    setWindowFlags(Qt::WindowDoesNotAcceptFocus | Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::BypassWindowManagerHint);

    QGridLayout *gridLayout = new QGridLayout(this);
    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(int)), SLOT(buttonClicked(int)));

    int row = 0, col = 0;

    for (int i = 0; i < 5; i++)
    {
        for(int j = 0; j < 11; j++)
        {
            QPushButton *b = new QPushButton();
            b->setFixedWidth(width);
            b->setText(QString(kbLayoutArray[i][j].label));

            mapper->setMapping(b, kbLayoutArray[i][j].key);
            connect(b, SIGNAL(clicked(bool)), mapper, SLOT(map()));
            gridLayout->addWidget(b, i, j);
        }
    }
}

void Keyboard::showKeyboard()
{
    QWidget::show();
}

void Keyboard::hideKeyboard()
{
   QWidget::hide();
}

bool Keyboard::keyboardVisible() const
{
    return QWidget::isVisible();
}

void Keyboard::buttonClicked(int key)
{
    if((key == Qt::Key_Enter) || (key == Qt::Key_Backspace))
        emit specialKeyClicked(key);
    else
        emit keyClicked(keyToCharacter(key));
}
