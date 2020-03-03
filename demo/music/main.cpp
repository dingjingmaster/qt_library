#include <QCoreApplication>
#include <QMediaPlayer>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QMediaPlayer player;

    player.setMedia(QUrl::fromLocalFile("/data/文件归档/音乐/Windy Hill.mp3"));
    player.setVolume(10);
    player.play();

    return a.exec();
}
