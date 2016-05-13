//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Eckhart Wörner <ewoerner@kde.org>
//

#ifndef GEOCUTE_VELOCITYPROVIDER_H
#define GEOCUTE_VELOCITYPROVIDER_H

#include "Provider.h"
#include "Velocity.h"



class QString;

namespace GeoCute
{

class VelocityProvider : public Provider
{
    Q_OBJECT
    
    public:
        VelocityProvider(const QString& service, const QString& path,
            QObject* parent = 0);
        ~VelocityProvider();
        Velocity velocity();
        
    Q_SIGNALS:
        void velocityChanged(GeoCute::Velocity info);
        
    private:
        class Private;
        Private* const d;
        Q_PRIVATE_SLOT(d, void velocityChangedCall(QDBusMessage message))
};

}



#endif
