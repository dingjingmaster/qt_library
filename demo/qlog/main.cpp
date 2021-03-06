#include <QCoreApplication>

#include <QFile>
#include <QStandardPaths>
#include <QTime>
#include <QDebug>
#include <iostream>
#include <qlogging.h>

void messageOutput (QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray currentTime = QTime::currentTime().toString().toLocal8Bit();

    QString logFilePath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/test.log";
    std::cout << "log path: " << logFilePath.toStdString() << std::endl; // /tmp/test.log

    bool showDebug = true;
    if (!QFile::exists(logFilePath)) {
        showDebug = false;
    }
    FILE *log_file = nullptr;

    if (showDebug) {
        log_file = fopen(logFilePath.toLocal8Bit().constData(), "a+");
    }

    const char *file = context.file ? context.file : "";
    const char *function = context.function ? context.function : "";
    switch (type) {
    case QtDebugMsg:
        if (!log_file) {
            break;
        }
        fprintf(log_file, "Debug: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtInfoMsg:
        fprintf(log_file? log_file: stdout, "Info: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtWarningMsg:
        fprintf(log_file? log_file: stderr, "Warning: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtCriticalMsg:
        fprintf(log_file? log_file: stderr, "Critical: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    case QtFatalMsg:
        fprintf(log_file? log_file: stderr, "Fatal: %s: %s (%s:%u, %s)\n", currentTime.constData(), localMsg.constData(), file, context.line, function);
        break;
    }

    if (log_file)
        fclose(log_file);
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(messageOutput);
    QCoreApplication a(argc, argv);

    qDebug() << "aa";
    qInfo() << "bb";
    /*
在 /tmp/test.log
Debug: 11:51:36: aa (../qlog/main.cpp:60, int main(int, char**))
Info: 11:51:36: bb (../qlog/main.cpp:61, int main(int, char**))
     */

    return a.exec();
}
