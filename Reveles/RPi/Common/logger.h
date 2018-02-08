#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QString>
#include <iostream>
#include <iomanip>
#include "datatypes.h"
#include <QFile>

class Logger //: public QObject
{
    //Q_OBJECT

public:
    static LoggerFlags flags;
    static QFile logFile;

    ~Logger();

    static void InitLog();
    static void SetLogFlags(LoggerFlags lf);
    static void SetPrecision(int p);
    static void ClearLogFlags();
    static void CloseLog();

    static void write(const QObject *src, QString message);
    static void writeLine(const QObject *src, QString message);

private:
    Logger();
    static void write(const QObject *src, std::string message);
};

#endif // LOGGER_H
