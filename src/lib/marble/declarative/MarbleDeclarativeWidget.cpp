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
#include "ZoomButtonInterceptor.h"

#include "GeoDataCoordinates.h"
#include "MarbleWidget.h"
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

#if QT_VERSION < 0x050000
  typedef QDeclarativeComponent QQmlComponent;
#endif

MarbleWidget::MarbleWidget( QGraphicsItem *parent , Qt::WindowFlags flags ) :
    QGraphicsProxyWidget( parent, flags ),
    m_marbleWidget( new Marble::MarbleWidget ),
    m_inputEnabled( true ),
    m_interceptor( new ZoomButtonInterceptor( this, this ) )
{
    m_marbleWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    QSettings settings;
    m_marbleWidget->readPluginSettings( settings );
    m_marbleWidget->model()->routingManager()->profilesModel()->loadDefaultProfiles();
    m_marbleWidget->model()->routingManager()->readSettings();
    m_marbleWidget->model()->bookmarkManager()->loadFile( "bookmarks/bookmarks.kml" );
    setWidget( m_marbleWidget );

    connect( m_marbleWidget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
             this, SIGNAL(visibleLatLonAltBoxChanged()) );
    connect( m_marbleWidget->model(), SIGNAL(workOfflineChanged()),
             this, SIGNAL(workOfflineChanged()) );
    connect( m_marbleWidget, SIGNAL(zoomChanged(int)),
             this, SIGNAL(radiusChanged()) );
    connect( m_marbleWidget, SIGNAL(themeChanged(QString)),
             this, SIGNAL(mapThemeChanged()) );
    connect( m_marbleWidget, SIGNAL(projectionChanged(Projection)),
             this, SIGNAL(projectionChanged()) );
    connect( m_marbleWidget, SIGNAL(mouseClickGeoPosition(qreal,qreal,GeoDataCoordinates::Unit)),
             this, SLOT(forwardMouseClick(qreal,qreal,GeoDataCoordinates::Unit)) );
    connect( &m_center, SIGNAL(latitudeChanged()), this, SLOT(updateCenterPosition()));
    connect( &m_center, SIGNAL(longitudeChanged()), this, SLOT(updateCenterPosition()));

    m_marbleWidget->inputHandler()->setMouseButtonPopupEnabled( Qt::LeftButton, false );
    m_marbleWidget->inputHandler()->setPanViaArrowsEnabled( false );
    grabGesture( Qt::PinchGesture, Qt::ReceivePartialGestures | Qt::IgnoredGesturesPropagateToParent );
    setAcceptTouchEvents(true);
}

MarbleWidget::~MarbleWidget()
{
    QSettings settings;
    m_marbleWidget->writePluginSettings( settings );
    m_marbleWidget->model()->routingManager()->writeSettings();
}

Marble::MarbleModel *MarbleWidget::model()
{
    return m_marbleWidget->model();
}

const Marble::ViewportParams *MarbleWidget::viewport() const
{
    return m_marbleWidget->viewport();
}

QList<QObject*> MarbleWidget::renderPlugins() const
{
    QList<QObject*> result;
    foreach ( Marble::RenderPlugin* plugin, m_marbleWidget->renderPlugins() ) {
        result << plugin;
    }

    return result;
}

QStringList MarbleWidget::activeFloatItems() const
{
    QStringList result;
    foreach( Marble::AbstractFloatItem * floatItem, m_marbleWidget->floatItems() ) {
        if ( floatItem->enabled() && floatItem->visible() ) {
            result << floatItem->nameId();
        }
    }
    return result;
}

void MarbleWidget::setActiveFloatItems( const QStringList &items )
{
    foreach( Marble::AbstractFloatItem * floatItem, m_marbleWidget->floatItems() ) {
        floatItem->setEnabled( items.contains( floatItem->nameId() ) );
        floatItem->setVisible( items.contains( floatItem->nameId() ) );
    }
}

QStringList MarbleWidget::activeRenderPlugins() const
{
    QStringList result;
    foreach( Marble::RenderPlugin * plugin, m_marbleWidget->renderPlugins() ) {
        if ( plugin->enabled() && plugin->visible() ) {
            result << plugin->nameId();
        }
    }
    return result;
}

#if QT_VERSION < 0x050000
QDeclarativeListProperty<QObject> MarbleWidget::childList()
{
    return QDeclarativeListProperty<QObject>( this, m_children );
}

QDeclarativeListProperty<DeclarativeDataPlugin> MarbleWidget::dataLayers()
{
    return QDeclarativeListProperty<DeclarativeDataPlugin>( this, 0, &MarbleWidget::addLayer, 0, 0, 0 );
}

