//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef DECLARATIVE_MARBLE_WIDGET_H
#define DECLARATIVE_MARBLE_WIDGET_H

#include "Tracking.h"
#include "Routing.h"
#include "Coordinate.h"

#include <QtGui/QGraphicsProxyWidget>
#include <QtCore/QPoint>

namespace Marble
{
// Forward declaration
class MarbleWidget;

namespace Declarative
{

/**
  * Wraps a Marble::MarbleWidget, providing access to important properties and methods
  *
  * @todo FIXME: Currently stuffed in a QGraphicsProxyWidget as otherwise it is only
  * displayed in QML when it is the only widget. For performance reasons it would be
  * nice to avoid this.
  */
class MarbleWidget : public QGraphicsProxyWidget
{
    Q_OBJECT

    Q_PROPERTY( Marble::Declarative::Coordinate* center READ center WRITE setCenter NOTIFY centerChanged )

    Q_PROPERTY( QString mapThemeId READ mapThemeId WRITE setMapThemeId )
    Q_PROPERTY( QString projection READ projection WRITE setProjection )
    Q_PROPERTY( bool inputEnabled READ inputEnabled WRITE setInputEnabled )
    Q_PROPERTY( bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged )
    Q_PROPERTY( QStringList activeFloatItems READ activeFloatItems WRITE setActiveFloatItems )
    Q_PROPERTY( Marble::Declarative::Tracking* tracking READ tracking NOTIFY trackingChanged )
    Q_PROPERTY( Marble::Declarative::Routing* routing READ routing NOTIFY routingChanged )

public:
    /** Constructor */
    explicit MarbleWidget( QGraphicsItem *parent = 0, Qt::WindowFlags flags = 0 );

    bool workOffline() const;

    void setWorkOffline( bool workOffline );

Q_SIGNALS:
    /** Forwarded from MarbleWidget. Zoom value and/or center position have changed */
    void visibleLatLonAltBoxChanged();

    void trackingChanged();

    void routingChanged();

    void centerChanged();

    void workOfflineChanged();

public Q_SLOTS:
    Marble::Declarative::Coordinate* center();

    void setCenter( Marble::Declarative::Coordinate* center );

    /** Returns a list of active (!) float items */
    QStringList activeFloatItems() const;

    /** Activates all of the given float items and deactivates any others */
    void setActiveFloatItems( const QStringList &items );

    /** Returns true if the map accepts keyboard/mouse input */
    bool inputEnabled() const;

    /** Toggle keyboard/mouse input */
    void setInputEnabled( bool enabled );

    /**
      * Returns the currently active map theme id, if any, in the
      * form of e.g. "earth/openstreetmap/openstreetmap.dgml"
      */
    QString mapThemeId() const;

    /**
      * Change the currently active map theme id. Ignored if the given
      * map theme id is invalid (not installed).
      * @see DeclarativeMapThemeManager
      */
    void setMapThemeId( const QString &mapThemeId );

    /**
      * Returns the active projection which can be either "Equirectangular",
      * "Mercator" or "Spherical"
      */
    QString projection( ) const;

    /**
      * Change the active projection. Accepted values are "Equirectangular",
      * "Mercator" and "Spherical"
      */
    void setProjection( const QString &projection );

    /** Zoom in by a fixed amount */
    void zoomIn();

    /** Zoom out by a fixed amount */
    void zoomOut();

    /**
      * Returns the screen position of the given coordinate,
      * an invalid point if the coordinate is not visible
      */
    QPoint pixel( qreal longitude, qreal latitude ) const;

    /**
      * Returns the coordinate at the given screen position
      */
    Marble::Declarative::Coordinate *coordinate( int x, int y );

    Marble::Declarative::Tracking* tracking();

    Marble::Declarative::Routing* routing();

private Q_SLOTS:
    void updateCenterPosition();

private:
    /** Wrapped MarbleWidget */
    Marble::MarbleWidget* m_marbleWidget;

    bool m_inputEnabled;

    Marble::Declarative::Tracking* m_tracking;

    Marble::Declarative::Routing* m_routing;

    Marble::Declarative::Coordinate m_center;
};

} // namespace Declarative
} // namespace Marble

#endif
