### QDbus

#### qdbus 查看

`要发送信号可以使用dbus-send,要查看Dbus上的消息流可以使用dbus-monitor`

#### 类型

1. 原生类型

| Qt类型 | D-Bus类型 |
| -- | -- |
|uchar            |           BYTE           |
|bool             |            BOOLEAN       |
|short            |            INT16         |
|ushort           |           UINT16         |
|int              |              INT32       |
|uint             |             UINT32       |
|qlonglong        |        INT64             |
|qulonglong       |       UINT64             |
|double           |          DOUBLE          |
|QString          |         STRING           |
|QDBusVariant     |    VARIANT               |
|QDBusObjectPath  |OBJECT_PATH               |
|QDBusSignature   |  SIGNATURE               |

2. 复合类型

> 三种复合类型：ARRAY、STRUCT和 maps/dictionaries

ARRAY零个或多个相同元素的集合，STRUCT是由不同类型的固定数量的元素组成的集合，Maps or dictionaries是元素对的数组，一个map中可以有零个或多个元素。

3. 扩展类型系统

为了在QtDBus模块使用自定义类型，自定义类性必须使用Q_DECLARE_METATYPE()声明为Qt元类型，使用qDBusRegisterMetaType()函数注册。流操作符会被注册系统自动找到。
QtDBus模块为Qt容器类使用数组和map提供了模板特化，例如QMap和QList，不必实现流操作符函数。对于其它的类型，流操作符必须显示实现。

#### QDbus 常用类

1. QDBusMessage

    QDBusMessage类表示D-Bus总线发送或接收的一个消息。
    QDBusMessage对象代表总线上四种消息类型中的一种，四种消息类型如下：
    A、Method calls
    B、Method return values
    C、Signal emissions
    D、Error codes
    可以使用静态函数createError()、createMethodCall()、createSignal()创建消息。使用QDBusConnection::send() 函数发送消息。

2. QDBusConnection

    QDBusConnection代表到D-Bus总线的一个连接，是一个D-Bus会话的起始点。通过QDBusConnection连接对象，可以访问远程对象、接口，连接远程信号到本地槽函数，注册对象等。
    D-Bus连接通过connectToBus()函数创建，connectToBus()函数会创建一个到总线服务端的连接，完成初始化工作，并关联一个连接名到连接。
    使用disconnectFromBus()函数会断开连接。一旦断开连接后，调用connectToBus()函数将不会重建连接，必须创建新的QDBusConnection实例。
    作为两种最常用总线类型的辅助，sessionBus()和systemBus()函数分别创建到会话在总线和系统总线的连接并返回，会在初次使用时打开，在QCoreApplication析构函数调用时断开。
    D-Bus支持点对点通信，不必使用总线服务。两个应用程序可以直接交流和交换消息。可以通过传递一个地址到connectToBus()函数实现。
    QDBusConnection connectToBus(BusType type, const QString & name)
    打开一个type类型的连接，并关联name连接名，返回关联本连接的QDBusConnection对象。
    QDBusConnection connectToBus(const QString & address, const QString & name)
    打开一个地址为address的私有总线，并关联name连接名，返回关联本连接的QDBusConnection对象。
    QDBusConnection connectToPeer(const QString & address, const QString & name)
    打开一个点对点的连接到address地址，并关联name连接名，返回关联本连接的QDBusConnection对象。
    void disconnectFromBus(const QString & name)
    关闭名为name的总线连接
    void disconnectFromPeer(const QString & name)
    关闭名为name的对等连接

3. QDBusInterface

    QByteArray localMachineId()
    返回一个D-Bus总线系统知道的本机ID
    QDBusConnection sender()
    返回发送信号的连接
    QDBusConnection sessionBus()
    返回一个打开到session总线的QDBusConnection对象
    QDBusConnection systemBus()
    返回一个打开到system总线的QDBusConnection对象
    QDBusPendingCall asyncCall(const QDBusMessage & message, int timeout = -1)const
    发送message消息到连接，并立即返回。本函数只支持method调用。返回一个用于追踪应答的QDBusPendingCall对象。
    QDBusMessage call(const QDBusMessage & message, QDBus::CallMode mode = QDBus::Block, int timeout = -1 ) const
    通过本连接发送消息message，并且阻塞，等待应答。
    bool registerObject(const QString & path, QObject * object, RegisterOptions options = ExportAdaptors)
    注册object对象到路径path，options选项指定由多少对象会被暴露到D-Bus总线，如果注册成功，返回true。
    bool registerService(const QString & serviceName)
    试图在D-Bus总线上注册serviceName服务，如果注册成功，返回true；如果名字已经在其它应用被注册，则注册失败。

