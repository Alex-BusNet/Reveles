#include "logger.h"
#include <QDir>
#include <ctime>

Logger::Logger()
{

}

Logger::~Logger()
{
    if(logFile.isOpen())
    {
        logFile.flush();
        logFile.close();
    }
}

void Logger::InitLog()
{
    flags = NO_LOG_FLAGS;

    if(!QDir("logs").exists())
        QDir::current().mkdir("logs");

    char filename[15];

    std::time_t rawtime;
    struct tm *tInfo;
    time(&rawtime);
    tInfo = std::gmtime(&rawtime);

    std::strftime(filename, sizeof(filename), "%m_%d_%Y.txt", tInfo);

    logFile.setFileName(QString("logs/%1").arg(filename));
    if(!logFile.open(QIODevice::Append))
    {
        std::cout << "[ Logger ] Could not open log file " << filename << std::endl;
    }
    else
    {
        QByteArray arr;
        arr.append(flags);
        logFile.write(arr);
    }
}

void Logger::SetLogFlags(LoggerFlags lf)
{
    if(lf != NO_LOG_FLAGS)
    {
        if((lf & BOOL_AS_ALPHA) == BOOL_AS_ALPHA)
            std::cout << std::boolalpha << "set BOOL_AS_ALPHA" << std::endl;

        if((lf & SHOW_POINT) == SHOW_POINT)
            std::cout << std::showpoint << "set SHOW_POINT" << std::endl;

        if((lf & SHOW_POS_NEG) == SHOW_POS_NEG)
            std::cout << std::showpos << "set SHOW_POS_NET" << std::endl;

        if((lf & FIXED_LENGTH) == FIXED_LENGTH)
            std::cout << std::fixed << "set FIXED_LENGTH" << std::endl;
    }

    flags = lf;
}

void Logger::SetPrecision(int p)
{
    std::cout << std::setprecision(p);
}

void Logger::ClearLogFlags()
{
    if(flags != NO_LOG_FLAGS)
    {
        std::cout << std::setprecision(6);

        if((flags & BOOL_AS_ALPHA) == BOOL_AS_ALPHA)
            std::cout << std::noboolalpha;

        if((flags & SHOW_POINT) == SHOW_POINT)
            std::cout << std::noshowpoint;

        if((flags & SHOW_POS_NEG) == SHOW_POS_NEG)
            std::cout << std::noshowpos;

        if((flags & FIXED_LENGTH) == FIXED_LENGTH)
            std::cout.unsetf(std::ios::fixed);

        flags = NO_LOG_FLAGS;
    }
}

void Logger::CloseLog()
{
    if(logFile.isOpen())
    {
        logFile.flush();
        logFile.close();
    }
}

void Logger::write(const QObject *src, std::string message)
{
    std::cout << "[ " << src->objectName().toStdString() << " ] " << message;

    if(logFile.isOpen())
    {
        logFile.write((QString("[ %1 ] ").arg(src->objectName()).toStdString() + message).c_str());
    }
}

void Logger::write(const QObject *src, QString message)
{
    write(src, message.toStdString());
}

void Logger::writeLine(const QObject *src, QString message)
{
    message += "\n";
    write(src, message);
}
