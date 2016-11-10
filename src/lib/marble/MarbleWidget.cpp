//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "MarbleWidget.h"

#include <qmath.h>
#include <QHash>
#include <QSettings>
#include <QTime>
#include <QPaintEvent>
#include <QPaintEngine>
#include <QRegion>
#include <QNetworkProxy>
#include <QMetaMethod>
#include "DataMigration.h"
#include "FpsLayer.h"
#include "FileManager.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLookAt.h"
#include "GeoPainter.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "Planet.h"
#include "PopupLayer.h"
#include "RenderState.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "routing/RoutingLayer.h"
#include "MarbleAbstractPresenter.h"
#include "StyleBuilder.h"

namespace Marble
{

class MarbleWidget::CustomPaintLayer : public LayerInterface
{
 public:
    explicit CustomPaintLayer( MarbleWidget *widget )
        : m_widget( widget )
    {
    }

    virtual QStringList renderPosition() const { return QStringList() << "USER_TOOLS"; }

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString &renderPos, GeoSceneLayer *layer )
    {
        Q_UNUSED( viewport );
        Q_UNUSED( renderPos );
        Q_UNUSED( layer );

        painter->setPen( Qt::black );
        m_widget->customPaint( painter );

        return true;
    }

    virtual qreal zValue() const { return 1.0e7; }

    RenderState renderState() const { return RenderState(QStringLiteral("Custom Widget Paint")); }

    QString runtimeTrace() const { return QStringLiteral("MarbleWidget::CustomPaintLayer"); }

 private:
    MarbleWidget *const m_widget;
};


class MarbleWidgetPrivate
{
 public:
    explicit MarbleWidgetPrivate( MarbleWidget *parent ) :
          m_widget( parent ),
          m_model(),
          m_map( &m_model ),
          m_presenter( &m_map ),
          m_inputhandler( 0 ),
          m_routingLayer( 0 ),
          m_mapInfoDialog( 0 ),
          m_customPaintLayer( parent ),
          m_popupmenu( 0 ),
          m_showFrameRate( false )
    {
    }

    ~MarbleWidgetPrivate()
    {
        m_map.removeLayer( &m_customPaintLayer );
        m_map.removeLayer( m_mapInfoDialog );
        delete m_mapInfoDialog;
        delete m_popupmenu;
    }

    void  construct();

    void updateMapTheme();

    void setInputHandler();
    void setInputHandler( MarbleWidgetInputHandler *handler );

    /**
      * @brief Update widget flags and cause a full repaint
      *
      * The background of the widget only needs to be redrawn in certain cases. This
      * method sets the widget flags accordingly and triggers a repaint.
      */
    void updateSystemBackgroundAttribute();

    MarbleWidget    *const m_widget;

    MarbleModel m_model;
    MarbleMap m_map;

    MarbleAbstractPresenter m_presenter;

    MarbleWidgetInputHandler  *m_inputhandler;

    RoutingLayer     *m_routingLayer;
    PopupLayer    *m_mapInfoDialog;
    MarbleWidget::CustomPaintLayer m_customPaintLayer;

    MarbleWidgetPopupMenu *m_popupmenu;

    bool             m_showFrameRate;
};



MarbleWidget::MarbleWidget(QWidget *parent)
    : QWidget( parent ),
      d( new MarbleWidgetPrivate( this ) )
{
//    setAttribute( Qt::WA_PaintOnScreen, true );
    d->construct();
}

MarbleWidget::~MarbleWidget()
{
    // Remove and delete an existing InputHandler
    // initialized in d->construct()
    setInputHandler( 0 );

    delete d;
}

