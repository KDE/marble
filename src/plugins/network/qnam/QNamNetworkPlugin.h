//
// This file is part of the Marble Virtual Globe.
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

#include "NetworkPlugin.h"

namespace Marble
{

class QNamNetworkPlugin: public NetworkPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::NetworkPluginInterface )

 public:
    QNamNetworkPlugin();
    virtual ~QNamNetworkPlugin();

    // PluginInterface abstract methods
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString version() const;
    virtual QString description() const;
    virtual QString copyrightYears() const;
    virtual QList<PluginAuthor> pluginAuthors() const;
    virtual QIcon icon() const;

    // NetworkPluginInterface abstract methods
    virtual HttpJob * createJob( const QUrl &source, const QString &destination,
                                 const QString &id );

    // NetworkPlugin abstract methods
    virtual NetworkPlugin * newInstance() const;

 private:
    QNetworkAccessManager * const m_networkAccessManager;
};

}

#endif
