//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#include "MarbleDeclarativeWidget.h"

#include "Coordinate.h"

#include "GeoDataCoordinates.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleMath.h"
#include "MapThemeManager.h"
#include "AbstractFloatItem.h"
#include "AbstractDataPlugin.h"
#include "RenderPlugin.h"
#include "MarbleMap.h"
#include "MarbleDirs.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "DownloadRegion.h"
#include "BookmarkManager.h"
#include "routing/Route.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingModel.h"
#include "routing/RoutingProfilesModel.h"
#include "DeclarativeDataPlugin.h"
#include "PluginManager.h"

#include <QSettings>
#include <QApplication>

MarbleWidget::MarbleWidget( QQuickItem *parent ) :
    Marble::MarbleQuickItem( parent ),
    m_inputEnabled( true )
{
    setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    model()->routingManager()->profilesModel()->loadDefaultProfiles();
    model()->routingManager()->readSettings();
    model()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );

    connect( map(), SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
             this, SIGNAL(visibleLatLonAltBoxChanged()) );
    connect( model(), SIGNAL(workOfflineChanged()),
             this, SIGNAL(workOfflineChanged()) );
    // @TODO FIXME Port to QtQuick 2
    //connect( map(), SIGNAL(zoomChanged(int)),
    //         this, SIGNAL(radiusChanged()) );
    connect( map(), SIGNAL(themeChanged(QString)),
             this, SIGNAL(mapThemeChanged()) );
    // @TODO FIXME Port to QtQuick 2
    //connect( map(), SIGNAL(projectionChanged(Projection)),
    //         this, SIGNAL(projectionChanged()) );
    connect( map(), SIGNAL(mouseClickGeoPosition(qreal,qreal,GeoDataCoordinates::Unit)),
             this, SLOT(forwardMouseClick(qreal,qreal,GeoDataCoordinates::Unit)) );
    connect( &m_center, SIGNAL(latitudeChanged()), this, SLOT(updateCenterPosition()));
    connect( &m_center, SIGNAL(longitudeChanged()), this, SLOT(updateCenterPosition()));
}

MarbleWidget::~MarbleWidget()
{
    model()->routingManager()->writeSettings();
}

const Marble::ViewportParams *MarbleWidget::viewport() const
{
    return map()->viewport();
}

QList<QObject*> MarbleWidget::renderPlugins() const
{
    QList<QObject*> result;
    foreach ( Marble::RenderPlugin* plugin, map()->renderPlugins() ) {
        result << plugin;
    }

    return result;
}

QStringList MarbleWidget::activeFloatItems() const
{
    QStringList result;
    foreach( Marble::AbstractFloatItem * floatItem, map()->floatItems() ) {
        if ( floatItem->enabled() && floatItem->visible() ) {
            result << floatItem->nameId();
        }
    }
    return result;
}

void MarbleWidget::setActiveFloatItems( const QStringList &items )
{
    foreach( Marble::AbstractFloatItem * floatItem, map()->floatItems() ) {
        floatItem->setEnabled( items.contains( floatItem->nameId() ) );
        floatItem->setVisible( items.contains( floatItem->nameId() ) );
    }
}

QStringList MarbleWidget::activeRenderPlugins() const
{
    QStringList result;
    foreach( Marble::RenderPlugin * plugin, map()->renderPlugins() ) {
        if ( plugin->enabled() && plugin->visible() ) {
            result << plugin->nameId();
        }
    }
    return result;
}

QQmlListProperty<QObject> MarbleWidget::childList()
{
    return QQmlListProperty<QObject>( this, m_children );
}

QQmlListProperty<DeclarativeDataPlugin> MarbleWidget::dataLayers()
{
    // @TODO FIXME Port to QtQuick 2
    // return QQmlListProperty<DeclarativeDataPlugin>( this, 0, &Marble::MarbleMap::addLayer, 0, 0, 0 );
    return QQmlListProperty<DeclarativeDataPlugin>();
}

void MarbleWidget::setActiveRenderPlugins( const QStringList &items )
{
    foreach( Marble::RenderPlugin * plugin, map()->renderPlugins() ) {
        plugin->setEnabled( items.contains( plugin->nameId() ) );
        plugin->setVisible( items.contains( plugin->nameId() ) );
    }
}

QPoint MarbleWidget::pixel( qreal lon, qreal lat ) const
{
    Marble::GeoDataCoordinates position( lon, lat, 0, Marble::GeoDataCoordinates::Degree );
    qreal x( 0.0 );
    qreal y( 0.0 );
    const Marble::ViewportParams *viewport = map()->viewport();
    viewport->screenCoordinates( position, x, y );
    return QPoint( x, y );
}

Coordinate *MarbleWidget::coordinate( int x, int y )
{
    qreal lat( 0.0 ), lon( 0.0 );
    map()->geoCoordinates( x, y, lon, lat );
    return new Coordinate( lon, lat, 0.0, this );
}

Coordinate* MarbleWidget::center()
{
    m_center.blockSignals( true );
    m_center.setLongitude( map()->centerLongitude() );
    m_center.setLatitude( map()->centerLatitude() );
    m_center.blockSignals( false );
    return &m_center;
}

void MarbleWidget::setCenter( Coordinate* center )
{
    if ( center ) {
        m_center.blockSignals( true );
        m_center.setLongitude( center->longitude() );
        m_center.setLatitude( center->latitude() );
        m_center.setAltitude( center->altitude() );
        m_center.blockSignals( false );
        updateCenterPosition();
    }
}

