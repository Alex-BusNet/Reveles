#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <iostream>
#include <iomanip>
#include "datatypes.h"
#include <QFile>

class Logger : public QObject
{
    Q_OBJECT

public:
    Logger();
    ~Logger();
    static Logger* instance();

    static LoggerFlags flags;
    static QFile logFile;

    static void InitLog();
    static void SetLogFlags(LoggerFlags lf);
    static void SetPrecision(int p);
    static void ClearLogFlags();
    static void CloseLog();

    static void write(const QObject *src, QString message);
    static void writeLine(const QObject *src, QString message);

private:
    static void write(const QObject *src, std::string message);
    void sendSignal(QString msg);
signals:
    void newMessage(const QString msg);
};

#endif // LOGGER_H
