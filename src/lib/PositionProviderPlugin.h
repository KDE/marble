//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009 Eckhart Wörner <ewoerner@kde.org>
//

#ifndef MARBLE_POSITIONPROVIDERPLUGIN_H
#define MARBLE_POSITIONPROVIDERPLUGIN_H

#include "PositionProviderPluginInterface.h"
#include "marble_export.h"

#include <QtCore/QtPlugin>



namespace Marble
{

/**
 * @short The abstract class that provides position information.
 */
class MARBLE_EXPORT PositionProviderPlugin : public QObject, public PositionProviderPluginInterface
{
    Q_OBJECT

 public:
    virtual ~PositionProviderPlugin();
    /**
     * Create a new PositionProvider Plugin and return it.
     * Has to be defined in concrete position provider plugin classes.
     */
    virtual PositionProviderPlugin * newInstance() const = 0;

    /**
      * Returns the speed of the gps device in metres per second
      */
    virtual qreal speed() const;

    /**
      * Returns the direction in which the gps device is moving.
      * The direction is an angle(in degrees) clockwise from geographical north
      */
    virtual qreal direction() const;


 signals:
    void statusChanged( PositionProviderStatus status ) const;
    void positionChanged( GeoDataCoordinates position,
                          GeoDataAccuracy accuracy ) const;

 protected:
    PositionProviderPlugin();

 private:
    Q_DISABLE_COPY( PositionProviderPlugin )

};

}

#endif