4. QDBusReply

    QDBusInterface是远程对象接口的代理。
    QDBusInterface是一种通用的访问器类，用于调用远程对象，连接到远程对象导出的信号，获取/设置远程属性的值。当没有生成表示远程接口的生成代码时时，QDBusInterface类对远程对象的动态访问非常有用。
    调用通常是通过使用call（）函数来实现，call函数构造消息，通过总线发送消息，等待应答并解码应答。信号使用QObject::connect()函数进行连接。最终，使用QObject::property()和QObject::setProperty()函数对属性进行访问。

    QDBusReply类用于存储对远程对象的方法调用的应答。
    一个QDBusReply对象是方法调用的应答QDBusMessage对象的一个子集。QDBusReply对象只包含第一个输出参数或错误代码，并由QDBusInterface派生类使用，以允许将错误代码返回为函数的返回参数。

```c
QDBusReply<QString> reply = interface->call("RemoteMethod");
 if (reply.isValid())
     // use the returned value
     useValue(reply.value());
 else
     // call failed. Show an error condition.
     showError(reply.error());
```
对于没有输出参数或返回值的远程调用，使用isValid()函数测试应答是否成功。
5. QDBusAbstractAdaptor

    QDBusAbstractAdaptor类使用D-Bus Adaptor基类。
    QDBusAbstractAdaptor类是用于使用D-Bus向外部提供接口的所有对象的起点。可以通过将一个或多个派生自QDBusAbstractAdaptor的类附加到一个普通QObject对象上，使用QDBusConnection::registerObject注册QObject对象可以实现。QDBusAbstractAdaptor是一个轻量级封装，主要用于中继调用实际对象及其信号。
    每个QDBusAbstractAdaptor派生类都应该使用类定义中的Q_CLASSINFO宏来定义D-Bus接口。注意，这种方式只有一个接口可以暴露。
    QDBusAbstractAdaptor使用了信号、槽、属性的标准QObject机制来决定哪些信号、槽、属性被暴露到总线。任何QDBusAbstractAdaptor派生类发送的信号通过任何D-Bus连接自动中继到注册的对象上。
    QDBusAbstractAdaptor派生类对象必须使用new创建在堆上，不必由用户删除。

6. QDBusAbstractInterface

    QDBusAbstractInterface是QtDBus模块中允许访问远程接口的所有D-Bus接口的基类。
    自动生成的代码类也继承自QDBusAbstractInterface，此描述的所有方法在生成的代码中也有效。除了此处的描述，生成代码类为远程方法提供了成员函数，允许在编译时检查正确参数和返回值，以及匹配的属性类型和匹配的信号参数。

7. QDBusArgument

    QDBusArgument类用于整理和分发D-Bus参数。QDBusArgument用于通过D-Bus发送参数到远程应用，并接收返回。
    QDBusArgument是QtDBus类型系统的核心类，QtDBus类型系统用于解析和原生类型。复合类型可以通过在数组、词典或结构中使用一个或多个原生类型创建。

8. QDBusConnectionInterface

    QDBusConnectionInterface类提供了对D-Bus总线服务的访问。
    D-Bus总线服务端中提供了一个特殊的接口org.freedesktop.DBus，允许客户端运行访问总线的某些属性，例如当前连接的客户端列表，QDBusConnectionInterface类提供对org.freedesktop.DBus接口的访问。
    本类中最常用的是使用registerService()和unregisterService()在总线上注册和注销服务名。
    QDBusConnectionInterface类定义四个信号，在总线上有服务状态变化时发送。

    ```c
    void callWithCallbackFailed(const QDBusError & error, const QDBusMessage & call)
    void serviceOwnerChanged(const QString & name, const QString & oldOwner, const QString & newOwner)
    void serviceRegistered(const QString & serviceName)
    void serviceUnregistered(const QString & serviceName)
    ```

