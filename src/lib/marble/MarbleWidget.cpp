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
#include <QAbstractItemModel>
#include <QHash>
#include <QSettings>
#include <QTime>
#include <QItemSelectionModel>
#include <QPaintEvent>
#include <QRegion>
#include <QSizePolicy>
#include <QNetworkProxy>
#include <QMetaMethod>

#ifdef MARBLE_DBUS
#include <QDBusConnection>
#endif

#include "DataMigration.h"
#include "FpsLayer.h"
#include "FileManager.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoPainter.h"
#include "MarbleClock.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "MarbleLocale.h"
#include "MarbleMap.h"
#include "MarbleModel.h"
#include "MarblePhysics.h"
#include "MarbleWidgetInputHandler.h"
#include "MarbleWidgetPopupMenu.h"
#include "Planet.h"
#include "PopupLayer.h"
#include "RenderPlugin.h"
#include "SunLocator.h"
#include "TileCreatorDialog.h"
#include "ViewportParams.h"
#include "routing/RoutingLayer.h"
#include "MarbleAbstractPresenter.h"

namespace Marble
{

class MarbleWidget::CustomPaintLayer : public LayerInterface
{
 public:
    CustomPaintLayer( MarbleWidget *widget )
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

 private:
    MarbleWidget *const m_widget;
};


class MarbleWidgetPrivate : public MarbleAbstractPresenter
{
 public:
    MarbleWidgetPrivate( MarbleWidget *parent )
        : MarbleAbstractPresenter(),
          m_widget( parent ),
          m_inputhandler( 0 ),
          m_physics( this ),
          m_routingLayer( 0 ),
          m_mapInfoDialog( 0 ),
          m_customPaintLayer( parent ),
          m_popupmenu( 0 ),
          m_showFrameRate( false )
    {
    }

    ~MarbleWidgetPrivate()
    {
        map()->removeLayer( &m_customPaintLayer );
        map()->removeLayer( m_mapInfoDialog );
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

    MarbleWidgetInputHandler  *m_inputhandler;

    MarblePhysics    m_physics;

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

#ifdef MARBLE_DBUS
    QDBusConnection::sessionBus().registerObject( "/MarbleWidget", m_widget,
                                                  QDBusConnection::ExportAllSlots
                                                  | QDBusConnection::ExportAllSignals
                                                  | QDBusConnection::ExportAllProperties );
#endif

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
    map()->setSize( m_widget->width(), m_widget->height() );
    map()->setShowFrameRate( false );  // never let the map draw the frame rate,
                                       // we do this differently here in the widget

    m_widget->connect(this, SIGNAL(regionSelected(QList<double>)), m_widget, SIGNAL(regionSelected(QList<double>)));

    m_widget->connect(this, SIGNAL(updateRequired()),
                      m_widget, SLOT(update()));
    m_widget->connect(this, SIGNAL(zoomChanged(int)), m_widget, SIGNAL(zoomChanged(int)));
    m_widget->connect(this, SIGNAL(distanceChanged(QString)), m_widget, SIGNAL(distanceChanged(QString)));

    // forward some signals of m_map
    m_widget->connect( map(),   SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                       m_widget, SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)) );
    m_widget->connect( map(),   SIGNAL(projectionChanged(Projection)),
                       m_widget, SIGNAL(projectionChanged(Projection)) );
    m_widget->connect( map(),   SIGNAL(tileLevelChanged(int)),
                       m_widget, SIGNAL(tileLevelChanged(int)) );
    m_widget->connect( map(),   SIGNAL(framesPerSecond(qreal)),
                       m_widget, SIGNAL(framesPerSecond(qreal)) );

    m_widget->connect( map(),   SIGNAL(pluginSettingsChanged()),
                       m_widget, SIGNAL(pluginSettingsChanged()) );
    m_widget->connect( map(),   SIGNAL(renderPluginInitialized(RenderPlugin*)),
                       m_widget, SIGNAL(renderPluginInitialized(RenderPlugin*)) );

