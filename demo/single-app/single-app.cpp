#include "single-app.h"
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <QtCore/QtGlobal>
#include <QCryptographicHash>
#include <QDataStream>
#include <QThread>
#include <QTime>
#include <QtNetwork/QLocalServer>

void SingleApp::slotConnectionEstablished()
{
    qDebug() << "aaa";
    QLocalSocket *nextConnSocket = mServer->nextPendingConnection();
    mConnectionMap.insert(nextConnSocket, ConnectionInfo());

    QObject::connect(nextConnSocket, &QLocalSocket::aboutToClose,
        [nextConnSocket, this]() {
            auto &info = mConnectionMap[nextConnSocket];
            Q_EMIT this->slotClientConnectionClosed( nextConnSocket, info.instanceId );
        }
    );

    QObject::connect(nextConnSocket, &QLocalSocket::disconnected,
        [nextConnSocket, this]() {
            mConnectionMap.remove(nextConnSocket);
            nextConnSocket->deleteLater();
        }
    );

    QObject::connect(nextConnSocket, &QLocalSocket::readyRead,
        [nextConnSocket, this]() {
            auto &info = mConnectionMap[nextConnSocket];
            switch(info.stage) {
            case StageHeader:
                readInitMessageHeader(nextConnSocket);
                break;
            case StageBody:
                readInitMessageBody(nextConnSocket);
                break;
            case StageConnected:
                Q_EMIT this->slotDataAvailable( nextConnSocket, info.instanceId );
                break;
            default:
                break;
            };
        }
    );
}

void SingleApp::slotDataAvailable(QLocalSocket *dataSocket, quint32 instanceId)
{
    Q_EMIT receivedMessage(instanceId, dataSocket->readAll());
}

void SingleApp::slotClientConnectionClosed(QLocalSocket *closedSocket, quint32 instanceId)
{
    if (closedSocket->bytesAvailable() > 0) {
        Q_EMIT slotDataAvailable(closedSocket, instanceId);
    }
}


SingleApp::SingleApp(int &argc, char *argv[], const char *appName, bool allowSecondary, Options options, int timeout)
        : QApplication (argc, argv)
{
    qDebug() << "SignalApp ...";
    mServer = nullptr;
    mSocket = nullptr;
    mMemory = nullptr;
    mInstanceNumber = -1;

#if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    // On Android and iOS since the library is not supported fallback to
    // standard QApplication behaviour by simply returning at this point.
    qWarning() << "SingleApplication is not supported on Android and iOS systems.";
    return;
#endif

    // Store the current mode of the program
    mOptions = options;

    // Generating an application ID used for identifying the shared memory
    // block and QLocalServer
    genBlockServerName(appName);

#ifdef Q_OS_UNIX
    // By explicitly attaching it and then deleting it we make sure that the
    // memory is deleted even after the process has crashed on Unix.
    mMemory = new QSharedMemory(mBlockServerName);
    mMemory->attach();
    delete mMemory;
#endif
    // Guarantee thread safe behaviour with a shared memory block.
    mMemory = new QSharedMemory(mBlockServerName);

    // Create a shared memory block
    if(mMemory->create(sizeof (InstancesInfo))) {
        // Initialize the shared memory block
        mMemory->lock();
        qDebug() << "start init block ...";
        initializeMemoryBlock();
        qDebug() << "init block success !!!";
        mMemory->unlock();
    } else {
        // Attempt to attach to the memory segment
        if (!mMemory->attach()) {
            qCritical() << "SingleApplication: Unable to attach to shared memory block.";
            qCritical() << mMemory->errorString();
            ::exit( EXIT_FAILURE );
        }
    }

    InstancesInfo* inst = static_cast<InstancesInfo*>(mMemory->data());
    QTime time;
    time.start();

    qDebug() << "start while ...";
    // Make sure the shared memory block is initialised and in consistent state
    while(true) {
        mMemory->lock();
        if(blockChecksum() == inst->checksum) break;
        if(time.elapsed() > 5000) {
            qWarning() << "SingleApplication: Shared memory block has been in an inconsistent state from more than 5s. Assuming primary instance failure.";
            initializeMemoryBlock();
        }

        mMemory->unlock();

        // Random sleep here limits the probability of a collision between two racing apps
        qsrand(QDateTime::currentMSecsSinceEpoch() % std::numeric_limits<uint>::max());
        QThread::sleep(8 + static_cast<unsigned long>(static_cast<float>(qrand()) / RAND_MAX * 10));
    }

    qDebug () << "end while ...";

    qDebug() << inst->primary << " " << inst->checksum << " " << inst->secondary << " " << inst->primaryPid;

    if (inst->primary == false) {
        qDebug() << "start primary ...";
        startPrimary();
        mMemory->unlock();
        qDebug() << "is not primary ...";
        return;
    }

    // Check if another instance can be started
    if(allowSecondary) {
        qDebug () << "allow secondary ...";
        inst->secondary += 1;
        inst->checksum = blockChecksum();
        mInstanceNumber = inst->secondary;
        startSecondary();
        if (mOptions & Mode::SecondaryNotification) {
            connectToPrimary (timeout, SecondaryInstance);
        }
        mMemory->unlock();
        return;
    }

    mMemory->unlock();

    qDebug() << "connect to primary ...";
    connectToPrimary (timeout, NewInstance);

    qDebug() << "SUCCESS ...";
    ::exit(EXIT_SUCCESS);
}

