#include "testkey.h"

#include <QThread>
#include <QDebug>
#include <QObject>
#include <QCoreApplication>
#include <QGSettings/QGSettings>

/**
 * 默认存放路径: /usr/share/glib-2.0/schemas
 * 选择 keep-window-open.gschema.xml测试
 *
 * schema id = "keep-window-open"
 */

int main (int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    TestKey k(nullptr);
    QGSettings g("org.gnome.Epiphany.ui");

    qDebug() << "keep-window-open:" << g.get("keep-window-open").toInt();

    g.set("keep-window-open", true);
    qDebug() << "keep-window-open:" << g.get("keep-window-open").toInt();

    qDebug()<<g.keys();

    QObject::connect(&g, SIGNAL(changed), &k, SLOT(testKey));

    return app.exec();
}
