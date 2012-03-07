//
// This file is part of the Marble Virtual Globe.
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
#include "GeoDataCoordinates.h"
#include "GeoDataAccuracy.h"



namespace Marble
{
  
enum PositionProviderStatus {
    PositionProviderStatusError = 0,
    PositionProviderStatusUnavailable,
    PositionProviderStatusAcquiring,
    PositionProviderStatusAvailable
};

/**
 * @short The interface for position provider plugins.
 *
 */
class MARBLE_EXPORT PositionProviderPluginInterface: public PluginInterface
{
  public:
    virtual ~PositionProviderPluginInterface();

    virtual PositionProviderStatus status() const = 0;
    virtual GeoDataCoordinates position() const = 0;
    virtual GeoDataAccuracy accuracy() const = 0;

    /**
     * Returns the speed of the gps device in meters per second
     */
    virtual qreal speed() const = 0;

    /**
     * Returns the direction in which the gps device is moving.
     * The direction is an angle (in degrees) clockwise from geographical north
     */
    virtual qreal direction() const = 0;

    /**
      * Returns an error string to be presented to the user
      * if the status is PositionProviderStatusError. For other
      * states, the result value is undefined.
      */
    virtual QString error() const;
};

}

Q_DECLARE_INTERFACE( Marble::PositionProviderPluginInterface, "org.kde.Marble.PositionProviderPluginInterface/1.02" )

Q_DECLARE_METATYPE( Marble::PositionProviderStatus )

#endif