9. QDBusVariant

    QDBusVariant类使程序员能够识别由D-Bus类型系统提供的Variant类型。
    当D-Bus函数返回一个D-Bus变体类型时，可以使用如下方法获取：
    ```c
    // call a D-Bus function that returns a D-Bus variant
    QVariant v = callMyDBusFunction();
    // retrieve the D-Bus variant
    QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(v);
    // retrieve the actual value stored in the D-Bus variant
    QVariant result = dbusVariant.variant();
    ```

#### qdbuscpp2xml

    qdbuscpp2xml会解析QObject派生类的C++头文件或是源文件，生成D-Bus的内省xml文件。
    qdbuscpp2xml 会区分函数的输入输出，如果参数声明为const则会是输入，否则可能会被当作输出。

    ```c
    qdbuscpp2xml使用语法如下：
    qdbuscpp2xml [options...] [files...]
    Options参数如下：
        -p|-s|-m：只解析脚本化的属性、信号、方法（槽函数）
        -P|-S|-M：解析所有的属性、信号、方法（槽函数）
        -a：输出所有的脚本化内容，等价于-psm
        -A：输出所有的内容，等价于-PSM
        -o filename：输出内容到filename文件
    ```

    解析所有的方法输出到com.scorpio.test.xml文件命令如下：
    `qdbuscpp2xml -M test.h -o com.scorpio.test.xml`

#### qdbusxml2cpp

    qdbusxml2cpp根据输入文件中定义的接口，生成C++实现代码。
    qdbusxml2cpp可以辅助自动生成继承于QDBusAbstractAdaptor和QDBusAbstractInterface两个类的实现代码，用于进程通信服务端和客户端，简化了开发者的代码设计。

    ```
    qdbusxml2cpp使用语法如下：
    qdbusxml2cpp [options...] [xml-or-xml-file] [interfaces...]
    Options参数如下：
        -a filename：输出Adaptor代码到filename
        -c classname：使用classname作为生成类的类名
        -i filename：增加#include到输出
        -l classname：当生成Adaptor代码时，使用classname作为父类
        -m：在cpp文件中包含 #include "filename.moc"语句
        -N：不使用名称空间
        -p filename：生成Proxy代码到filename文件
    ```

    解析com.scorpio.test.xml文件，生成Adaptor类ValueAdaptor，文件名称为valueAdaptor.h、valueAdaptor.cpp命令行如下：
    qdbusxml2cpp com.scorpio.test.xml -i test.h -a valueAdaptor

    解析com.scorpio.test.xml文件，生成Proxy类ComScorpioTestValueInterface，文件名称为testInterface.h、testInterface.cpp命令行如下：
    qdbusxml2cpp com.scorpio.test.xml -p testInterface

