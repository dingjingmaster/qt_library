#ifndef TESTKEY_H
#define TESTKEY_H

#include <QObject>

class TestKey : public QObject
{
    Q_OBJECT
public:
    explicit TestKey(QObject *parent = nullptr);

Q_SLOT
    void testKey(QString&);

Q_SIGNALS:

};

#endif // TESTKEY_H
