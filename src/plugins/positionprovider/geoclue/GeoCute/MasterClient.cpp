//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "MasterClient_p.h"

#include <QString>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDBusObjectPath>

#include "PositionProvider.h"



using namespace GeoCute;

static QString createClientPath()
{
    SimpleDBusInterface masterInterface(serviceName, masterPathName,
        masterInterfaceName);
    QDBusReply<QDBusObjectPath> reply = masterInterface.call("Create");
    if (reply.isValid())
        return reply.value().path();
    else
	return QString();
}

MasterClient::Private::Private()
    : interface(serviceName, createClientPath(), interfaceName)
{
}

MasterClient::MasterClient(QObject* parent)
    : QObject(parent), d(new Private)
{
}

MasterClient::~MasterClient()
{
    delete d;
}

void MasterClient::setRequirements(AccuracyLevel accuracy, int min_time,
    SignallingFlags signalling, ResourceFlags resources)
{
    if (!d->interface.path().isEmpty()) {
        d->interface.call("SetRequirements", accuracy, min_time,
        signalling == SignallingRequired, resources);
    }
}

PositionProvider* MasterClient::positionProvider()
{
    if (!d->interface.path().isEmpty()) {
        d->interface.call("PositionStart");
        return new PositionProvider(d->interface.service(), d->interface.path());
    } else
        return 0;
}



#include "moc_MasterClient.cpp"
