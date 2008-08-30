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

#include "MarbleNetworkPluginInterface.h"
#include "marble_export.h"

#include <QtCore/QtPlugin>

namespace Marble
{

class MarbleNetworkPluginPrivate;

/**
 * @short The abstract class that handles network operations.
 *
 */
class MARBLE_EXPORT MarbleNetworkPlugin : public QObject, public MarbleNetworkPluginInterface
{
    Q_OBJECT

 public:
    virtual ~MarbleNetworkPlugin();

 protected:
    MarbleNetworkPlugin();

 private:
    Q_DISABLE_COPY( MarbleNetworkPlugin )
    MarbleNetworkPluginPrivate  * const d;
};

}

#endif // MARBLE_NETWORK_PLUGIN_H
