//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "SimpleDBusInterface.h"



using namespace GeoCute;

SimpleDBusInterface::SimpleDBusInterface(const QString& service,
    const QString& path, const QString& interface,
    const QDBusConnection& connection, QObject* parent)
    : QObject(parent), p_connection(connection), p_interface(interface),
      p_path(path), p_service(service)
{
}

QDBusPendingCall SimpleDBusInterface::asyncCall(const QString& method) const
{
    const QDBusMessage message
        = QDBusMessage::createMethodCall(p_service, p_path, p_interface, method);
    return p_connection.asyncCall(message);
}

QDBusMessage SimpleDBusInterface::call(const QString& method,
    const QVariant& arg1, const QVariant& arg2, const QVariant& arg3,
    const QVariant& arg4, const QVariant& arg5, const QVariant& arg6,
    const QVariant& arg7, const QVariant& arg8)
{
    QDBusMessage message = QDBusMessage::createMethodCall(p_service, p_path,
        p_interface, method);
    QList<QVariant> arguments;
    if (arg1.isValid())
        arguments.append(arg1);
    if (arg2.isValid())
        arguments.append(arg1);
    if (arg3.isValid())
        arguments.append(arg1);
    if (arg4.isValid())
        arguments.append(arg1);
    if (arg5.isValid())
        arguments.append(arg1);
    if (arg6.isValid())
        arguments.append(arg1);
    if (arg7.isValid())
        arguments.append(arg1);
    if (arg8.isValid())
        arguments.append(arg1);
    message.setArguments(arguments);
    return p_connection.call(message);
}

void SimpleDBusInterface::callWithCallback(const QString& method,
    const QList<QVariant>& args, QObject* receiver, const char* returnMethod,
    const char* errorMethod) const
{
    QDBusMessage message
        = QDBusMessage::createMethodCall(p_service, p_path, p_interface, method);
    message.setArguments(args);
    p_connection.callWithCallback(message, receiver, returnMethod, errorMethod);
}

void SimpleDBusInterface::connect(const QString& name, QObject* receiver,
    const char* slot)
{
    p_connection.connect(p_service, p_path, p_interface, name, receiver, slot);
}

QString SimpleDBusInterface::interface() const
{
    return p_interface;
}

QString SimpleDBusInterface::path() const
{
    return p_path;
}

QString SimpleDBusInterface::service() const
{
    return p_service;
}



#include "moc_SimpleDBusInterface.cpp"
