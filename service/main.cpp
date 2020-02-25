#include "httpserver.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QHostAddress address;

    address.setAddress("127.0.0.1");

    HTTPServer::instance().run(address, 6000);

    return a.exec();
}