void MarbleWidgetPrivate::construct()
{
    QPointer<DataMigration> dataMigration = new DataMigration( m_widget );
    dataMigration->exec();
    delete dataMigration;

    // Widget settings
    m_widget->setMinimumSize( 200, 300 );
    m_widget->setFocusPolicy( Qt::WheelFocus );
    m_widget->setFocus( Qt::OtherFocusReason );

    // Set background: black.
    m_widget->setPalette( QPalette ( Qt::black ) );

    // Set whether the black space gets displayed or the earth gets simply
    // displayed on the widget background.
    m_widget->setAutoFillBackground( true );

    // Initialize the map and forward some signals.
    m_map.setSize( m_widget->width(), m_widget->height() );
    m_map.setShowFrameRate( false );  // never let the map draw the frame rate,
                                       // we do this differently here in the widget

    m_widget->connect( &m_presenter, SIGNAL(regionSelected(QList<double>)), m_widget, SIGNAL(regionSelected(QList<double>)) );

    m_widget->connect( &m_presenter, SIGNAL(zoomChanged(int)), m_widget, SIGNAL(zoomChanged(int)) );
    m_widget->connect( &m_presenter, SIGNAL(distanceChanged(QString)), m_widget, SIGNAL(distanceChanged(QString)) );

    // forward some signals of m_map
    m_widget->connect( &m_map,   SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                       m_widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)) );
    m_widget->connect( &m_map,   SIGNAL(projectionChanged(Projection)),
                       m_widget, SIGNAL(projectionChanged(Projection)) );
    m_widget->connect( &m_map,   SIGNAL(tileLevelChanged(int)),
                       m_widget, SIGNAL(tileLevelChanged(int)) );
    m_widget->connect( &m_map,   SIGNAL(framesPerSecond(qreal)),
                       m_widget, SIGNAL(framesPerSecond(qreal)) );
    m_widget->connect( &m_map,   SIGNAL(viewContextChanged(ViewContext)),
                       m_widget, SLOT(setViewContext(ViewContext)) );

    m_widget->connect( &m_map,   SIGNAL(pluginSettingsChanged()),
                       m_widget, SIGNAL(pluginSettingsChanged()) );
    m_widget->connect( &m_map,   SIGNAL(renderPluginInitialized(RenderPlugin*)),
                       m_widget, SIGNAL(renderPluginInitialized(RenderPlugin*)) );

    // react to some signals of m_map
    m_widget->connect( &m_map,   SIGNAL(themeChanged(QString)),
                       m_widget, SLOT(updateMapTheme()) );
    m_widget->connect( &m_map,   SIGNAL(viewContextChanged(ViewContext)),
                       m_widget, SIGNAL(viewContextChanged(ViewContext)) );
    m_widget->connect( &m_map,   SIGNAL(repaintNeeded(QRegion)),
                       m_widget, SLOT(update()) );
    m_widget->connect( &m_map,   SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                       m_widget, SLOT(updateSystemBackgroundAttribute()) );
    m_widget->connect( &m_map,   SIGNAL(renderStatusChanged(RenderStatus)),
                       m_widget, SIGNAL(renderStatusChanged(RenderStatus)) );
    m_widget->connect( &m_map,   SIGNAL(renderStateChanged(RenderState)),
                       m_widget, SIGNAL(renderStateChanged(RenderState)) );

    m_widget->connect( m_model.fileManager(), SIGNAL(centeredDocument(GeoDataLatLonBox)),
                       m_widget, SLOT(centerOn(GeoDataLatLonBox)) );


    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( &m_model, SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                             const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_popupmenu = new MarbleWidgetPopupMenu( m_widget, &m_model );

    m_routingLayer = new RoutingLayer( m_widget, m_widget );
    m_routingLayer->setPlacemarkModel( 0 );
    QObject::connect( m_routingLayer, SIGNAL(repaintNeeded(QRect)),
                      m_widget, SLOT(update()) );

    m_mapInfoDialog = new PopupLayer( m_widget, m_widget );
    m_mapInfoDialog->setVisible( false );
    m_widget->connect( m_mapInfoDialog, SIGNAL(repaintNeeded()), m_widget, SLOT(update()) );
    m_map.addLayer( m_mapInfoDialog );

    setInputHandler();
    m_widget->setMouseTracking( true );

    m_map.addLayer( &m_customPaintLayer );

    m_widget->connect( m_inputhandler, SIGNAL(mouseClickGeoPosition(qreal,qreal,GeoDataCoordinates::Unit)),
                       m_widget, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)) );
    m_widget->setHighlightEnabled( true );

}

