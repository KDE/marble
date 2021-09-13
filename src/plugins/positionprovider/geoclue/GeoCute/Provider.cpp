// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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



#include "moc_Provider.cpp"
