//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "MarbleDeclarativeWidget.h"

#include "MapTheme.h"
#include "Coordinate.h"
#include "Tracking.h"
#include "ZoomButtonInterceptor.h"

#include "GeoDataCoordinates.h"
#include "MarbleWidget.h"
#include "MarbleModel.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleMath.h"
#include "MapThemeManager.h"
#include "AbstractFloatItem.h"
#include "RenderPlugin.h"
#include "MarbleMap.h"
#include "MarbleDirs.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "DownloadRegion.h"
#include "routing/RoutingManager.h"
#include "routing/RoutingProfilesModel.h"

MarbleWidget::MarbleWidget( QGraphicsItem *parent , Qt::WindowFlags flags ) :
    QGraphicsProxyWidget( parent, flags ), m_marbleWidget( new Marble::MarbleWidget ),
    m_inputEnabled( true ), m_tracking( 0 ), m_routing( 0 ), m_navigation( 0 ), m_search( 0 ),
    m_interceptor( new ZoomButtonInterceptor( this, this ) )
{
    m_marbleWidget->setMapThemeId( "earth/openstreetmap/openstreetmap.dgml" );
    m_marbleWidget->model()->routingManager()->profilesModel()->loadDefaultProfiles();
    m_marbleWidget->model()->routingManager()->readSettings();
    setWidget( m_marbleWidget );

    connect( m_marbleWidget, SIGNAL( visibleLatLonAltBoxChanged( GeoDataLatLonAltBox ) ),
             this, SIGNAL( visibleLatLonAltBoxChanged( ) ) );
    connect( m_marbleWidget->model(), SIGNAL( workOfflineChanged() ),
             this, SIGNAL( workOfflineChanged() ) );
    connect( m_marbleWidget, SIGNAL( zoomChanged( int ) ),
             this, SIGNAL( radiusChanged() ) );
    connect( m_marbleWidget, SIGNAL( themeChanged( const QString & ) ),
             this, SIGNAL( mapThemeChanged() ) );
    connect( m_marbleWidget, SIGNAL( mouseClickGeoPosition( qreal, qreal, GeoDataCoordinates::Unit ) ),
             this, SLOT( forwardMouseClick( qreal, qreal, GeoDataCoordinates::Unit ) ) );
    connect( &m_center, SIGNAL(latitudeChanged()), this, SLOT(updateCenterPosition()));
    connect( &m_center, SIGNAL(longitudeChanged()), this, SLOT(updateCenterPosition()));

    m_marbleWidget->inputHandler()->setMouseButtonPopupEnabled( Qt::LeftButton, false );
    m_marbleWidget->inputHandler()->setPanViaArrowsEnabled( false );
    grabGesture( Qt::PinchGesture, Qt::ReceivePartialGestures | Qt::IgnoredGesturesPropagateToParent );
    setAcceptTouchEvents(true);
}

MarbleWidget::~MarbleWidget()
{
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

QList<Marble::RenderPlugin *> MarbleWidget::renderPlugins() const
{
    return m_marbleWidget->renderPlugins();
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

Tracking* MarbleWidget::tracking()
{
    if ( !m_tracking ) {
        m_tracking = new Tracking( this );
        m_tracking->setMarbleWidget( this );
        emit trackingChanged();
    }

    return m_tracking;
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

void MarbleWidget::centerOn( const Marble::GeoDataLatLonAltBox &bbox )
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
    emit mouseClickGeoPosition( position.longitude( Marble::GeoDataCoordinates::Degree ),
                                position.latitude( Marble::GeoDataCoordinates::Degree ) );
}

Routing* MarbleWidget::routing()
{
    if ( !m_routing ) {
        m_routing = new Routing( this );
        m_routing->setMarbleWidget( this );
    }

    return m_routing;
}

Navigation *MarbleWidget::navigation()
{
    if ( !m_navigation ) {
        m_navigation = new Navigation( this );
        m_navigation->setMarbleWidget( this );
    }

    return m_navigation;
}

Search* MarbleWidget::search()
{
    if ( !m_search ) {
        m_search = new Search( this );
        m_search->setMarbleWidget( this );
        m_search->setDelegateParent( this );
    }

    return m_search;
}

QObject *MarbleWidget::mapThemeModel()
{
    return m_marbleWidget->model()->mapThemeManager()->mapThemeModel();
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
    QString const mapThemeId = m_marbleWidget->mapThemeId();
    QString const sourceDir = mapThemeId.left( mapThemeId.lastIndexOf( '/' ) );
    QVector<Marble::TileCoordsPyramid> const pyramid = region.routeRegion( m_marbleWidget->textureLayer(), offset );
    if ( !pyramid.isEmpty() ) {
        m_marbleWidget->downloadRegion( sourceDir, pyramid );
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

#include "MarbleDeclarativeWidget.moc"
