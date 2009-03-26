//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
//


#ifndef MARBLE_NETWORK_PLUGIN_H
#define MARBLE_NETWORK_PLUGIN_H

#include "NetworkPluginInterface.h"
#include "marble_export.h"

#include <QtCore/QtPlugin>

namespace Marble
{

class NetworkPluginPrivate;

/**
 * @short The abstract class that handles network operations.
 *
 */
class MARBLE_EXPORT NetworkPlugin : public QObject, public NetworkPluginInterface
{
    Q_OBJECT

 public:
    virtual ~NetworkPlugin();

 protected:
    NetworkPlugin();

 private:
    Q_DISABLE_COPY( NetworkPlugin )
    NetworkPluginPrivate  * const d;
};

}

#endif // MARBLE_NETWORK_PLUGIN_H