void MarbleWidgetPrivate::setInputHandler()
{
    setInputHandler( new MarbleWidgetInputHandler( &m_presenter, m_widget ) );
}

void MarbleWidgetPrivate::setInputHandler( MarbleWidgetInputHandler *handler )
{
    delete m_inputhandler;
    m_inputhandler = handler;

    if ( m_inputhandler )
    {
        m_widget->installEventFilter( m_inputhandler );

        QObject::connect( m_inputhandler, SIGNAL(mouseClickScreenPosition(int,int)),
               m_widget,       SLOT(notifyMouseClick(int,int)) );

        QObject::connect( m_inputhandler, SIGNAL(mouseMoveGeoPosition(QString)),
                 m_widget,       SIGNAL(mouseMoveGeoPosition(QString)) );
    }
}

void MarbleWidgetPrivate::updateSystemBackgroundAttribute()
{
    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    const bool isOn = m_map.viewport()->mapCoversViewport() && !m_model.mapThemeId().isEmpty();
    m_widget->setAttribute( Qt::WA_NoSystemBackground, isOn );
}

// ----------------------------------------------------------------


MarbleModel *MarbleWidget::model()
{
    return &d->m_model;
}

const MarbleModel *MarbleWidget::model() const
{
    return &d->m_model;
}

ViewportParams* MarbleWidget::viewport()
{
    return d->m_map.viewport();
}

const ViewportParams* MarbleWidget::viewport() const
{
    return d->m_map.viewport();
}

MarbleWidgetPopupMenu *MarbleWidget::popupMenu()
{
    return d->m_popupmenu;
}


void MarbleWidget::setInputHandler( MarbleWidgetInputHandler *handler )
{
    d->setInputHandler(handler);
}

MarbleWidgetInputHandler *MarbleWidget::inputHandler() const
{
  return d->m_inputhandler;
}

int MarbleWidget::radius() const
{
    return d->m_map.radius();
}

void MarbleWidget::setRadius( int radius )
{
    d->m_map.setRadius( radius );
}

qreal MarbleWidget::moveStep() const
{
    return d->m_presenter.moveStep();
}

int MarbleWidget::zoom() const
{
    return d->m_presenter.logzoom();
}

int MarbleWidget::tileZoomLevel() const
{
    return d->m_map.tileZoomLevel();
}

int  MarbleWidget::minimumZoom() const
{
    return d->m_map.minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->m_map.maximumZoom();
}

QVector<const GeoDataFeature*> MarbleWidget::whichFeatureAt( const QPoint &curpos ) const
{
    return d->m_map.whichFeatureAt( curpos );
}

QList<AbstractDataPluginItem*> MarbleWidget::whichItemAt( const QPoint &curpos ) const
{
    return d->m_map.whichItemAt( curpos );
}

void MarbleWidget::addLayer( LayerInterface *layer )
{
    d->m_map.addLayer( layer );
}

void MarbleWidget::removeLayer( LayerInterface *layer )
{
    d->m_map.removeLayer( layer );
}

Marble::TextureLayer* MarbleWidget::textureLayer() const
{
    return d->m_map.textureLayer();
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
}

RenderStatus MarbleWidget::renderStatus() const
{
    return d->m_map.renderStatus();
}

RenderState MarbleWidget::renderState() const
{
    return d->m_map.renderState();
}

void MarbleWidget::setHighlightEnabled(bool enabled)
{
    if ( enabled ) {
        connect( this, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
                 &d->m_map, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
                 Qt::UniqueConnection );
    }
    else {
        disconnect( this, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)),
                 &d->m_map, SIGNAL(highlightedPlacemarksChanged(qreal,qreal,GeoDataCoordinates::Unit)) );
    }
}

