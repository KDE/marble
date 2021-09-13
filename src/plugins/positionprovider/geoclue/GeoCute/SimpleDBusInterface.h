// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_SIMPLEDBUSINTERFACE_H
#define GEOCUTE_SIMPLEDBUSINTERFACE_H

#include <QList>
#include <QObject>
#include <QString>
#include <QVariant>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusPendingCall>



namespace GeoCute
{

class SimpleDBusInterface : public QObject
{
    Q_OBJECT

    public:
        SimpleDBusInterface(const QString& service, const QString& path,
            const QString& interface = QString(),
            const QDBusConnection& connection = QDBusConnection::sessionBus(),
            QObject* parent = 0);
        QDBusPendingCall asyncCall(const QString& method) const;
        QDBusMessage call(const QString& method,
            const QVariant& arg1 = QVariant(),
            const QVariant& arg2 = QVariant(),
            const QVariant& arg3 = QVariant(),
            const QVariant& arg4 = QVariant(),
            const QVariant& arg5 = QVariant(),
            const QVariant& arg6 = QVariant(),
            const QVariant& arg7 = QVariant(),
            const QVariant& arg8 = QVariant());
        void callWithCallback(const QString& message,
            const QList<QVariant>& args, QObject* receiver,
            const char* returnMethod, const char* errorMethod) const;
        void connect(const QString& name, QObject* receiver,
            const char* slot);
        QString interface() const;
        QString path() const;
        QString service() const;
            
    private:
        QDBusConnection p_connection;
        const QString p_interface;
        const QString p_path;
        const QString p_service;
};

}



#endif
