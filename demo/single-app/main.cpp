#include "single-app.h"

#include <QApplication>

class SingleDemo : SingleApp {
    Q_OBJECT

public:
    explicit SingleDemo (int argc, char* argv[], const char* appName = "demo")
        : SingleApp (argc, argv, appName, true)
    {
        setApplicationVersion ("1.0.0");
        setApplicationName("demo-desktop");
        if (this->isPrimary()) {
            qDebug() << "first process";
        }
    }

private:

};

int main(int argc, char *argv[])
{
    qDebug() << "start ...";
    SingleApp a(argc, argv);
    if (a.isSecondary()) {
        qDebug() << "already running!!!";
        return 0;
    }

    return a.exec();
}
