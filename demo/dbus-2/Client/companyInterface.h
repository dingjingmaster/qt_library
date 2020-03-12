/*
 * This file was generated by qdbusxml2cpp version 0.7
 * Command line was: qdbusxml2cpp company.xml -p companyInterface
 *
 * qdbusxml2cpp is Copyright (C) 2015 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef COMPANYINTERFACE_H_1516854372
#define COMPANYINTERFACE_H_1516854372

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>

/*
 * Proxy class for interface company.interface
 */
class CompanyInterfaceInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "company.interface"; }

public:
    CompanyInterfaceInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = 0);

    ~CompanyInterfaceInterface();

public Q_SLOTS: // METHODS
    inline QDBusPendingReply<int> candidateAge(int age)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(age);
        return asyncCallWithArgumentList(QLatin1String("candidateAge"), argumentList);
    }

    inline QDBusPendingReply<double> candidateExpectSalary(double salary)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(salary);
        return asyncCallWithArgumentList(QLatin1String("candidateExpectSalary"), argumentList);
    }

    inline QDBusPendingReply<QString> candidateName(const QString &name)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(name);
        return asyncCallWithArgumentList(QLatin1String("candidateName"), argumentList);
    }

    inline QDBusPendingReply<QString> candidateProfession(const QString &profession)
    {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(profession);
        return asyncCallWithArgumentList(QLatin1String("candidateProfession"), argumentList);
    }

Q_SIGNALS: // SIGNALS
    void ageChangedSingal();
    void nameChangedSingal();
    void professionChangedSingal();
    void recruitInfo(const QString &name, const QString &result);
    void salaryChangedSingal();
};

namespace company {
  typedef ::CompanyInterfaceInterface interface;
}
#endif