#else
QQmlListProperty<QObject> MarbleWidget::childList()
{
    return QQmlListProperty<QObject>( this, m_children );
}

QQmlListProperty<DeclarativeDataPlugin> MarbleWidget::dataLayers()
{
    return QQmlListProperty<DeclarativeDataPlugin>( this, 0, &MarbleWidget::addLayer, 0, 0, 0 );
}
#endif

void MarbleWidget::setActiveRenderPlugins( const QStringList &items )
{
    foreach( Marble::RenderPlugin * plugin, m_marbleWidget->renderPlugins() ) {
        plugin->setEnabled( items.contains( plugin->nameId() ) );
        plugin->setVisible( items.contains( plugin->nameId() ) );
    }
}

bool MarbleWidget::inputEnabled() const
{
    return m_inputEnabled;
}

void MarbleWidget::setInputEnabled( bool enabled )
{
    m_inputEnabled = enabled;
    m_marbleWidget->setInputEnabled( enabled );
}

QString MarbleWidget::mapThemeId() const
{
    return m_marbleWidget->mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString &mapThemeId )
{
    m_marbleWidget->setMapThemeId( mapThemeId );
}

QString MarbleWidget::projection( ) const
{
    switch ( m_marbleWidget->projection() ) {
    case Marble::Equirectangular:
        return "Equirectangular";
    case Marble::Mercator:
        return "Mercator";
    case Marble::Spherical:
        return "Spherical";
    case Marble::Gnomonic:
        return "Gnomonic";
    case Marble::Stereographic:
        return "Stereographic";
    case Marble::LambertAzimuthal:
        return "Lambert Azimuthal Equal-Area";
    case Marble::AzimuthalEquidistant:
        return "Azimuthal Equidistant";
    case Marble::VerticalPerspective:
        return "Perspective Globe";
    }

    Q_ASSERT( false && "Marble got a new projection which we do not know about yet" );
    return "Spherical";
}

void MarbleWidget::setProjection( const QString &projection )
{
    if ( projection.compare( "Equirectangular", Qt::CaseInsensitive ) == 0 ) {
        m_marbleWidget->setProjection( Marble::Equirectangular );
    } else if ( projection.compare( "Mercator", Qt::CaseInsensitive ) == 0 ) {
        m_marbleWidget->setProjection( Marble::Mercator );
    } else {
        m_marbleWidget->setProjection( Marble::Spherical );
    }
}

void MarbleWidget::zoomIn()
{
    m_marbleWidget->zoomIn();
}

void MarbleWidget::zoomOut()
{
    m_marbleWidget->zoomOut();
}

QPoint MarbleWidget::pixel( qreal lon, qreal lat ) const
{
    Marble::GeoDataCoordinates position( lon, lat, 0, Marble::GeoDataCoordinates::Degree );
    qreal x( 0.0 );
    qreal y( 0.0 );
    Marble::ViewportParams *viewport = m_marbleWidget->viewport();
    viewport->screenCoordinates( position, x, y );
    return QPoint( x, y );
}

Coordinate *MarbleWidget::coordinate( int x, int y )
{
    qreal lat( 0.0 ), lon( 0.0 );
    m_marbleWidget->geoCoordinates( x, y, lon, lat );
    return new Coordinate( lon, lat, 0.0, this );
}