void MarbleWidget::updateCenterPosition()
{
    map()->centerOn( m_center.longitude(), m_center.latitude() );
}

void MarbleWidget::forwardMouseClick(qreal lon, qreal lat, Marble::GeoDataCoordinates::Unit unit )
{
    Marble::GeoDataCoordinates position( lon, lat, unit );
    Marble::GeoDataCoordinates::Unit degree = Marble::GeoDataCoordinates::Degree;
    QPoint const point = pixel( position.longitude( degree ), position.latitude( degree ) );
    QVector<const Marble::GeoDataFeature*> const features = map()->whichFeatureAt( point );
    if ( !features.isEmpty() ) {
        if ( features.size() == 1 ) {
            Placemark* placemark = new Placemark;
            const Marble::GeoDataPlacemark * geoDataPlacemark = dynamic_cast<const Marble::GeoDataPlacemark*>( features.first() );
            if ( geoDataPlacemark ) {
                placemark->setGeoDataPlacemark( *geoDataPlacemark );
                emit placemarkSelected( placemark );
            }
        }
    } else {
        emit mouseClickGeoPosition( position.longitude( degree ),
                                    position.latitude( degree ) );
    }
}

void MarbleWidget::addLayer( QQmlListProperty<DeclarativeDataPlugin> *list, DeclarativeDataPlugin *layer )
{
    MarbleWidget *object = qobject_cast<MarbleWidget *>( list->object );
    if ( object ) {
        model()->pluginManager()->addRenderPlugin( layer );
        object->setDataPluginDelegate( layer->nameId(), layer->delegate() );
        object->m_dataLayers << layer;
    }
}

QStandardItemModel *MarbleWidget::mapThemeModel()
{
    return m_mapThemeManager.mapThemeModel();
}

void MarbleWidget::setGeoSceneProperty(const QString &key, bool value)
{
    map()->setPropertyValue( key, value );
}

void MarbleWidget::downloadRoute( qreal offset, int topTileLevel, int bottomTileLevel )
{
    Marble::DownloadRegion region;
    region.setMarbleModel( model() );
    region.setVisibleTileLevel( map()->tileZoomLevel() );
    region.setTileLevelRange( topTileLevel, bottomTileLevel );
    const Marble::GeoDataLineString waypoints = model()->routingManager()->routingModel()->route().path();
    QVector<Marble::TileCoordsPyramid> const pyramid = region.fromPath( map()->textureLayer(), offset, waypoints );
    if ( !pyramid.isEmpty() ) {
        map()->downloadRegion( pyramid );
    }
}

void MarbleWidget::downloadArea(int topTileLevel, int bottomTileLevel)
{
    Marble::DownloadRegion region;
    region.setMarbleModel( model() );
    region.setVisibleTileLevel( map()->tileZoomLevel() );
    region.setTileLevelRange( topTileLevel, bottomTileLevel );
    QVector<Marble::TileCoordsPyramid> const pyramid = region.region( map()->textureLayer(), map()->viewport()->viewLatLonAltBox() );
    if ( !pyramid.isEmpty() ) {
        map()->downloadRegion( pyramid );
    }
}

void MarbleWidget::setDataPluginDelegate( const QString &plugin, QQmlComponent *delegate )
{
    QList<Marble::RenderPlugin*> renderPlugins = map()->renderPlugins();
    foreach( Marble::RenderPlugin* renderPlugin, renderPlugins ) {
        Marble::AbstractDataPlugin* dataPlugin = qobject_cast<Marble::AbstractDataPlugin*>( renderPlugin );
        if ( dataPlugin && dataPlugin->nameId() == plugin ) {
            // @TODO FIXME Port to QtQuick 2
            // dataPlugin->setDelegate( delegate, this );
        }
    }
}

bool MarbleWidget::workOffline() const
{
    return model()->workOffline();
}

void MarbleWidget::setWorkOffline( bool workOffline )
{
    model()->setWorkOffline( workOffline );
}

int MarbleWidget::radius() const
{
    return map()->radius();
}

void MarbleWidget::setRadius( int radius )
{
    map()->setRadius( radius );
}

Marble::RenderPlugin *MarbleWidget::renderPlugin( const QString & name )
{
    foreach( Marble::RenderPlugin * plugin, map()->renderPlugins() )
    {
        if( plugin->nameId() == name ) {
            return plugin;
        }
    }
    return 0;
}

bool MarbleWidget::containsRenderPlugin( const QString & name )
{
    foreach( Marble::RenderPlugin * plugin, map()->renderPlugins() )
    {
        if( plugin->nameId() == name ) {
            return true;
        }
    }
    return false;
}

QList<QObject*> MarbleWidget::floatItems() const
{
    QList<QObject*> result;
    foreach ( Marble::AbstractFloatItem* plugin, map()->floatItems() ) {
        result << plugin;
    }

    return result;
}

Marble::AbstractFloatItem* MarbleWidget::floatItem( const QString & name )
{
    foreach( Marble::AbstractFloatItem * plugin, map()->floatItems() )
    {
        if( plugin->nameId() == name ) {
            return plugin ;
        }
    }
    return 0;
}

bool MarbleWidget::containsFloatItem( const QString & name )
{
    foreach( Marble::AbstractFloatItem * plugin, map()->floatItems() )
    {
        if( plugin->nameId() == name ) {
            return true;
        }
    }
    return false;
}

#include "moc_MarbleDeclarativeWidget.cpp"
