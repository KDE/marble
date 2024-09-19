// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_VELOCITYPROVIDER_P_H
#define GEOCUTE_VELOCITYPROVIDER_P_H

#include "VelocityProvider.h"

#include <QDBusMessage>
#include <QString>

#include "SimpleDBusInterface.h"

static const QString interfaceName = "org.freedesktop.Geoclue.Velocity";

class GeoCute::VelocityProvider::Private
{
public:
    Private(VelocityProvider &parent, const QString &service, const QString &path);
    void velocityChangedCall(QDBusMessage message);

    SimpleDBusInterface interface;
    Velocity currentVelocity;

private:
    VelocityProvider &parent;
};

#endif
