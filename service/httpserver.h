#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QHostAddress>
#include <QObject>
#include <QTcpServer>

class HTTPServer : public QObject
{
    Q_OBJECT
public:
    static HTTPServer& instance();
    void run(const QHostAddress &address=QHostAddress::Any, const quint16& port=6000);
    QString getRequestHead(QByteArray&);
    QString getRequestBody(QByteArray&);

private slots:
    void newConnection();
    void readyRead();
    void newConnectionError(QAbstractSocket::SocketError error);

private:
    explicit HTTPServer(QObject* parent=nullptr);
    ~HTTPServer();

private:
    QTcpServer* mHTTPServer;

signals:

};

#endif // HTTPSERVER_H
