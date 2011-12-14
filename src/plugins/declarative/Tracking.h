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

namespace Marble
{

class AdjustNavigation;

namespace Declarative
{

class MarbleWidget;

class Tracking : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool showPosition READ showPosition WRITE setShowPosition NOTIFY showPositionChanged )
    Q_PROPERTY( bool showTrack READ showTrack WRITE setShowTrack NOTIFY showTrackChanged )
    Q_PROPERTY( bool autoCenter READ autoCenter WRITE setAutoCenter NOTIFY autoCenterChanged )
    Q_PROPERTY( bool autoZoom READ autoZoom WRITE setAutoZoom NOTIFY autoZoomChanged )
    Q_PROPERTY( Marble::Declarative::PositionSource* positionSource READ positionSource WRITE setPositionSource NOTIFY positionSourceChanged )
    Q_PROPERTY( QObject* positionMarker READ positionMarker WRITE setPositionMarker NOTIFY positionMarkerChanged )
    Q_PROPERTY( bool hasLastKnownPosition READ hasLastKnownPosition NOTIFY hasLastKnownPositionChanged )
    Q_PROPERTY( Marble::Declarative::Coordinate* lastKnownPosition READ lastKnownPosition WRITE setLastKnownPosition NOTIFY lastKnownPositionChanged )

public:
    explicit Tracking( QObject* parent = 0);

    bool showPosition() const;

    void setShowPosition( bool show );

    bool showTrack() const;

    void setShowTrack( bool show );

    Marble::Declarative::PositionSource* positionSource();

    void setPositionSource( Marble::Declarative::PositionSource* source );

    QObject* positionMarker();

    void setPositionMarker( QObject* marker );

    void setMarbleWidget( Marble::Declarative::MarbleWidget* widget );

    bool hasLastKnownPosition() const;

    Coordinate *lastKnownPosition();

    void setLastKnownPosition( Marble::Declarative::Coordinate* lastKnownPosition );

    bool autoCenter() const;

    void setAutoCenter( bool enabled );

    bool autoZoom() const;

    void setAutoZoom( bool enabled );

public Q_SLOTS:
    void saveTrack( const QString &fileName );

    void openTrack( const QString &fileName );

    void clearTrack();

Q_SIGNALS:
    void showPositionChanged();

    void showTrackChanged();

    void positionSourceChanged();

    void positionMarkerChanged();

    void hasLastKnownPositionChanged();

    void lastKnownPositionChanged();

    void autoCenterChanged();

    void autoZoomChanged();

private Q_SLOTS:
    void updatePositionMarker();

    void updateLastKnownPosition();

    void setHasLastKnownPosition();

private:
    void setShowPositionMarkerPlugin( bool visible );

    bool m_showPosition;

    bool m_showTrack;

    Marble::Declarative::PositionSource* m_positionSource;

    QObject* m_positionMarker;

    Marble::Declarative::MarbleWidget* m_marbleWidget;

    bool m_hasLastKnownPosition;

    Marble::Declarative::Coordinate m_lastKnownPosition;

    Marble::AdjustNavigation* m_autoNavigation;
};

}

}

#endif
