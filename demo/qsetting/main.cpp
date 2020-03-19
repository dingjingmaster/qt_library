#include <QCoreApplication>
#include <QSettings>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

//    QSettings setting("company", "software");                 // 默认保存载 $HOME/.config/ 下 company/software.conf

    QSettings setting("config.ini", QSettings::IniFormat);      // 默认保存在 当前运行路径下

    setting.setValue("aaa/bbb", 20);

    int bbb = setting.value("aaa/bbb").toInt();
    qDebug() << "bbb: " << bbb;

    return a.exec();
}
