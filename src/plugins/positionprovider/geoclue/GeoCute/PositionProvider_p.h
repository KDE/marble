//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_POSITIONPROVIDER_P_H
#define GEOCUTE_POSITIONPROVIDER_P_H

#include "PositionProvider.h"

#include <QtCore/QString>
#include <QtDBus/QDBusMessage>

#include "SimpleDBusInterface.h"



static const QString interfaceName = "org.freedesktop.Geoclue.Position";

class GeoCute::PositionProvider::Private
{
    public:
        Private(PositionProvider& parent, const QString& service,
            const QString& path);
        void positionChangedCall(QDBusMessage message);
        
        SimpleDBusInterface interface;
        Position currentPosition;
        
    private:
        PositionProvider& parent;
};



#endif
