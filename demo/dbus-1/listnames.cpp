#include <stdio.h>

#include <QDebug>
#include <QStringList>
#include <QtDBus/QtDBus>
#include <QCoreApplication>

/**
 * 三种方法，列出已注册的名称
 */

void method1()
{
    qDebug() << "Method 1:";

    QDBusReply<QStringList> reply = QDBusConnection::sessionBus().interface()->registeredServiceNames();
    if (!reply.isValid()) {
        qDebug() << "Error:" << reply.error().message();
        exit(1);
    }
    const QStringList values = reply.value();
    for (const QString &name : values)
        qDebug() << name;
}

void method2()
{
    qDebug() << "Method 2:";

    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusInterface dbus_iface("org.freedesktop.DBus", "/org/freedesktop/DBus",
                              "org.freedesktop.DBus", bus);
    qDebug() << dbus_iface.call("ListNames").arguments().at(0);
}

void method3()
{
    qDebug() << "Method 3:";
    qDebug() << QDBusConnection::sessionBus().interface()->registeredServiceNames().value();
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    if (!QDBusConnection::sessionBus().isConnected()) {
        qDebug() << "Cannot connect to the D-Bus session bus.\n"
                    "To start it, run:\n"
                    "\teval `dbus-launch --auto-syntax`\n";
    }

//    method1();
//    method2();
//    method3();

    return 0;
}
