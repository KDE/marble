//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
//

#ifndef MARBLE_NETWORKPLUGININTERFACE_H
#define MARBLE_NETWORKPLUGININTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QString>

#include "PluginInterface.h"

class QUrl;

namespace Marble
{

class HttpJob;

/**
 * @short The interface for network operation plugins.
 *
 */
class NetworkPluginInterface: public PluginInterface
{
 public:
    virtual ~NetworkPluginInterface();

    /**
     * @brief Creates a new HTTP download job.
     * @return the new HTTP job
     */
    virtual HttpJob *createJob( const QUrl &source, const QString &destination, const QString &id ) = 0;
};

}

Q_DECLARE_INTERFACE( Marble::NetworkPluginInterface, "org.kde.Marble.NetworkPluginInterface/1.00" )

#endif