bool MarbleWidget::showOverviewMap() const
{
    return d->m_map.showOverviewMap();
}

bool MarbleWidget::showScaleBar() const
{
    return d->m_map.showScaleBar();
}

bool MarbleWidget::showCompass() const
{
    return d->m_map.showCompass();
}

bool MarbleWidget::showClouds() const
{
    return d->m_map.showClouds();
}

bool MarbleWidget::showSunShading() const
{
    return d->m_map.showSunShading();
}

bool MarbleWidget::showCityLights() const
{
    return d->m_map.showCityLights();
}

bool MarbleWidget::isLockedToSubSolarPoint() const
{
    return d->m_map.isLockedToSubSolarPoint();
}

bool MarbleWidget::isSubSolarPointIconVisible() const
{
    return d->m_map.isSubSolarPointIconVisible();
}

bool MarbleWidget::showAtmosphere() const
{
    return d->m_map.showAtmosphere();
}

bool MarbleWidget::showCrosshairs() const
{
    return d->m_map.showCrosshairs();
}

bool MarbleWidget::showGrid() const
{
    return d->m_map.showGrid();
}

bool MarbleWidget::showPlaces() const
{
    return d->m_map.showPlaces();
}

bool MarbleWidget::showCities() const
{
    return d->m_map.showCities();
}

bool MarbleWidget::showTerrain() const
{
    return d->m_map.showTerrain();
}

bool MarbleWidget::showOtherPlaces() const
{
    return d->m_map.showOtherPlaces();
}

bool MarbleWidget::showRelief() const
{
    return d->m_map.showRelief();
}

bool MarbleWidget::showIceLayer() const
{
    return d->m_map.showIceLayer();
}

bool MarbleWidget::showBorders() const
{
    return d->m_map.showBorders();
}

bool MarbleWidget::showRivers() const
{
    return d->m_map.showRivers();
}

bool MarbleWidget::showLakes() const
{
    return d->m_map.showLakes();
}

bool MarbleWidget::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool MarbleWidget::showBackground() const
{
    return d->m_map.showBackground();
}

quint64 MarbleWidget::volatileTileCacheLimit() const
{
    return d->m_map.volatileTileCacheLimit();
}


void MarbleWidget::setZoom( int newZoom, FlyToMode mode )
{
    d->m_presenter.setZoom( newZoom, mode );
}

void MarbleWidget::zoomView( int zoom, FlyToMode mode )
{
    d->m_presenter.zoomView( zoom, mode );
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    d->m_presenter.zoomViewBy( zoomStep, mode );
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    d->m_presenter.zoomIn( mode );
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    d->m_presenter.zoomOut( mode );
}

void MarbleWidget::rotateBy( const qreal deltaLon, const qreal deltaLat, FlyToMode mode )
{
    d->m_presenter.rotateBy( deltaLon, deltaLat, mode );
}


void MarbleWidget::centerOn( const qreal lon, const qreal lat, bool animated )
{
    d->m_presenter.centerOn( lon, lat, animated );
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    d->m_presenter.centerOn( position, animated );
}

void MarbleWidget::centerOn( const GeoDataLatLonBox &box, bool animated )
{
   d->m_presenter.centerOn( box, animated );
}

void MarbleWidget::centerOn( const GeoDataPlacemark& placemark, bool animated )
{
    d->m_presenter.centerOn( placemark, animated );
}

void MarbleWidget::setCenterLatitude( qreal lat, FlyToMode mode )
{
    d->m_presenter.setCenterLatitude( lat, mode );
}

void MarbleWidget::setCenterLongitude( qreal lon, FlyToMode mode )
{
    d->m_presenter.setCenterLongitude( lon, mode );
}

Projection MarbleWidget::projection() const
{
    return d->m_map.projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->m_map.setProjection( projection );
}

void MarbleWidget::setProjection( int projection )
{
    setProjection( Projection( qAbs( projection ) % (Mercator+1) ) );
}

