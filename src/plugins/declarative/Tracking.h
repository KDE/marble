//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_DECLARATIVE_TRACKING_H
#define MARBLE_DECLARATIVE_TRACKING_H

#include "PositionSource.h"

#include <QtCore/QObject>
#include <QtDeclarative/QtDeclarative>

class MarbleWidget;

namespace Marble {
class AutoNavigation;
}

class Tracking : public QObject
{
    Q_OBJECT
    Q_ENUMS( PositionMarkerType )

    Q_PROPERTY( bool showTrack READ showTrack WRITE setShowTrack NOTIFY showTrackChanged )
    Q_PROPERTY( bool autoCenter READ autoCenter WRITE setAutoCenter NOTIFY autoCenterChanged )
    Q_PROPERTY( bool autoZoom READ autoZoom WRITE setAutoZoom NOTIFY autoZoomChanged )
    Q_PROPERTY( PositionSource* positionSource READ positionSource WRITE setPositionSource NOTIFY positionSourceChanged )
    Q_PROPERTY( QObject* positionMarker READ positionMarker WRITE setPositionMarker NOTIFY positionMarkerChanged )
    Q_PROPERTY( bool hasLastKnownPosition READ hasLastKnownPosition NOTIFY hasLastKnownPositionChanged )
    Q_PROPERTY( Coordinate* lastKnownPosition READ lastKnownPosition WRITE setLastKnownPosition NOTIFY lastKnownPositionChanged )
    Q_PROPERTY( PositionMarkerType positionMarkerType READ positionMarkerType WRITE setPositionMarkerType NOTIFY positionMarkerTypeChanged )
    Q_PROPERTY( double distance READ distance NOTIFY distanceChanged )

public:
    enum PositionMarkerType {
        None,
        Circle,
        Arrow
    };

    explicit Tracking( QObject* parent = 0 );

    bool showTrack() const;

    void setShowTrack( bool show );

    PositionSource* positionSource();

    void setPositionSource( PositionSource* source );

    QObject* positionMarker();

    void setPositionMarker( QObject* marker );

    void setMarbleWidget( MarbleWidget* widget );

    bool hasLastKnownPosition() const;

    Coordinate *lastKnownPosition();

    void setLastKnownPosition( Coordinate* lastKnownPosition );

    bool autoCenter() const;

    void setAutoCenter( bool enabled );

    bool autoZoom() const;

    void setAutoZoom( bool enabled );

    PositionMarkerType positionMarkerType() const;

    void setPositionMarkerType( PositionMarkerType type );

    double distance() const;

public Q_SLOTS:
    void saveTrack( const QString &fileName );

    void openTrack( const QString &fileName );

    void clearTrack();

Q_SIGNALS:
    void showTrackChanged();

    void positionSourceChanged();

    void positionMarkerChanged();

    void hasLastKnownPositionChanged();

    void lastKnownPositionChanged();

    void autoCenterChanged();

    void autoZoomChanged();

    void positionMarkerTypeChanged();

    void distanceChanged();

private Q_SLOTS:
    void updatePositionMarker();

    void updateLastKnownPosition();

    void setHasLastKnownPosition();

private:
    void setShowPositionMarkerPlugin( bool visible );

    bool m_showTrack;

    PositionSource* m_positionSource;

    QObject* m_positionMarker;

    MarbleWidget* m_marbleWidget;

    bool m_hasLastKnownPosition;

    Coordinate m_lastKnownPosition;

    Marble::AutoNavigation* m_autoNavigation;

    PositionMarkerType m_positionMarkerType;
};


#endif
