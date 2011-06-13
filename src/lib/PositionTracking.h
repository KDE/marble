//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#ifndef MARBLE_POSITIONTRACKING_H
#define MARBLE_POSITIONTRACKING_H

#include "marble_export.h"
#include "PositionProviderPluginInterface.h"

#include <QtCore/QObject>

namespace Marble
{

class GeoDataAccuracy;
class GeoDataDocument;
class GeoDataCoordinates;
class PositionProviderPlugin;
class PositionTrackingPrivate;
class MarbleModel;

class MARBLE_EXPORT PositionTracking : public QObject
{
    Q_OBJECT

    Q_PROPERTY( PositionProviderPlugin* positionProviderPlugin READ positionProviderPlugin WRITE setPositionProviderPlugin NOTIFY positionProviderPluginChanged )

public:
    explicit PositionTracking( MarbleModel* model );
    ~PositionTracking();

    /**
      * Change the position provider to use. You can provide 0 to disable
      * position tracking. Ownership of the provided plugin is taken.
      */
    void setPositionProviderPlugin( PositionProviderPlugin* plugin );

    /** @brief Returns the current position provider plugin, or 0 if none is in use */
    PositionProviderPlugin* positionProviderPlugin();

    /**
      * @brief gives the error message from the current position provider
      */
    QString error() const;

    /**
     * @brief provides speed of the gps device
     */
    qreal speed() const;

    /**
     * @brief provides direction of the gps device in degrees with geographical north
     */
    qreal direction() const;

    /** @brief Returns the estimated accuracy of the current position */
    GeoDataAccuracy accuracy() const;

    /**
     * @brief provides the visibility of the Position Tracking document
     */
    bool trackVisible() const;

    /** @brief Returns the current position, if any */
    GeoDataCoordinates currentLocation() const;

    /** @brief Returns the status of the current position provider plugin, if any */
    PositionProviderStatus status() const;

    /** @brief Returns true if there is no position in the track */
    bool isTrackEmpty() const;

public Q_SLOTS:
    /**
      * Toggles the visibility of the Position Tracking document
      */
    void setTrackVisible ( bool visible );

    /**
      * Saves the track document to file
      */
    bool saveTrack( QString& fileName );

    /**
      * Removes all track segments which were recorded
      */
    void clearTrack();

Q_SIGNALS:
    void  gpsLocation( GeoDataCoordinates, qreal );

    void statusChanged( PositionProviderStatus status );

    /**
     * @brief emits positionProviderPluginChanged(0) when provider is disabled
     */

    void positionProviderPluginChanged( PositionProviderPlugin *activePlugin );

 private:
    PositionTrackingPrivate *d;
};

}



#endif
