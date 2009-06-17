#include "MasterClient_p.h"

#include <QtCore/QString>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusObjectPath>

#include "PositionProvider.h"



using namespace GeoCute;

static QString createClientPath() {
    SimpleDBusInterface masterInterface(serviceName, masterPathName,
        masterInterfaceName);
    QDBusReply<QDBusObjectPath> reply = masterInterface.call("Create");
    return reply.value().path();
}

MasterClient::Private::Private()
    : interface(serviceName, createClientPath(), interfaceName) { }

MasterClient::MasterClient(QObject* parent)
    : QObject(parent), d(new Private) { }

MasterClient::~MasterClient() {
    delete d;
}

void MasterClient::setRequirements(AccuracyLevel accuracy, int min_time,
    SignallingFlags signalling, ResourceFlags resources)
{
    d->interface.call("SetRequirements", accuracy, min_time,
        signalling == SignallingRequired, resources);
}

PositionProvider* MasterClient::positionProvider() {
    d->interface.call("PositionStart");
    return new PositionProvider(d->interface.service(), d->interface.path());
}



#include "MasterClient.moc"