    // react to some signals of m_map
    m_widget->connect( map(),   SIGNAL(themeChanged(QString)),
                       m_widget, SLOT(updateMapTheme()) );
    m_widget->connect( map(),   SIGNAL(repaintNeeded(QRegion)),
                       m_widget, SLOT(update()) );
    m_widget->connect( map(),   SIGNAL(visibleLatLonAltBoxChanged(GeoDataLatLonAltBox)),
                       m_widget, SLOT(updateSystemBackgroundAttribute()) );

    m_widget->connect( model()->fileManager(), SIGNAL(centeredDocument(GeoDataLatLonBox)),
                       m_widget, SLOT(centerOn(GeoDataLatLonBox)) );


    // Show a progress dialog when the model calculates new map tiles.
    m_widget->connect( model(), SIGNAL( creatingTilesStart( TileCreator*, const QString&,
                                                             const QString& ) ),
                       m_widget, SLOT( creatingTilesStart( TileCreator*, const QString&,
                                                           const QString& ) ) );

    m_popupmenu = new MarbleWidgetPopupMenu( m_widget, model() );

    m_routingLayer = new RoutingLayer( m_widget, m_widget );
    m_routingLayer->setPlacemarkModel( 0 );
    QObject::connect( m_routingLayer, SIGNAL(repaintNeeded(QRect)),
                      m_widget, SLOT(update()) );

    m_mapInfoDialog = new PopupLayer( m_widget, m_widget );
    m_mapInfoDialog->setVisible( false );
    m_widget->connect( m_mapInfoDialog, SIGNAL(repaintNeeded()), m_widget, SLOT(update()) );
    map()->addLayer( m_mapInfoDialog );

    setInputHandler();
    m_widget->setMouseTracking( true );

    map()->addLayer( &m_customPaintLayer );
}

void MarbleWidgetPrivate::setInputHandler()
{
    setInputHandler(new MarbleWidgetInputHandler(this, m_widget));
}

void MarbleWidgetPrivate::setInputHandler( MarbleWidgetInputHandler *handler )
{
    delete m_inputhandler;
    m_inputhandler = handler;

    if ( m_inputhandler )
    {
        m_widget->installEventFilter( m_inputhandler );

        connect( m_inputhandler, SIGNAL(mouseClickScreenPosition(int,int)),
               m_widget,       SLOT(notifyMouseClick(int,int)) );

        connect( m_inputhandler, SIGNAL(mouseMoveGeoPosition(QString)),
                 m_widget,       SIGNAL(mouseMoveGeoPosition(QString)) );
    }
}

void MarbleWidgetPrivate::updateSystemBackgroundAttribute()
{
    // We only have to repaint the background every time if the earth
    // doesn't cover the whole image.
    const bool isOn = map()->viewport()->mapCoversViewport() && !model()->mapThemeId().isEmpty();
    m_widget->setAttribute( Qt::WA_NoSystemBackground, isOn );
}

// ----------------------------------------------------------------


MarbleModel *MarbleWidget::model()
{
    return d->model();
}

const MarbleModel *MarbleWidget::model() const
{
    return d->model();
}

ViewportParams* MarbleWidget::viewport()
{
    return d->viewport();
}

const ViewportParams* MarbleWidget::viewport() const
{
    return d->viewport();
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
    return d->radius();
}

void MarbleWidget::setRadius( int radius )
{
    d->setRadius(radius);
}

qreal MarbleWidget::moveStep() const
{
    return d->moveStep();
}

int MarbleWidget::zoom() const
{
    return d->logzoom();
}

int MarbleWidget::tileZoomLevel() const
{
    return d->map()->tileZoomLevel();
}

int  MarbleWidget::minimumZoom() const
{
    return d->minimumZoom();
}

int  MarbleWidget::maximumZoom() const
{
    return d->maximumZoom();
}

QVector<const GeoDataPlacemark*> MarbleWidget::whichFeatureAt( const QPoint &curpos ) const
{
    return d->map()->whichFeatureAt( curpos );
}