#### QtDBus 编程

    1. 创建服务并注册对象
        .h
        ```c++
        #ifndef TEST_H
        #define TEST_H
        #include <QObject>

        class test: public QObject
        {
            Q_OBJECT
            //定义Interface名称为com.scorpio.test.value
            Q_CLASSINFO("D-Bus Interface", "com.scorpio.test.value")
        public:
            test(int value);

        public slots:
            int maxValue();
            int minValue();
            int value();
        private:
            int m_value;
        };
        ```
        .cpp
        ```
        #include "test.h"

        test::test(int value)
        {
            m_value = value;
        }

        int test::maxValue()
        {
            return 100;
        }
        int test::minValue()
        {
            return 0;
        }
        int test::value()
        {
            return m_value;
        }
        ```
        main.cpp
        ```cpp
        #include <QCoreApplication>
        #include <QDBusConnection>
        #include <QDebug>
        #include <QDBusError>
        #include "test.h"

        int main(int argc, char *argv[])
        {
            QCoreApplication a(argc, argv);

            //建立到session bus的连接
            QDBusConnection connection = QDBusConnection::sessionBus();
            //在session bus上注册名为com.scorpio.test的服务
            if(!connection.registerService("com.scorpio.test"))
            {
                qDebug() << "error:" << connection.lastError().message();
                exit(-1);
            }
            test object(60);
            //注册名为/test/objects的对象，把类Object所有槽函数导出为object的method
            connection.registerObject("/test/objects", &object,QDBusConnection::ExportAllSlots);

            return a.exec();
        }
        ```

    2. 通过 QDBusMessage 访问Service

        ```cpp
        #include <QCoreApplication>
        #include <QDBusMessage>
        #include <QDBusConnection>
        #include <QDebug>

        int main(int argc, char *argv[])
        {
            QCoreApplication a(argc, argv);
            //构造一个method_call消息，服务名称为：com.scorpio.test，对象路径为：/test/objects
            //接口名称为com.scorpio.test.value，method名称为value
            QDBusMessage message = QDBusMessage::createMethodCall("com.scorpio.test",
                                   "/test/objects",
                                   "com.scorpio.test.value",
                                   "value");
            //发送消息
            QDBusMessage response = QDBusConnection::sessionBus().call(message);
            //判断method是否被正确返回
            if (response.type() == QDBusMessage::ReplyMessage)
            {
                //从返回参数获取返回值
                int value = response.arguments().takeFirst().toInt();
                qDebug() << QString("value =  %1").arg(value);
            }
            else
            {
                qDebug() << "value method called failed!";
            }

            return a.exec();
        }
        ```

    3. 通过QDBusInterface 访问 Service

        ```cpp
        #include <QCoreApplication>
        #include <QDBusMessage>
        #include <QDBusConnection>
        #include <QDBusReply>
        #include <QDBusInterface>
        #include <QDebug>

        int main(int argc, char *argv[])
        {
            QCoreApplication a(argc, argv);
            // 创建QDBusInterface接口
            QDBusInterface interface("com.scorpio.test", "/test/objects",
                                     "com.scorpio.test.value",
                                     QDBusConnection::sessionBus());
            if (!interface.isValid())
            {
                qDebug() << qPrintable(QDBusConnection::sessionBus().lastError().message());
                exit(1);
            }
            //调用远程的value方法
            QDBusReply<int> reply = interface.call("value");
            if (reply.isValid())
            {
                int value = reply.value();
                qDebug() << QString("value =  %1").arg(value);
            }
            else
            {
                qDebug() << "value method called failed!";
            }

            return a.exec();
        }
        ```

    4. 从D-Bus XML自动生成Proxy类

    Proxy Object提供了一种更加直观的方式来访问Service，如同调用本地对象的方法一样。
    生成Proxy类的流程如下：
    A、使用工具qdbuscpp2xml从object.h生成XML文件；
    `qdbuscpp2xml -M test.h -o com.scorpio.test.xml`

    ```
    <!DOCTYPE node PUBLIC "-//freedesktop//DTD D-BUS Object Introspection 1.0//EN" "http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd">
    <node>
      <interface name="com.scorpio.test.value">
        <method name="maxValue">
          <arg type="i" direction="out"/>
        </method>
        <method name="minValue">
          <arg type="i" direction="out"/>
        </method>
        <method name="value">
          <arg type="i" direction="out"/>
        </method>
      </interface>
    </node>
    ```

    B、使用工具qdbusxml2cpp从XML文件生成继承自QDBusInterface的类
    qdbusxml2cpp com.scorpio.test.xml -p valueInterface
    生成两个文件：valueInterface.cpp和valueInterface.h
    valueInterface.h文件：

    ```c
    /*
     * This file was generated by qdbusxml2cpp version 0.7
     * Command line was: qdbusxml2cpp com.scorpio.test.xml -p testInterface
     *
     * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
     *
     * This is an auto-generated file.
     * Do not edit! All changes made to it will be lost.
     */

    #ifndef TESTINTERFACE_H_1526737677
    #define TESTINTERFACE_H_1526737677

    #include <QtCore/QObject>
    #include <QtCore/QByteArray>
    #include <QtCore/QList>
    #include <QtCore/QMap>
    #include <QtCore/QString>
    #include <QtCore/QStringList>
    #include <QtCore/QVariant>
    #include <QtDBus/QtDBus>

    /*
     * Proxy class for interface com.scorpio.test.value
     */
    class ComScorpioTestValueInterface: public QDBusAbstractInterface
    {
        Q_OBJECT
    public:
        static inline const char *staticInterfaceName()
        { return "com.scorpio.test.value"; }

    public:
        ComScorpioTestValueInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

        ~ComScorpioTestValueInterface();

    public Q_SLOTS: // METHODS
        inline QDBusPendingReply<int> maxValue()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("maxValue"), argumentList);
        }

        inline QDBusPendingReply<int> minValue()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("minValue"), argumentList);
        }

        inline QDBusPendingReply<int> value()
        {
            QList<QVariant> argumentList;
            return asyncCallWithArgumentList(QLatin1String("value"), argumentList);
        }

    Q_SIGNALS: // SIGNALS
    };

    namespace com {
      namespace scorpio {
        namespace test {
          typedef ::ComScorpioTestValueInterface value;
        }
      }
    }
    #endif
    ```

    ```cpp
    valueInterface.cpp文件：

    /*
     * This file was generated by qdbusxml2cpp version 0.7
     * Command line was: qdbusxml2cpp com.scorpio.test.xml -p testInterface
     *
     * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
     *
     * This is an auto-generated file.
     * This file may have been hand-edited. Look for HAND-EDIT comments
     * before re-generating it.
     */

    #include "testInterface.h"

    /*
     * Implementation of interface class ComScorpioTestValueInterface
     */

    ComScorpioTestValueInterface::ComScorpioTestValueInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
        : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
    {
    }

    ComScorpioTestValueInterface::~ComScorpioTestValueInterface()
    {
    }

    调用Proxy类访问Service如下：

    #include <QCoreApplication>
    #include <QDBusMessage>
    #include <QDBusConnection>
    #include <QDBusReply>
    #include <QDBusInterface>
    #include <QDebug>
    #include "testInterface.h"

    int main(int argc, char *argv[])
    {
        QCoreApplication a(argc, argv);
        // 初始化自动生成的Proxy类com::scorpio::test::value
        com::scorpio::test::value test("com.scorpio.test",
                                       "/test/objects",
                                       QDBusConnection::sessionBus());
        // 调用value方法
        QDBusPendingReply<int> reply = test.value();
        //qdbusxml2cpp生成的Proxy类是采用异步的方式来传递Message，
        //所以需要调用waitForFinished来等到Message执行完成
        reply.waitForFinished();
        if (reply.isValid())
        {
            int value = reply.value();
            qDebug() << QString("value =  %1").arg(value);
        }
        else
        {
            qDebug() << "value method called failed!";
        }

        return a.exec();
    }
    ```

    5、使用Adapter注册Object

    可以直接把test类注册为消息总线上的一个Object，但QT4不推荐。QT4推荐使用Adapter来注册Object。
    大多数情况下，可能只需要把自定义的类里的方法有选择的发布到消息总线上，使用Adapter可以很方便的实现选择性发布。
    生成Adapter类的流程如下：
    A、使用工具 qdbuscpp2xml从test.h生成XML文件
    qdbuscpp2xml -M test.h -o com.scorpio.test.xml
    B、编辑com.scorpio.test.xml，选择需要发布的method，不需要发布的删除。
    C、使用工具qdbusxml2cpp从XML文件生成继承自QDBusInterface的类
    qdbusxml2cpp com.scorpio.test.xml -i test.h -a valueAdaptor
    生成两个文件：valueAdaptor.cpp和valueAdaptor.h
    valueAdaptor.h文件：

    ```cpp
    /*
     * This file was generated by qdbusxml2cpp version 0.7
     * Command line was: qdbusxml2cpp com.scorpio.test.xml -i test.h -a valueAdaptor
     *
     * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
     *
     * This is an auto-generated file.
     * This file may have been hand-edited. Look for HAND-EDIT comments
     * before re-generating it.
     */

    #ifndef VALUEADAPTOR_H_1526742670
    #define VALUEADAPTOR_H_1526742670

    #include <QtCore/QObject>
    #include <QtDBus/QtDBus>
    #include "test.h"
    class QByteArray;
    template<class T> class QList;
    template<class Key, class Value> class QMap;
    class QString;
    class QStringList;
    class QVariant;

    /*
     * Adaptor class for interface com.scorpio.test.value
     */
    class ValueAdaptor: public QDBusAbstractAdaptor
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", "com.scorpio.test.value")
        Q_CLASSINFO("D-Bus Introspection", ""
    "  <interface name=\"com.scorpio.test.value\">\n"
    "    <method name=\"maxValue\">\n"
    "      <arg direction=\"out\" type=\"i\"/>\n"
    "    </method>\n"
    "    <method name=\"minValue\">\n"
    "      <arg direction=\"out\" type=\"i\"/>\n"
    "    </method>\n"
    "  </interface>\n"
            "")
    public:
        ValueAdaptor(QObject *parent);
        virtual ~ValueAdaptor();

    public: // PROPERTIES
    public Q_SLOTS: // METHODS
        int maxValue();
        int minValue();
    Q_SIGNALS: // SIGNALS
    };

    #endif
    ```

    valueAdaptor.cpp文件：

    ```cpp
    /*
     * This file was generated by qdbusxml2cpp version 0.7
     * Command line was: qdbusxml2cpp com.scorpio.test.xml -i test.h -a valueAdaptor
     *
     * qdbusxml2cpp is Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
     *
     * This is an auto-generated file.
     * Do not edit! All changes made to it will be lost.
     */

    #include "valueAdaptor.h"
    #include <QtCore/QMetaObject>
    #include <QtCore/QByteArray>
    #include <QtCore/QList>
    #include <QtCore/QMap>
    #include <QtCore/QString>
    #include <QtCore/QStringList>
    #include <QtCore/QVariant>

    /*
     * Implementation of adaptor class ValueAdaptor
     */

    ValueAdaptor::ValueAdaptor(QObject *parent)
        : QDBusAbstractAdaptor(parent)
    {
        // constructor
        setAutoRelaySignals(true);
    }

    ValueAdaptor::~ValueAdaptor()
    {
        // destructor
    }

    int ValueAdaptor::maxValue()
    {
        // handle method call com.scorpio.test.value.maxValue
        int out0;
        QMetaObject::invokeMethod(parent(), "maxValue", Q_RETURN_ARG(int, out0));
        return out0;
    }

    int ValueAdaptor::minValue()
    {
        // handle method call com.scorpio.test.value.minValue
        int out0;
        QMetaObject::invokeMethod(parent(), "minValue", Q_RETURN_ARG(int, out0));
        return out0;
    }
    ```

    调用Adaptor类注册Object对象如下：

    ```cpp
    #include <QCoreApplication>
    #include <QDBusConnection>
    #include <QDebug>
    #include <QDBusError>
    #include "test.h"
    #include "valueAdaptor.h"

    int main(int argc, char *argv[])
    {
        QCoreApplication a(argc, argv);
        QDBusConnection connection = QDBusConnection::sessionBus();
        test object(60);
        //ValueAdaptor是qdbusxml2cpp生成的Adaptor类
        ValueAdaptor valueAdaptor(&object);
        if (!connection.registerService("com.scorpio.test"))
        {
            qDebug() << connection.lastError().message();
            exit(1);
        }
        connection.registerObject("/test/objects", &object);
        return a.exec();
    }
    ```

    使用qdbusviewer查看发布的method。
    Qt高级——QtDBus快速入门

    6. 自动启动Service

    D-Bus系统提供了一种机制可以在访问某个service时，自动把应用程序运行起来。
    需要在/usr/share/dbus-1/services下面建立com.scorpio.test.service文件，文件的内容如下：

    ```
    [D-BUS Service]
    Name=com.scorpio.test
    Exec=/path/to/scorpio/test
    ```

    在访问test的method前，不必手动运行应用程序。
