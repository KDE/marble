#ifndef GEOCUTE_MASTERCLIENT_P_H
#define GEOCUTE_MASTERCLIENT_P_H

#include "MasterClient.h"

#include <QtCore/QString>

#include "SimpleDBusInterface.h"



static const QString serviceName = "org.freedesktop.Geoclue.Master";
static const QString interfaceName = "org.freedesktop.Geoclue.MasterClient";
static const QString masterInterfaceName = "org.freedesktop.Geoclue.Master";
static const QString masterPathName = "/org/freedesktop/Geoclue/Master";

static QString createClientPath();

class GeoCute::MasterClient::Private {
    public:
        Private();
        
        SimpleDBusInterface interface;
};



#endif
