#ifndef GEOCUTE_PROVIDER_P_H
#define GEOCUTE_PROVIDER_P_H

#include "Provider.h"

#include <QtCore/QString>

#include "SimpleDBusInterface.h"



static const QString interfaceName = "org.freedesktop.Geoclue";

class GeoCute::Provider::Private {
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
