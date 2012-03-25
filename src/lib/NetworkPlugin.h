//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Pino Toscano <pino@kde.org>
// Copyright 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_NETWORKPLUGIN_H
#define MARBLE_NETWORKPLUGIN_H

#include "NetworkPluginInterface.h"
#include "marble_export.h"

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

    /**
     * Create a new Network Plugin and return it.
     * Has to be defined in concrete network plugin classes.
     */
    virtual NetworkPlugin * newInstance() const = 0;

    virtual void initialize() = 0;

    virtual bool isInitialized() const = 0;

 protected:
    NetworkPlugin();

 private:
    Q_DISABLE_COPY( NetworkPlugin )
    NetworkPluginPrivate  * const d;
};

}

#endif