void MarbleWidget::moveLeft( FlyToMode mode )
{
    d->m_presenter.moveByStep( -1, 0, mode );
}

void MarbleWidget::moveRight( FlyToMode mode )
{
    d->m_presenter.moveByStep( 1, 0, mode );
}

void MarbleWidget::moveUp( FlyToMode mode )
{
    d->m_presenter.moveByStep( 0, -1, mode );
}

void MarbleWidget::moveDown( FlyToMode mode )
{
    d->m_presenter.moveByStep( 0, 1, mode );
}

void MarbleWidget::leaveEvent( QEvent* )
{
    emit mouseMoveGeoPosition( QCoreApplication::translate( "Marble", NOT_AVAILABLE ) );
}

void MarbleWidget::resizeEvent( QResizeEvent *event )
{
    setUpdatesEnabled( false );
    d->m_map.setSize( event->size() );
    setUpdatesEnabled( true );

    QWidget::resizeEvent( event );
}

void MarbleWidget::connectNotify( const QMetaMethod &signal )
{
    if ( d->m_inputhandler && signal == QMetaMethod::fromSignal( &MarbleWidget::mouseMoveGeoPosition ) ) {
        d->m_inputhandler->setPositionSignalConnected( true );
    }
}

void MarbleWidget::disconnectNotify( const QMetaMethod &signal )
{
    if ( d->m_inputhandler && signal == QMetaMethod::fromSignal( &MarbleWidget::mouseMoveGeoPosition ) ) {
        d->m_inputhandler->setPositionSignalConnected( false );
    }
}

bool MarbleWidget::screenCoordinates( qreal lon, qreal lat,
                                      qreal& x, qreal& y ) const
{
    return d->m_map.screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates( int x, int y,
                                   qreal& lon, qreal& lat,
                                   GeoDataCoordinates::Unit unit ) const
{
    return d->m_map.geoCoordinates( x, y, lon, lat, unit );
}

qreal MarbleWidget::centerLatitude() const
{
    return d->m_map.centerLatitude();
}

qreal MarbleWidget::centerLongitude() const
{
    return d->m_map.centerLongitude();
}

QRegion MarbleWidget::mapRegion() const
{
    return viewport()->mapRegion();
}

void MarbleWidget::paintEvent( QPaintEvent *evt )
{
    QTime t;
    t.start();

    QPaintDevice *paintDevice = this;
    QImage image;
    if (!isEnabled())
    {
        // If the globe covers fully the screen then we can use the faster
        // RGB32 as there are no translucent areas involved.
        QImage::Format imageFormat = ( d->m_map.viewport()->mapCoversViewport() )
                                     ? QImage::Format_RGB32
                                     : QImage::Format_ARGB32_Premultiplied;
        // Paint to an intermediate image
        image = QImage( rect().size(), imageFormat );
        image.fill( Qt::transparent );
        paintDevice = &image;
    }

    {
        // FIXME: Better way to get the GeoPainter
        // Create a painter that will do the painting.
        GeoPainter geoPainter( paintDevice, d->m_map.viewport(), d->m_map.mapQuality() );

        d->m_map.paint( geoPainter, evt->rect() );
    }

    if ( !isEnabled() )
    {
        // Draw a grayscale version of the intermediate image
        QRgb* pixel = reinterpret_cast<QRgb*>( image.scanLine( 0 ));
        for (int i=0; i<image.width()*image.height(); ++i, ++pixel) {
            int gray = qGray( *pixel );
            *pixel = qRgb( gray, gray, gray );
        }

        QPainter widgetPainter( this );
        widgetPainter.drawImage( rect(), image );
    }

    if ( d->m_showFrameRate )
    {
        QPainter painter( this );
        FpsLayer fpsPainter( &t );
        fpsPainter.paint( &painter );

        const qreal fps = 1000.0 / (qreal)( t.elapsed() + 1 );
        emit framesPerSecond( fps );
    }
}

void MarbleWidget::customPaint( GeoPainter *painter )
{
    Q_UNUSED( painter );
    /* This is a NOOP in the base class*/
}


void MarbleWidget::goHome( FlyToMode mode )
{
    d->m_presenter.goHome( mode );
}

QString MarbleWidget::mapThemeId() const
{
    return d->m_model.mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString& mapThemeId )
{
    d->m_map.setMapThemeId( mapThemeId );
}

void MarbleWidgetPrivate::updateMapTheme()
{
    m_map.removeLayer( m_routingLayer );

    m_widget->setRadius( m_widget->radius() ); // Corrects zoom range, if needed

    if (m_model.planetId() == QLatin1String("earth")) {
        m_map.addLayer( m_routingLayer );
    }

    emit m_widget->themeChanged( m_map.mapThemeId() );

    // Now we want a full repaint as the atmosphere might differ
    m_widget->setAttribute( Qt::WA_NoSystemBackground, false );

    m_widget->update();
}

GeoSceneDocument *MarbleWidget::mapTheme() const
{
    return d->m_model.mapTheme();
}

void MarbleWidget::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleWidget the property " << name << "was set to " << value;
    d->m_map.setPropertyValue( name, value );
}