Coordinate* MarbleWidget::center()
{
    m_center.blockSignals( true );
    m_center.setLongitude( m_marbleWidget->centerLongitude() );
    m_center.setLatitude( m_marbleWidget->centerLatitude() );
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

void MarbleWidget::centerOn( const Marble::GeoDataLatLonBox &bbox )
{
    m_marbleWidget->centerOn( bbox );
}

void MarbleWidget::centerOn( const Marble::GeoDataCoordinates &coordinates )
{
    m_marbleWidget->centerOn( coordinates );
}

void MarbleWidget::updateCenterPosition()
{
    m_marbleWidget->centerOn( m_center.longitude(), m_center.latitude() );
}

void MarbleWidget::forwardMouseClick(qreal lon, qreal lat, Marble::GeoDataCoordinates::Unit unit )
{
    Marble::GeoDataCoordinates position( lon, lat, unit );
    Marble::GeoDataCoordinates::Unit degree = Marble::GeoDataCoordinates::Degree;
    QPoint const point = pixel( position.longitude( degree ), position.latitude( degree ) );
    QVector<const Marble::GeoDataFeature*> const features = m_marbleWidget->whichFeatureAt( point );
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

#if QT_VERSION < 0x050000
void MarbleWidget::addLayer( QDeclarativeListProperty<DeclarativeDataPlugin> *list, DeclarativeDataPlugin *layer )
#else
void MarbleWidget::addLayer( QQmlListProperty<DeclarativeDataPlugin> *list, DeclarativeDataPlugin *layer )
#endif
{
    MarbleWidget *object = qobject_cast<MarbleWidget *>( list->object );
    if ( object ) {
        object->m_marbleWidget->model()->pluginManager()->addRenderPlugin( layer );
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
    m_marbleWidget->setPropertyValue( key, value );
}

void MarbleWidget::downloadRoute( qreal offset, int topTileLevel, int bottomTileLevel )
{
    Marble::DownloadRegion region;
    region.setMarbleModel( m_marbleWidget->model() );
    region.setVisibleTileLevel( m_marbleWidget->tileZoomLevel() );
    region.setTileLevelRange( topTileLevel, bottomTileLevel );
    const Marble::GeoDataLineString waypoints = m_marbleWidget->model()->routingManager()->routingModel()->route().path();
    QVector<Marble::TileCoordsPyramid> const pyramid = region.fromPath( m_marbleWidget->textureLayer(), offset, waypoints );
    if ( !pyramid.isEmpty() ) {
        m_marbleWidget->downloadRegion( pyramid );
    }
}

void MarbleWidget::downloadArea(int topTileLevel, int bottomTileLevel)
{
    Marble::DownloadRegion region;
    region.setMarbleModel( m_marbleWidget->model() );
    region.setVisibleTileLevel( m_marbleWidget->tileZoomLevel() );
    region.setTileLevelRange( topTileLevel, bottomTileLevel );
    QVector<Marble::TileCoordsPyramid> const pyramid = region.region( m_marbleWidget->textureLayer(), m_marbleWidget->viewport()->viewLatLonAltBox() );
    if ( !pyramid.isEmpty() ) {
        m_marbleWidget->downloadRegion( pyramid );
    }
}

void MarbleWidget::setDataPluginDelegate( const QString &plugin, QQmlComponent *delegate )
{
    QList<Marble::RenderPlugin*> renderPlugins = m_marbleWidget->renderPlugins();
    foreach( Marble::RenderPlugin* renderPlugin, renderPlugins ) {
        Marble::AbstractDataPlugin* dataPlugin = qobject_cast<Marble::AbstractDataPlugin*>( renderPlugin );
        if ( dataPlugin && dataPlugin->nameId() == plugin ) {
            dataPlugin->setDelegate( delegate, this );
        }
    }
}

bool MarbleWidget::workOffline() const
{
    return m_marbleWidget->model()->workOffline();
}

void MarbleWidget::setWorkOffline( bool workOffline )
{
    m_marbleWidget->model()->setWorkOffline( workOffline );
}

int MarbleWidget::radius() const
{
    return m_marbleWidget->radius();
}

void MarbleWidget::setRadius( int radius )
{
    m_marbleWidget->setRadius( radius );
}

bool MarbleWidget::event ( QEvent * event )
{
    if ( m_marbleWidget && event && event->type() == QEvent::Gesture ) {
        return QApplication::sendEvent( m_marbleWidget, event );
    }

    return QGraphicsProxyWidget::event( event );
}

bool MarbleWidget::sceneEvent( QEvent *event )
{
    if ( event->type() == QEvent::TouchBegin ) {
        event->accept();
        return true;
    }

    return QGraphicsProxyWidget::sceneEvent( event );
}

Marble::RenderPlugin *MarbleWidget::renderPlugin( const QString & name )
{
    foreach( Marble::RenderPlugin * plugin, m_marbleWidget->renderPlugins() )
    {
        if( plugin->nameId() == name ) {
            return plugin;
        }
    }
    return 0;
}

bool MarbleWidget::containsRenderPlugin( const QString & name )
{
    foreach( Marble::RenderPlugin * plugin, m_marbleWidget->renderPlugins() )
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
    foreach ( Marble::AbstractFloatItem* plugin, m_marbleWidget->floatItems() ) {
        result << plugin;
    }

    return result;
}

Marble::AbstractFloatItem* MarbleWidget::floatItem( const QString & name )
{
    foreach( Marble::AbstractFloatItem * plugin, m_marbleWidget->floatItems() )
    {
        if( plugin->nameId() == name ) {
            return plugin ;
        }
    }
    return 0;
}

bool MarbleWidget::containsFloatItem( const QString & name )
{
    foreach( Marble::AbstractFloatItem * plugin, m_marbleWidget->floatItems() )
    {
        if( plugin->nameId() == name ) {
            return true;
        }
    }
    return false;
}

#include "moc_MarbleDeclarativeWidget.cpp"
