//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_VELOCITYPROVIDER_P_H
#define GEOCUTE_VELOCITYPROVIDER_P_H

#include "VelocityProvider.h"

#include <QtCore/QString>
#include <QtDBus/QDBusMessage>

#include "SimpleDBusInterface.h"



static const QString interfaceName = "org.freedesktop.Geoclue.Velocity";

class GeoCute::VelocityProvider::Private
{
    public:
        Private(VelocityProvider& parent, const QString& service,
            const QString& path);
        void velocityChangedCall(QDBusMessage message);
        
        SimpleDBusInterface interface;
        Velocity currentVelocity;
        
    private:
        VelocityProvider& parent;
};



#endif