void MarbleWidget::setShowOverviewMap( bool visible )
{
    d->m_map.setShowOverviewMap( visible );
}

void MarbleWidget::setShowScaleBar( bool visible )
{
    d->m_map.setShowScaleBar( visible );
}

void MarbleWidget::setShowCompass( bool visible )
{
    d->m_map.setShowCompass( visible );
}

void MarbleWidget::setShowClouds( bool visible )
{
    d->m_map.setShowClouds( visible );
}

void MarbleWidget::setShowSunShading( bool visible )
{
    d->m_map.setShowSunShading( visible );
}

void MarbleWidget::setShowCityLights( bool visible )
{
    d->m_map.setShowCityLights( visible );
}

void MarbleWidget::setLockToSubSolarPoint( bool visible )
{
    if ( d->m_map.isLockedToSubSolarPoint() != visible ) { // Toggling input modifies event filters, so avoid that if not needed
        d->m_map.setLockToSubSolarPoint( visible );
        setInputEnabled( !d->m_map.isLockedToSubSolarPoint() );
    }
}

void MarbleWidget::setSubSolarPointIconVisible( bool visible )
{
    if ( d->m_map.isSubSolarPointIconVisible() != visible ) {
        d->m_map.setSubSolarPointIconVisible( visible );
    }

    QList<RenderPlugin *> pluginList = renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ((*i)->nameId() == QLatin1String("sun")) {
            (*i)->setVisible( visible );
        }
    }
}

void MarbleWidget::setShowAtmosphere( bool visible )
{
    d->m_map.setShowAtmosphere( visible );
}

void MarbleWidget::setShowCrosshairs( bool visible )
{
    d->m_map.setShowCrosshairs( visible );
}

void MarbleWidget::setShowGrid( bool visible )
{
    d->m_map.setShowGrid( visible );
}

void MarbleWidget::setShowPlaces( bool visible )
{
    d->m_map.setShowPlaces( visible );
}

void MarbleWidget::setShowCities( bool visible )
{
    d->m_map.setShowCities( visible );
}

void MarbleWidget::setShowTerrain( bool visible )
{
    d->m_map.setShowTerrain( visible );
}

void MarbleWidget::setShowOtherPlaces( bool visible )
{
    d->m_map.setShowOtherPlaces( visible );
}

void MarbleWidget::setShowRelief( bool visible )
{
    d->m_map.setShowRelief( visible );
}

void MarbleWidget::setShowIceLayer( bool visible )
{
    d->m_map.setShowIceLayer( visible );
}

void MarbleWidget::setShowBorders( bool visible )
{
    d->m_map.setShowBorders( visible );
}

void MarbleWidget::setShowRivers( bool visible )
{
    d->m_map.setShowRivers( visible );
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->m_map.setShowLakes( visible );
}

