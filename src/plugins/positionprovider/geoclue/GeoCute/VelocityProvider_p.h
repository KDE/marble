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