QList<AbstractDataPluginItem*> MarbleWidget::whichItemAt( const QPoint &curpos ) const
{
    return d->map()->whichItemAt( curpos );
}

void MarbleWidget::addLayer( LayerInterface *layer )
{
    d->map()->addLayer( layer );
}

void MarbleWidget::removeLayer( LayerInterface *layer )
{
    d->map()->removeLayer( layer );
}

Marble::TextureLayer* MarbleWidget::textureLayer() const
{
    return d->map()->textureLayer();
}

QPixmap MarbleWidget::mapScreenShot()
{
    return QPixmap::grabWidget( this );
}

bool MarbleWidget::showOverviewMap() const
{
    return d->map()->showOverviewMap();
}

bool MarbleWidget::showScaleBar() const
{
    return d->map()->showScaleBar();
}

bool MarbleWidget::showCompass() const
{
    return d->map()->showCompass();
}

bool MarbleWidget::showClouds() const
{
    return d->map()->showClouds();
}

bool MarbleWidget::showSunShading() const
{
    return d->map()->showSunShading();
}

bool MarbleWidget::showCityLights() const
{
    return d->map()->showCityLights();
}

bool MarbleWidget::isLockedToSubSolarPoint() const
{
    return d->map()->isLockedToSubSolarPoint();
}

bool MarbleWidget::isSubSolarPointIconVisible() const
{
    return d->map()->isSubSolarPointIconVisible();
}

bool MarbleWidget::showAtmosphere() const
{
    return d->map()->showAtmosphere();
}

bool MarbleWidget::showCrosshairs() const
{
    return d->map()->showCrosshairs();
}

bool MarbleWidget::showGrid() const
{
    return d->map()->showGrid();
}

bool MarbleWidget::showPlaces() const
{
    return d->map()->showPlaces();
}

bool MarbleWidget::showCities() const
{
    return d->map()->showCities();
}

bool MarbleWidget::showTerrain() const
{
    return d->map()->showTerrain();
}

bool MarbleWidget::showOtherPlaces() const
{
    return d->map()->showOtherPlaces();
}

bool MarbleWidget::showRelief() const
{
    return d->map()->showRelief();
}

bool MarbleWidget::showIceLayer() const
{
    return d->map()->showIceLayer();
}

bool MarbleWidget::showBorders() const
{
    return d->map()->showBorders();
}

bool MarbleWidget::showRivers() const
{
    return d->map()->showRivers();
}

bool MarbleWidget::showLakes() const
{
    return d->map()->showLakes();
}

bool MarbleWidget::showFrameRate() const
{
    return d->m_showFrameRate;
}

bool MarbleWidget::showBackground() const
{
    return d->map()->showBackground();
}

quint64 MarbleWidget::volatileTileCacheLimit() const
{
    return d->map()->volatileTileCacheLimit();
}


void MarbleWidget::setZoom( int newZoom, FlyToMode mode )
{
    d->setZoom(newZoom, mode);
}

void MarbleWidget::zoomView( int zoom, FlyToMode mode )
{
    d->zoomView(zoom, mode);
}


void MarbleWidget::zoomViewBy( int zoomStep, FlyToMode mode )
{
    d->zoomViewBy(zoomStep, mode);
}


void MarbleWidget::zoomIn( FlyToMode mode )
{
    d->zoomIn(mode);
}

void MarbleWidget::zoomOut( FlyToMode mode )
{
    d->zoomOut(mode);
}

void MarbleWidget::rotateBy( const qreal deltaLon, const qreal deltaLat, FlyToMode mode )
{
    d->rotateBy(deltaLon, deltaLat, mode);
}


void MarbleWidget::centerOn( const qreal lon, const qreal lat, bool animated )
{
    d->centerOn(lon, lat, animated);
}

void MarbleWidget::centerOn( const GeoDataCoordinates &position, bool animated )
{
    d->centerOn(position, animated);
}

void MarbleWidget::centerOn( const GeoDataLatLonBox &box, bool animated )
{
   d->centerOn(box, animated);
}

