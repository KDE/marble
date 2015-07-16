//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "VelocityProvider_p.h"

#include <QDBusReply>



using namespace GeoCute;

VelocityProvider::Private::Private(VelocityProvider& parent,
    const QString& service, const QString& path)
    : interface(service, path, interfaceName), parent(parent)
{
    // Get an initial velocity
    interface.callWithCallback("GetVelocity", QList<QVariant>(), &parent,
        SLOT(velocityChangedCall(QDBusMessage)), 0);
    // Stay informed about future velocity changes
    interface.connect("VelocityChanged", &parent,
        SLOT(velocityChangedCall(QDBusMessage)));
}

void VelocityProvider::Private::velocityChangedCall(QDBusMessage message)
{
    Velocity newVelocity;
    newVelocity.fields
        = static_cast<VelocityFields>(message.arguments()[0].toInt());
    newVelocity.speed = message.arguments()[2].toDouble();
    newVelocity.direction = message.arguments()[3].toDouble();
    newVelocity.climb = message.arguments()[4].toDouble();
    
    currentVelocity = newVelocity;
    emit parent.velocityChanged(newVelocity);
}



VelocityProvider::VelocityProvider(const QString& service, const QString& path,
    QObject* parent)
    : Provider(service, path, parent),
      d(new Private(*this, service, path))
{
}

VelocityProvider::~VelocityProvider()
{
    delete d;
}

Velocity VelocityProvider::velocity()
{
    return d->currentVelocity;
}



#include "moc_VelocityProvider.cpp"
