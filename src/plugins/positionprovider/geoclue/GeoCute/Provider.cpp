//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#include "Provider_p.h"



using namespace GeoCute;

Provider::Private::Private(Provider& parent, const QString& service,
    const QString& path)
    : currentStatus(StatusUnavailable),
      interface(service, path, interfaceName),
      parent(parent)
      
{
    // Make sure the provider stays available as long as needed
    interface.asyncCall("AddReference");
    // Get an initial status
    interface.callWithCallback("GetStatus", QList<QVariant>(), &parent,
        SLOT(statusChangedCall(int)), 0);
    // Stay informed about future status updates
    interface.connect("StatusChanged", &parent, SLOT(statusChangedCall(int)));
}

Provider::Private::~Private()
{
    interface.asyncCall("RemoveReference");
}

void Provider::Private::statusChangedCall(int status)
{
    const Status newStatus = static_cast<Status>(status);
    if (newStatus != currentStatus) {
        currentStatus = newStatus;
        // FIXME: Re-enable
        emit parent.statusChanged(newStatus);
    }
}



Provider::Provider(const QString& service, const QString& path,
    QObject* parent)
    : QObject(parent), d(new Private(*this, service, path))
{
}

Provider::~Provider()
{
    delete d;
}

Status Provider::status() const
{
    return d->currentStatus;
}



#include "Provider.moc"