void MarbleWidget::centerOn( const GeoDataPlacemark& placemark, bool animated )
{
    d->centerOn(placemark, animated);
}

void MarbleWidget::setCenterLatitude( qreal lat, FlyToMode mode )
{
    d->setCenterLatitude(lat, mode);
}

void MarbleWidget::setCenterLongitude( qreal lon, FlyToMode mode )
{
    d->setCenterLongitude(lon, mode);
}

Projection MarbleWidget::projection() const
{
    return d->map()->projection();
}

void MarbleWidget::setProjection( Projection projection )
{
    d->map()->setProjection( projection );

    update();
}

void MarbleWidget::setProjection( int projection )
{
    setProjection( Projection( qAbs( projection ) % (Mercator+1) ) );
}

void MarbleWidget::moveLeft( FlyToMode mode )
{
    d->moveByStep( -1, 0, mode );
}

void MarbleWidget::moveRight( FlyToMode mode )
{
    d->moveByStep( 1, 0, mode );
}

void MarbleWidget::moveUp( FlyToMode mode )
{
    d->moveByStep( 0, -1, mode );
}

void MarbleWidget::moveDown( FlyToMode mode )
{
    d->moveByStep( 0, 1, mode );
}

void MarbleWidget::leaveEvent( QEvent* )
{
    emit mouseMoveGeoPosition( tr( NOT_AVAILABLE ) );
}

void MarbleWidget::resizeEvent( QResizeEvent *event )
{
    setUpdatesEnabled( false );
    d->map()->setSize( event->size() );
    setUpdatesEnabled( true );

    QWidget::resizeEvent( event );
}

#if QT_VERSION < 0x050000
void MarbleWidget::connectNotify( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL(mouseMoveGeoPosition(QString)) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( true );
}

void MarbleWidget::disconnectNotify( const char * signal )
{
    if ( QByteArray( signal ) == 
         QMetaObject::normalizedSignature ( SIGNAL(mouseMoveGeoPosition(QString)) ) )
        if ( d->m_inputhandler )
            d->m_inputhandler->setPositionSignalConnected( false );
}
#else
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
#endif

bool MarbleWidget::screenCoordinates( qreal lon, qreal lat,
                                      qreal& x, qreal& y ) const
{
    return d->map()->screenCoordinates( lon, lat, x, y );
}

bool MarbleWidget::geoCoordinates( int x, int y,
                                   qreal& lon, qreal& lat,
                                   GeoDataCoordinates::Unit unit ) const
{
    return d->map()->geoCoordinates( x, y, lon, lat, unit );
}

qreal MarbleWidget::centerLatitude() const
{
    return d->centerLatitude();
}

qreal MarbleWidget::centerLongitude() const
{
    return d->centerLongitude();
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
        QImage::Format imageFormat = ( d->map()->viewport()->mapCoversViewport() )
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
        GeoPainter geoPainter( paintDevice, d->map()->viewport(), d->map()->mapQuality() );

        d->map()->paint( geoPainter, evt->rect() );
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
    d->goHome(mode);
}

QString MarbleWidget::mapThemeId() const
{
    return d->model()->mapThemeId();
}

void MarbleWidget::setMapThemeId( const QString& mapThemeId )
{
    d->map()->setMapThemeId( mapThemeId );
}

void MarbleWidgetPrivate::updateMapTheme()
{
    map()->removeLayer( m_routingLayer );

    m_widget->setRadius( m_widget->radius() ); // Corrects zoom range, if needed

    if ( model()->planetId() == "earth" ) {
        map()->addLayer( m_routingLayer );
    }

    emit m_widget->themeChanged( map()->mapThemeId() );

    // Now we want a full repaint as the atmosphere might differ
    m_widget->setAttribute( Qt::WA_NoSystemBackground, false );

    m_widget->update();
}

GeoSceneDocument *MarbleWidget::mapTheme() const
{
    return d->model()->mapTheme();
}