void MarbleWidget::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;

    update();
}

void MarbleWidget::setShowBackground( bool visible )
{
    d->m_map.setShowBackground( visible );
}

void MarbleWidget::setShowRuntimeTrace( bool visible )
{
    d->m_map.setShowRuntimeTrace( visible );
}

bool MarbleWidget::showRuntimeTrace() const
{
    return d->m_map.showRuntimeTrace();
}

void MarbleWidget::setShowDebugPolygons( bool visible)
{
    d->m_map.setShowDebugPolygons( visible );
}

bool MarbleWidget::showDebugPolygons() const
{
    return d->m_map.showDebugPolygons();
}

void MarbleWidget::setShowDebugBatchRender( bool visible)
{
    d->m_map.setShowDebugBatchRender( visible );
}

bool MarbleWidget::showDebugBatchRender() const
{
    return d->m_map.showDebugBatchRender();
}

void MarbleWidget::setShowDebugPlacemarks( bool visible)
{
    d->m_map.setShowDebugPlacemarks( visible );
}

bool MarbleWidget::showDebugPlacemarks() const
{
    return d->m_map.showDebugPlacemarks();
}

void MarbleWidget::setShowTileId( bool visible )
{
    d->m_map.setShowTileId( visible );
}

void MarbleWidget::notifyMouseClick( int x, int y)
{
    qreal  lon   = 0;
    qreal  lat   = 0;

    bool const valid = geoCoordinates( x, y, lon, lat, GeoDataCoordinates::Radian );

    if ( valid ) {
        emit mouseClickGeoPosition( lon, lat, GeoDataCoordinates::Radian );
    }
}

void MarbleWidget::clearVolatileTileCache()
{
    mDebug() << "About to clear VolatileTileCache";
    d->m_map.clearVolatileTileCache();
}

void MarbleWidget::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->m_map.setVolatileTileCacheLimit( kiloBytes );
}

// This slot will called when the Globe starts to create the tiles.

void MarbleWidget::creatingTilesStart( TileCreator *creator,
                                       const QString &name, 
                                       const QString &description )
{
    QPointer<TileCreatorDialog> dialog = new TileCreatorDialog( creator, this );
    dialog->setSummary( name, description );
    dialog->exec();
    delete dialog;
}

MapQuality MarbleWidget::mapQuality( ViewContext viewContext ) const
{
    return d->m_map.mapQuality( viewContext );
}

void MarbleWidget::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    d->m_map.setMapQualityForViewContext( quality, viewContext );
}

ViewContext MarbleWidget::viewContext() const
{
    return d->m_map.viewContext();
}

void MarbleWidget::setViewContext( ViewContext viewContext )
{
    // Inform routing layer about view context change. If not done,
    // the routing layer causes severe performance problems when dragging the
    // map. So either do not remove this line, or keep a similar call in place
    // when you refactor it and test your changes wrt drag performance at
    // high zoom level with long routes!
    d->m_routingLayer->setViewContext( viewContext );

    d->m_map.setViewContext( viewContext );
}

bool MarbleWidget::animationsEnabled() const
{
    return d->m_presenter.animationsEnabled();
}

void MarbleWidget::setAnimationsEnabled( bool enabled )
{
    d->m_presenter.setAnimationsEnabled( enabled );
}