SingleApp::~SingleApp()
{
    if (nullptr != mSocket) {
        mSocket->close();
        delete mSocket;
    }

    mMemory->lock();
    InstancesInfo* inst = static_cast<InstancesInfo*>(mMemory->data());
    if(mServer != nullptr) {
        mServer->close();
        delete mServer;
        inst->primary = false;
        inst->primaryPid = -1;
        inst->checksum = blockChecksum();
    }
    mMemory->unlock();

    delete mMemory;
}

bool SingleApp::isPrimary()
{
    return mServer != nullptr;
}

bool SingleApp::isSecondary ()
{
    return mServer == nullptr;
}

quint32 SingleApp::instanceId()
{
    return mInstanceNumber;
}

bool SingleApp::sendMessage(QByteArray message, int timeout)
{
    // Nobody to connect to
    if(isPrimary()) return false;

    // Make sure the socket is connected
    connectToPrimary(timeout,  Reconnect);

    mSocket->write(message);
    bool dataWritten = mSocket->waitForBytesWritten(timeout);
    mSocket->flush();
    return dataWritten;
}

void SingleApp::genBlockServerName(const char *appName)
{
    QCryptographicHash appData(QCryptographicHash::Sha256);
    appData.addData (appName, 17);
    appData.addData (SingleApp::QApplication::applicationName().toUtf8());
    appData.addData (SingleApp::QApplication::organizationName().toUtf8());
    appData.addData (SingleApp::QApplication::organizationDomain().toUtf8());

    if(!(mOptions & SingleApp::Mode::ExcludeAppVersion)) {
        appData.addData( SingleApp::QApplication::applicationVersion().toUtf8());
    }

    if(!(mOptions & SingleApp::Mode::ExcludeAppPath) ) {
#ifdef Q_OS_WIN
        appData.addData( SingleApplication::app_t::applicationFilePath().toLower().toUtf8() );
#else
        appData.addData( SingleApp::QApplication::applicationFilePath().toUtf8() );
#endif
    }

    // User level block requires a user specific data in the hash
    if( mOptions & SingleApp::Mode::User ) {
#ifdef Q_OS_WIN
        wchar_t username [ UNLEN + 1 ];
        // Specifies size of the buffer on input
        DWORD usernameLength = UNLEN + 1;
        if( GetUserNameW( username, &usernameLength ) ) {
            appData.addData( QString::fromWCharArray(username).toUtf8() );
        } else {
            appData.addData( qgetenv("USERNAME") );
        }
#endif
#ifdef Q_OS_UNIX
        QByteArray username;
        uid_t uid = geteuid();
        struct passwd *pw = getpwuid(uid);
        if(pw) {
            username = pw->pw_name;
        }
        if(username.isEmpty()) {
            username = qgetenv("USER");
        }
        appData.addData(username);
#endif
    }

    // Replace the backslash in RFC 2045 Base64 [a-zA-Z0-9+/=] to comply with
    // server naming requirements.
    mBlockServerName = appData.result().toBase64().replace("/", "_");
}

void SingleApp::initializeMemoryBlock()
{
    InstancesInfo* inst = static_cast<InstancesInfo*>(mMemory->data());
    inst->primary = false;
    inst->secondary = 0;
    inst->primaryPid = -1;
    inst->checksum = blockChecksum();
}

void SingleApp::startPrimary()
{
    // Successful creation means that no main process exists
    // So we start a QLocalServer to listen for connections
    QLocalServer::removeServer(mBlockServerName);
    mServer = new QLocalServer();

    // Restrict access to the socket according to the
    // SingleApplication::Mode::User flag on User level or no restrictions
    if(mOptions & SingleApp::Mode::User) {
        mServer->setSocketOptions( QLocalServer::UserAccessOption );
    } else {
        mServer->setSocketOptions( QLocalServer::WorldAccessOption );
    }

    qDebug() << "start listenning ...";
    mServer->listen(mBlockServerName);
    qDebug() << "start connecting ...";
    QObject::connect(mServer, &QLocalServer::newConnection, this, &SingleApp::slotConnectionEstablished);

    qDebug() << "start cast ...";
    // Reset the number of connections
    InstancesInfo* inst = static_cast <InstancesInfo*>(mMemory->data());
    qDebug() << "reset connections ...";

    inst->primary = true;
    qDebug() << "application pid ...";
    inst->primaryPid = applicationPid();
    qDebug() << "block check sum ...";
    inst->checksum = blockChecksum();

    mInstanceNumber = 0;
}

