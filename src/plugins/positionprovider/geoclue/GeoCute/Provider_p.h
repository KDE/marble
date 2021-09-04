//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_PROVIDER_P_H
#define GEOCUTE_PROVIDER_P_H

#include "Provider.h"

#include <QString>

#include "SimpleDBusInterface.h"



static const QString interfaceName = "org.freedesktop.Geoclue";

class GeoCute::Provider::Private
{
    public:
        Private(Provider& parent, const QString& service,
            const QString& path);
        ~Private();
        void statusChangedCall(int status);
        
        Status currentStatus;
        SimpleDBusInterface interface;
        
    private:
        Provider& parent;
};



#endif
