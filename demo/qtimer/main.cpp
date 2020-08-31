#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QObject>

int main(int argc, char *argv[])
{
    int i = 0;
    static int times = 1000;
    QCoreApplication a(argc, argv);

    /* only execute once */
    QTimer::singleShot(200, [=]() {
        qDebug() << "a";
    });

    QTimer* timer = new QTimer();
    QObject::connect (timer, &QTimer::timeout, [=]() {
        qDebug () << "b" << i;

        timer->stop();
    });
    timer->start();

    return a.exec();
}
