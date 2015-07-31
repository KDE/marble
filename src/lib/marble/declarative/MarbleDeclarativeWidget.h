//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef DECLARATIVE_MARBLE_WIDGET_H
#define DECLARATIVE_MARBLE_WIDGET_H

#include "Coordinate.h"
#include "Placemark.h"

#include "MapThemeManager.h"

#include <QList>
#include <QPoint>
#include <QStandardItemModel>
#include <QtQml/qqml.h>
#include "MarbleQuickItem.h"

using Marble::GeoDataCoordinates; // Ouch. For signal/slot connection across different namespaces

namespace Marble
{
// Forward declarations
class AbstractFloatItem;
class MarbleModel;
class RenderPlugin;
class ViewportParams;
}

class DeclarativeDataPlugin;
/**
  * Wraps a Marble::MarbleWidget, providing access to important properties and methods
  *
  * @note: This is not meant to be used anymore, please use MarbleQuickItem instead
  *
  */
class MarbleWidget : public Marble::MarbleQuickItem
{
    Q_OBJECT

    Q_PROPERTY( Coordinate* center READ center WRITE setCenter NOTIFY visibleLatLonAltBoxChanged )
    Q_PROPERTY( int radius READ radius WRITE setRadius NOTIFY radiusChanged )
    Q_PROPERTY( QString mapThemeId READ mapThemeId WRITE setMapThemeId NOTIFY mapThemeChanged )
    Q_PROPERTY( bool workOffline READ workOffline WRITE setWorkOffline NOTIFY workOfflineChanged )
    Q_PROPERTY( QStringList activeFloatItems READ activeFloatItems WRITE setActiveFloatItems )
    Q_PROPERTY( QStringList activeRenderPlugins READ activeRenderPlugins WRITE setActiveRenderPlugins )
    Q_PROPERTY( QStandardItemModel* mapThemeModel READ mapThemeModel NOTIFY mapThemeModelChanged )
    Q_PROPERTY( QList<QObject*> renderPlugins READ renderPlugins CONSTANT )
    Q_PROPERTY( QList<QObject*> floatItems READ floatItems CONSTANT )

    Q_PROPERTY( QQmlListProperty<DeclarativeDataPlugin> dataLayers READ dataLayers )
    Q_PROPERTY( QQmlListProperty<QObject> children READ childList )

    Q_CLASSINFO("DefaultProperty", "children")

public:
    /** Constructor */
    explicit MarbleWidget( QQuickItem *parent = 0 );

    ~MarbleWidget();

    const Marble::ViewportParams *viewport() const;

    bool workOffline() const;

    void setWorkOffline( bool workOffline );

    int radius() const;

    void setRadius( int radius );

    void setActiveRenderPlugins( const QStringList &items );

    QStringList activeRenderPlugins() const;

    QQmlListProperty<QObject> childList();

    QQmlListProperty<DeclarativeDataPlugin> dataLayers();

Q_SIGNALS:
    /** Forwarded from MarbleWidget. Zoom value and/or center position have changed */
    void visibleLatLonAltBoxChanged();

    void workOfflineChanged();

    void radiusChanged();

    void mouseClickGeoPosition( qreal longitude, qreal latitude );

    void mapThemeModelChanged();

    void mapThemeChanged();

    void placemarkSelected( Placemark* placemark );

public Q_SLOTS:
    Coordinate* center();

    void setCenter( Coordinate* center );

    QList<QObject*> renderPlugins() const;

    Marble::RenderPlugin* renderPlugin( const QString & name );

    bool containsRenderPlugin( const QString & name );

    QList<QObject*> floatItems() const;

    Marble::AbstractFloatItem* floatItem( const QString & name );

    bool containsFloatItem( const QString & name );

    /** Returns a list of active (!) float items */
    QStringList activeFloatItems() const;

    /** Activates all of the given float items and deactivates any others */
    void setActiveFloatItems( const QStringList &items );

    /**
      * Returns the screen position of the given coordinate
      * (can be out of the screen borders)
      */
    QPoint pixel( qreal longitude, qreal latitude ) const;

    /**
      * Returns the coordinate at the given screen position
      */
    Coordinate *coordinate( int x, int y );

    QStandardItemModel* mapThemeModel();

    void setGeoSceneProperty( const QString &key, bool value );

    void downloadRoute( qreal offset, int topTileLevel, int bottomTileLevel );

    void downloadArea( int topTileLevel, int bottomTileLevel );

    void setDataPluginDelegate(const QString &plugin, QQmlComponent *delegate );

private Q_SLOTS:
    void updateCenterPosition();

    void forwardMouseClick( qreal lon, qreal lat, GeoDataCoordinates::Unit );

private:
    void addLayer( QQmlListProperty<DeclarativeDataPlugin> *list, DeclarativeDataPlugin *layer );

    Marble::MapThemeManager m_mapThemeManager;

    bool m_inputEnabled;

    Coordinate m_center;

    QList<DeclarativeDataPlugin*> m_dataLayers;

    QList<QObject*> m_children;
};

#endif