void MarbleWidget::setPropertyValue( const QString& name, bool value )
{
    mDebug() << "In MarbleWidget the property " << name << "was set to " << value;
    d->map()->setPropertyValue( name, value );

    update();
}

void MarbleWidget::setShowOverviewMap( bool visible )
{
    d->map()->setShowOverviewMap( visible );

    update();
}

void MarbleWidget::setShowScaleBar( bool visible )
{
    d->map()->setShowScaleBar( visible );

    update();
}

void MarbleWidget::setShowCompass( bool visible )
{
    d->map()->setShowCompass( visible );

    update();
}

void MarbleWidget::setShowClouds( bool visible )
{
    d->map()->setShowClouds( visible );

    update();
}

void MarbleWidget::setShowSunShading( bool visible )
{
    d->map()->setShowSunShading( visible );

    update();
}

void MarbleWidget::setShowCityLights( bool visible )
{
    d->map()->setShowCityLights( visible );

    update();
}

void MarbleWidget::setLockToSubSolarPoint( bool visible )
{
    if ( d->map()->isLockedToSubSolarPoint() != visible ) { // Toggling input modifies event filters, so avoid that if not needed
        d->map()->setLockToSubSolarPoint( visible );
        setInputEnabled( !d->map()->isLockedToSubSolarPoint() );
    }
}

void MarbleWidget::setSubSolarPointIconVisible( bool visible )
{
    if ( d->map()->isSubSolarPointIconVisible() != visible ) {
        d->map()->setSubSolarPointIconVisible( visible );
    }

    QList<RenderPlugin *> pluginList = renderPlugins();
    QList<RenderPlugin *>::const_iterator i = pluginList.constBegin();
    QList<RenderPlugin *>::const_iterator const end = pluginList.constEnd();
    for (; i != end; ++i ) {
        if ( (*i)->nameId() == "sun" ) {
            (*i)->setVisible( visible );
        }
    }
}

void MarbleWidget::setShowAtmosphere( bool visible )
{
    d->map()->setShowAtmosphere( visible );

    update();
}

void MarbleWidget::setShowCrosshairs( bool visible )
{
    d->map()->setShowCrosshairs( visible );

    update();
}

void MarbleWidget::setShowGrid( bool visible )
{
    d->map()->setShowGrid( visible );

    update();
}

void MarbleWidget::setShowPlaces( bool visible )
{
    d->map()->setShowPlaces( visible );

    update();
}

void MarbleWidget::setShowCities( bool visible )
{
    d->map()->setShowCities( visible );

    update();
}

void MarbleWidget::setShowTerrain( bool visible )
{
    d->map()->setShowTerrain( visible );

    update();
}

void MarbleWidget::setShowOtherPlaces( bool visible )
{
    d->map()->setShowOtherPlaces( visible );

    update();
}

void MarbleWidget::setShowRelief( bool visible )
{
    d->map()->setShowRelief( visible );

    update();
}

void MarbleWidget::setShowIceLayer( bool visible )
{
    d->map()->setShowIceLayer( visible );

    update();
}

void MarbleWidget::setShowBorders( bool visible )
{
    d->map()->setShowBorders( visible );

    update();
}

void MarbleWidget::setShowRivers( bool visible )
{
    d->map()->setShowRivers( visible );

    update();
}

void MarbleWidget::setShowLakes( bool visible )
{
    d->map()->setShowLakes( visible );

    update();
}

void MarbleWidget::setShowFrameRate( bool visible )
{
    d->m_showFrameRate = visible;

    update();
}

void MarbleWidget::setShowBackground( bool visible )
{
    d->map()->setShowBackground( visible );

    update();
}

void MarbleWidget::setShowRuntimeTrace( bool visible )
{
    d->map()->setShowRuntimeTrace( visible );
}

void MarbleWidget::setShowTileId( bool visible )
{
    d->map()->setShowTileId( visible );
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
    d->map()->clearVolatileTileCache();
}

void MarbleWidget::setVolatileTileCacheLimit( quint64 kiloBytes )
{
    d->map()->setVolatileTileCacheLimit( kiloBytes );
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
    return d->map()->mapQuality( viewContext );
}

