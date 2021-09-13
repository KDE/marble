// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_POSITIONPROVIDER_P_H
#define GEOCUTE_POSITIONPROVIDER_P_H

#include "PositionProvider.h"

#include <QString>
#include <QDBusMessage>

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
