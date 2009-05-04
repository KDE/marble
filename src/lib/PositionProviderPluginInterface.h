//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef MARBLE_POSITIONPROVIDERPLUGININTERFACE_H
#define MARBLE_POSITIONPROVIDERPLUGININTERFACE_H

#include <QtCore/QtPlugin>

#include "PluginInterface.h"
#include "geodata/data/GeoDataCoordinates.h"
#include "geodata/data/GeoDataAccuracy.h"



namespace Marble
{
  
enum PositionProviderStatusFlag {
    PositionProviderStatusUnavailable = 0,
    PositionProviderStatusAcquiring,
    PositionProviderStatusAvailable
};

Q_DECLARE_FLAGS(PositionProviderStatus, PositionProviderStatusFlag)

/**
 * @short The interface for position provider plugins.
 *
 */
class PositionProviderPluginInterface: public PluginInterface
{
  public:
    virtual ~PositionProviderPluginInterface();

    virtual PositionProviderStatus status() const = 0;
    virtual GeoDataCoordinates position() const = 0;
    virtual GeoDataAccuracy accuracy() const = 0;
};

}

Q_DECLARE_INTERFACE( Marble::PositionProviderPluginInterface, "org.kde.Marble.PositionProviderPluginInterface/1.00" )



#endif
