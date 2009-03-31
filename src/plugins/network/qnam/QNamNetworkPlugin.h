//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//
#ifndef MARBLE_QNAM_NETWORK_PLUGIN_H
#define MARBLE_QNAM_NETWORK_PLUGIN_H

class QNetworkAccessManager;

#include <QtCore/QMap>
#include <QtCore/QString>

#include "NetworkPlugin.h"

namespace Marble
{

class QNamNetworkPlugin: public NetworkPlugin
{
    Q_OBJECT

 public:
    virtual ~QNamNetworkPlugin();

    // PluginInterface abstract methods
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;

    // NetworkPluginInterface abstract methods
    virtual HttpJob * createJob( const QUrl &source, const QString &destination,
                                 const QString &id );

    // NetworkPlugin abstract methods
    virtual NetworkPlugin * createNetworkPlugin() const;

 private:
    QNetworkAccessManager * findOrCreateNetworkAccessManager( const QString & hostname );

    QMap<QString, QNetworkAccessManager *> m_networkAccessManagers;
};

}

#endif