AngleUnit MarbleWidget::defaultAngleUnit() const
{
    return d->m_map.defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( AngleUnit angleUnit )
{
    d->m_map.setDefaultAngleUnit( angleUnit );
}

QFont MarbleWidget::defaultFont() const
{
    return d->m_map.defaultFont();
}

void MarbleWidget::setDefaultFont( const QFont& font )
{
    d->m_map.setDefaultFont( font );
}

void MarbleWidget::setSelection( const QRect& region )
{
    d->m_presenter.setSelection( region );
}

qreal MarbleWidget::distance() const
{
    return d->m_presenter.distance();
}

void MarbleWidget::setDistance( qreal newDistance )
{
    d->m_presenter.setDistance( newDistance );
}

QString MarbleWidget::distanceString() const
{
    return d->m_presenter.distanceString();
}

void MarbleWidget::setInputEnabled( bool enabled )
{
    //if input is set as enabled
    if ( enabled )
    {
        if ( !d->m_inputhandler ) {
            d->setInputHandler();
        }
        else {
            installEventFilter( d->m_inputhandler );
        }
    }

    else // input is disabled
    {
        mDebug() << "MarbleWidget::disableInput";
        removeEventFilter( d->m_inputhandler );
        setCursor( Qt::ArrowCursor );
    }
}

QList<RenderPlugin *> MarbleWidget::renderPlugins() const
{
    return d->m_map.renderPlugins();
}

void MarbleWidget::readPluginSettings( QSettings& settings )
{
    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup(QLatin1String("plugin_") + plugin->nameId());

        QHash<QString,QVariant> hash;

        foreach ( const QString& key, settings.childKeys() ) {
            hash.insert( key, settings.value( key ) );
        }

        plugin->setSettings( hash );

        settings.endGroup();
    }
}

void MarbleWidget::writePluginSettings( QSettings& settings ) const
{
    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup(QLatin1String("plugin_") + plugin->nameId());

        QHash<QString,QVariant> hash = plugin->settings();

        QHash<QString,QVariant>::iterator it = hash.begin();
        while( it != hash.end() ) {
            settings.setValue( it.key(), it.value() );
            ++it;
        }

        settings.endGroup();
    }
}

QList<AbstractFloatItem *> MarbleWidget::floatItems() const
{
    return d->m_map.floatItems();
}

AbstractFloatItem * MarbleWidget::floatItem( const QString &nameId ) const
{
    return d->m_map.floatItem( nameId );
}

void MarbleWidget::changeEvent( QEvent * event )
{
    if ( event->type() == QEvent::EnabledChange )
    {
        setInputEnabled(isEnabled());
    }

    QWidget::changeEvent(event);
}

void MarbleWidget::flyTo( const GeoDataLookAt &newLookAt, FlyToMode mode )
{
    d->m_presenter.flyTo( newLookAt, mode );
}

void MarbleWidget::reloadMap()
{
    d->m_map.reload();
}

void MarbleWidget::downloadRegion( QVector<TileCoordsPyramid> const & pyramid )
{
    d->m_map.downloadRegion( pyramid );
}

GeoDataLookAt MarbleWidget::lookAt() const
{
    return d->m_presenter.lookAt();
}

GeoDataCoordinates MarbleWidget::focusPoint() const
{
    return d->m_map.viewport()->focusPoint();
}

void MarbleWidget::setFocusPoint( const GeoDataCoordinates &focusPoint )
{
    d->m_map.viewport()->setFocusPoint( focusPoint );
}

void MarbleWidget::resetFocusPoint()
{
    d->m_map.viewport()->resetFocusPoint();
}

qreal MarbleWidget::radiusFromDistance( qreal distance ) const
{
    return d->m_presenter.radiusFromDistance( distance );
}

qreal MarbleWidget::distanceFromRadius( qreal radius ) const
{
    return d->m_presenter.distanceFromRadius( radius );
}

qreal MarbleWidget::zoomFromDistance( qreal distance ) const
{
    return d->m_presenter.zoomFromDistance( distance );
}

qreal MarbleWidget::distanceFromZoom( qreal zoom ) const
{
    return d->m_presenter.distanceFromZoom( zoom );
}

RoutingLayer* MarbleWidget::routingLayer()
{
    return d->m_routingLayer;
}

PopupLayer *MarbleWidget::popupLayer()
{
    return d->m_mapInfoDialog;
}

const StyleBuilder* MarbleWidget::styleBuilder() const
{
    return d->m_map.styleBuilder();
}

}

#include "moc_MarbleWidget.cpp"
