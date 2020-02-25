#include "httpserver.h"

#include <QEventLoop>
#include <QTcpSocket>

HTTPServer &HTTPServer::instance()
{
    static HTTPServer obj;
    return obj;
}

void HTTPServer::run(const QHostAddress &address, const quint16 &port)
{
    mHTTPServer->listen(address, port);
}

QString HTTPServer::getRequestHead(QByteArray& b)
{
    QByteArray str;
    for (int i = 0; i < b.size() -  3; ++i) {
       if((b[i] == '\r') && (b[i + 1] == '\n') && (b[i + 2] == '\r') && (b[i + 3] == '\n')) break;
       str.append(b[i]);
    }
    return QString(str);
}

QString HTTPServer::getRequestBody(QByteArray& b)
{
    int i = 0;
    QByteArray str;
    for (i = 0; i < b.size() -  3; ++i) {
       if((b[i] == '\r') && (b[i + 1] == '\n') && (b[i + 2] == '\r') && (b[i + 3] == '\n')) {
           while((b[i] == '\r') || (b[i] == '\n')) ++i;
           break;
       }
    }
    for (; i < b.size(); ++i) {
        str.append(b[i]);
    }
    return QString(str.toBase64());
}

void HTTPServer::newConnection()
{
    QTcpSocket* socket = mHTTPServer->nextPendingConnection();
    qDebug() << "new";
    QObject::connect(socket, &QTcpSocket::readyRead, this, &HTTPServer::readyRead);
}

void HTTPServer::readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());

    if(socket) {
        QString requestString;
        QByteArray request;

        socket->waitForReadyRead(1000);
        while(!socket->atEnd()) {
            request = socket->readAll();
        }

        requestString = "================== new connection ==================\n";
        requestString += "== header ==\n" + getRequestHead(request) + "\n";
        requestString += "== body ==\n" + getRequestBody(request) + "\n";
        requestString += "====================== close =======================\n";

        QByteArray response = requestString.toUtf8();

        qDebug() << requestString;

        QString http = "HTTP/1.1 200 OK\r\n";
        http += "Server: dingjing - demo\r\n";
        http += "Content-Type: text/html;charset=utf-8\r\n";
        http += "Connection: keep-alive\r\n";
        http += QString("Content-Length: %1\r\n\r\n").arg(QString::number(response.size()));

        socket->write(http.toUtf8());
        socket->write(response);
        socket->flush();
        socket->close();
    }
}

void HTTPServer::newConnectionError(QAbstractSocket::SocketError error)
{
    qDebug() << "connection error!" << error;
}

HTTPServer::HTTPServer(QObject *parent) : QObject(parent)
{
    mHTTPServer = new QTcpServer(this);
    mHTTPServer->setMaxPendingConnections(1024);

    // 新连接
    QObject::connect(mHTTPServer, &QTcpServer::newConnection,this,&HTTPServer::newConnection);
    // 错误
    QObject::connect(mHTTPServer, &QTcpServer::acceptError, this, &HTTPServer::newConnectionError);
}

HTTPServer::~HTTPServer()
{

}
