// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Eckhart Wörner <ewoerner@kde.org>
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
