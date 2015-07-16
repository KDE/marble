//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "PositionProvider_p.h"

#include <QList>
#include <QVariant>
#include <QDBusReply>

#include "MasterClient.h"



using namespace GeoCute;

PositionProvider::Private::Private(PositionProvider& parent,
    const QString& service, const QString& path)
    : interface(service, path, interfaceName), parent(parent)
{
    // Get an initial position
    interface.callWithCallback("GetPosition", QList<QVariant>(), &parent,
        SLOT(positionChangedCall(QDBusMessage)), 0);
    // Stay informed about future position changes
    interface.connect("PositionChanged", &parent,
        SLOT(positionChangedCall(QDBusMessage)));
}

void PositionProvider::Private::positionChangedCall(QDBusMessage message)
{
    // FIXME: Check the number of arguments
    // FIXME: Make sure all result members are actually filled
    Position result;
    result.fields
        = static_cast<PositionFields>(message.arguments()[0].toInt());
    result.timestamp.setTime_t(message.arguments()[1].toInt());
    result.latitude = message.arguments()[2].toDouble();
    result.longitude = message.arguments()[3].toDouble();
    result.altitude = message.arguments()[4].toDouble();
    currentPosition = result;
    if (result.fields != PositionFieldNone)
        emit parent.positionChanged(result);
}



PositionProvider::PositionProvider(const QString& service, const QString& path,
    QObject* parent)
    : Provider(service, path, parent), d(new Private(*this, service, path))
{
}

PositionProvider::~PositionProvider()
{
    delete d;
}

PositionProvider* PositionProvider::detailed()
{
    // FIXME: The following code has been replaced for the moment
    // because it leads to a crash in geoclue-master
    MasterClient mc;
    mc.setRequirements(GeoCute::AccuracyLevelNone, 0,
        GeoCute::SignallingRequired, GeoCute::ResourceAll);
    return mc.positionProvider();
    // Return a specific GeoClue provider
    // return new PositionProvider("example.provider.hostip", "/");
}

Position PositionProvider::position() const
{
    return d->currentPosition;
}



#include "moc_PositionProvider.cpp"