void SingleApp::startSecondary()
{

}

void SingleApp::connectToPrimary(int msecs, ConnectionType connectionType)
{
    // Connect to the Local Server of the Primary Instance if not already
    // connected.
    if(mSocket == nullptr) {
        mSocket = new QLocalSocket();
    }

    // If already connected - we are done;
    if(mSocket->state() == QLocalSocket::ConnectedState)
        return;

    // If not connect
    if(mSocket->state() == QLocalSocket::UnconnectedState || mSocket->state() == QLocalSocket::ClosingState ) {
        mSocket->connectToServer(mBlockServerName);
    }

    // Wait for being connected
    if(mSocket->state() == QLocalSocket::ConnectingState ) {
        mSocket->waitForConnected( msecs );
    }

    // Initialisation message according to the SingleApplication protocol
    if(mSocket->state() == QLocalSocket::ConnectedState ) {
        // Notify the parent that a new instance had been started;
        QByteArray initMsg;
        QDataStream writeStream(&initMsg, QIODevice::WriteOnly);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        writeStream.setVersion(QDataStream::Qt_5_6);
#endif

        writeStream << mBlockServerName.toLatin1();
        writeStream << static_cast<quint8>(connectionType);
        writeStream << mInstanceNumber;
        quint16 checksum = qChecksum(initMsg.constData(), static_cast<quint32>(initMsg.length()));
        writeStream << checksum;

        // The header indicates the message length that follows
        QByteArray header;
        QDataStream headerStream(&header, QIODevice::WriteOnly);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        headerStream.setVersion(QDataStream::Qt_5_6);
#endif
        headerStream << static_cast <quint64>( initMsg.length() );

        mSocket->write( header );
        mSocket->write( initMsg );
        mSocket->flush();
        mSocket->waitForBytesWritten( msecs );
    }
}

quint16 SingleApp::blockChecksum()
{
    return qChecksum(static_cast <const char *>(mMemory->data()), offsetof(InstancesInfo, checksum));
}

qint64 SingleApp::primaryPid()
{
    qint64 pid;

    mMemory->lock();
    InstancesInfo* inst = static_cast<InstancesInfo*>(mMemory->data());
    pid = inst->primaryPid;
    mMemory->unlock();

    return pid;
}

void SingleApp::readInitMessageHeader(QLocalSocket *sock)
{
    if (!mConnectionMap.contains(sock)) {
        return;
    }

    if( sock->bytesAvailable() < ( qint64 )sizeof( quint64 ) ) {
        return;
    }

    QDataStream headerStream( sock );

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    headerStream.setVersion( QDataStream::Qt_5_6 );
#endif

    // Read the header to know the message length
    quint64 msgLen = 0;
    headerStream >> msgLen;
    ConnectionInfo &info = mConnectionMap[sock];
    info.stage = StageBody;
    info.msgLen = msgLen;

    if ( sock->bytesAvailable() >= (qint64) msgLen ) {
        readInitMessageBody( sock );
    }
}

void SingleApp::readInitMessageBody(QLocalSocket *sock)
{
    if (!mConnectionMap.contains( sock )) {
        return;
    }

    ConnectionInfo &info = mConnectionMap[sock];
    if( sock->bytesAvailable() < ( qint64 )info.msgLen ) {
        return;
    }

    // Read the message body
    QByteArray msgBytes = sock->read(info.msgLen);
    QDataStream readStream(msgBytes);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
    readStream.setVersion( QDataStream::Qt_5_6 );
#endif

    // server name
    QByteArray latin1Name;
    readStream >> latin1Name;

    // connection type
    ConnectionType connectionType = InvalidConnection;
    quint8 connTypeVal = InvalidConnection;
    readStream >> connTypeVal;
    connectionType = static_cast <ConnectionType>(connTypeVal);

    // instance id
    quint32 instanceId = 0;
    readStream >> instanceId;

    // checksum
    quint16 msgChecksum = 0;
    readStream >> msgChecksum;

    const quint16 actualChecksum = qChecksum( msgBytes.constData(), static_cast<quint32>( msgBytes.length() - sizeof( quint16 ) ) );

    bool isValid = readStream.status() == QDataStream::Ok && QLatin1String(latin1Name) == mBlockServerName && msgChecksum == actualChecksum;
    if(!isValid) {
        sock->close();
        return;
    }

    info.instanceId = instanceId;
    info.stage = StageConnected;

    if(connectionType == NewInstance || ( connectionType == SecondaryInstance && mOptions & SingleApp::Mode::SecondaryNotification)) {
        Q_EMIT instanceStarted();
    }

    if (sock->bytesAvailable() > 0) {
        Q_EMIT this->slotDataAvailable(sock, instanceId);
    }
}
