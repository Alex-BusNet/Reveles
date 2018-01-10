#include "qvkimplatforminputcontext.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QGuiApplication>
#include <QInputMethodEvent>
#include <QWidget>

QVkImPlatformInputContext::QVkImPlatformInputContext() : focusObject(0)
{
    kbInterface = new QDBusInterface("com.reveles.inputmethod", "/VirtualKeyboard", "local.server.Keyboard", QDBusConnection::sessionBus(), this);

    connect(kbInterface, SIGNAL(keyClicked(QString)), SLOT(keyboardKeyClicked(QString)));
    connect(kbInterface, SIGNAL(specialKeyClicked(int)), SLOT(keyboardSpecialKeyCliked(int)));
}

QVkImPlatformInputContext::~QVkImPlatformInputContext()
{

}

bool QVkImPlatformInputContext::isValid() const
{
    return kbInterface->isValid();
}

void QVkImPlatformInputContext::setFocusObject(QObject *object)
{
    focusObject = object;
}

void QVkImPlatformInputContext::showInputPanel()
{
    QPoint globalPos(0,0);

    QWidget *w = qobject_cast<QWidget*>(focusObject);
    if(w)
        globalPos = w->mapToGlobal(QPoint(0, 0));

    kbInterface->call("showKeyboard", globalPos.x(), globalPos.y());
}

void QVkImPlatformInputContext::hideInputPanel()
{
    kbInterface->call("hideKeyboard");
}

bool QVkImPlatformInputContext::isInputPanelVisible() const
{
    const QDBusReply<bool> reply = kbInterface->call("keyboardVisible");

    if(reply.isValid())
        return reply.value();
    else
        return false;
}

void QVkImPlatformInputContext::keyboardSpecialKeyCliked(int key)
{
    if(!focusObject)
        return;

    if(key == Qt::Key_Enter)
    {
        QKeyEvent *pressEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QGuiApplication::postEvent(focusObject, pressEvent);
        QKeyEvent *releaseEvent = new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier);
        QGuiApplication::postEvent(focusObject, releaseEvent);
    }
    else if(key == Qt::Key_Backspace)
    {
        QKeyEvent *pressEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        QGuiApplication::postEvent(focusObject, pressEvent);
        QKeyEvent *releaseEvent = new QKeyEvent(QKeyEvent::KeyRelease, Qt::Key_Backspace, Qt::NoModifier);
        QGuiApplication::postEvent(focusObject, releaseEvent);
    }
}

void QVkImPlatformInputContext::keyboardKeyClicked(const QString &character)
{
    if(!focusObject)
        return;

    QInputMethodEvent event;
    event.setCommitString(character);

    QGuiApplication::sendEvent(focusObject, &event);
}