void MarbleWidget::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    const MapQuality oldQuality = d->map()->mapQuality();

    d->map()->setMapQualityForViewContext( quality, viewContext );

    if ( d->map()->mapQuality() != oldQuality )
        update();
}

ViewContext MarbleWidget::viewContext() const
{
    return d->viewContext();
}

void MarbleWidget::setViewContext( ViewContext viewContext )
{   //TODO - move to MarbleAbstractPresenter as soon as RoutingLayer is ported there, replace with pImpl call
    if ( d->map()->viewContext() != viewContext ) {
        const MapQuality oldQuality = d->map()->mapQuality();
        d->map()->setViewContext( viewContext );
        d->m_routingLayer->setViewContext( viewContext );

        if ( d->map()->mapQuality() != oldQuality )
            update();
    }
}

bool MarbleWidget::animationsEnabled() const
{
    return d->animationsEnabled();
}

void MarbleWidget::setAnimationsEnabled( bool enabled )
{
    d->setAnimationsEnabled(enabled);
}

AngleUnit MarbleWidget::defaultAngleUnit() const
{
    return d->map()->defaultAngleUnit();
}

void MarbleWidget::setDefaultAngleUnit( AngleUnit angleUnit )
{
    d->map()->setDefaultAngleUnit( angleUnit );
}

QFont MarbleWidget::defaultFont() const
{
    return d->map()->defaultFont();
}

void MarbleWidget::setDefaultFont( const QFont& font )
{
    d->map()->setDefaultFont( font );
}

void MarbleWidget::setSelection( const QRect& region )
{
    d->setSelection(region);
}

qreal MarbleWidget::distance() const
{
    return d->distance();
}

void MarbleWidget::setDistance( qreal newDistance )
{
    d->setDistance(newDistance);
}

QString MarbleWidget::distanceString() const
{
    return d->distanceString();
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
    return d->map()->renderPlugins();
}

void MarbleWidget::readPluginSettings( QSettings& settings )
{
    foreach( RenderPlugin *plugin, renderPlugins() ) {
        settings.beginGroup( QString( "plugin_" ) + plugin->nameId() );

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
        settings.beginGroup( QString( "plugin_" ) + plugin->nameId() );

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
    return d->map()->floatItems();
}

AbstractFloatItem * MarbleWidget::floatItem( const QString &nameId ) const
{
    return d->map()->floatItem( nameId );
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
    d->flyTo(newLookAt, mode);
}

void MarbleWidget::reloadMap()
{
    d->map()->reload();
}

void MarbleWidget::downloadRegion( QVector<TileCoordsPyramid> const & pyramid )
{
    d->map()->downloadRegion( pyramid );
}

GeoDataLookAt MarbleWidget::lookAt() const
{
    return d->lookAt();
}

GeoDataCoordinates MarbleWidget::focusPoint() const
{
    return d->map()->viewport()->focusPoint();
}

void MarbleWidget::setFocusPoint( const GeoDataCoordinates &focusPoint )
{
    d->map()->viewport()->setFocusPoint( focusPoint );
}

void MarbleWidget::resetFocusPoint()
{
    d->map()->viewport()->resetFocusPoint();
}

qreal MarbleWidget::radiusFromDistance( qreal distance ) const
{
    return d->radiusFromDistance(distance);
}

qreal MarbleWidget::distanceFromRadius( qreal radius ) const
{
    return d->distanceFromRadius(radius);
}

qreal MarbleWidget::zoomFromDistance( qreal distance ) const
{
    return d->zoomFromDistance(distance);
}

qreal MarbleWidget::distanceFromZoom( qreal zoom ) const
{
    return d->distanceFromZoom(zoom);
}

RoutingLayer* MarbleWidget::routingLayer()
{
    return d->m_routingLayer;
}

PopupLayer *MarbleWidget::popupLayer()
{
    return d->m_mapInfoDialog;
}

}

#include "MarbleWidget.moc"
