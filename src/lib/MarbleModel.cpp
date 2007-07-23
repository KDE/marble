//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "MarbleModel.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#include "ClipPainter.h"
#include "GeoPolygon.h"
#include "ViewParams.h"
#include "texcolorizer.h"
#include "TileLoader.h"
#include "tilescissor.h"
#include "katlasdirs.h"
#include "katlastilecreatordialog.h"
#include "placemarkmanager.h"
#include "xmlhandler.h"
#include "gps/GpsLayer.h"
#include "utils.h"

class MarbleModelPrivate
{
 public:
    // View and paint stuff
    MapTheme            *m_maptheme;
    TextureColorizer    *m_texcolorizer;
#ifndef FLAT_PROJ
    GlobeScanlineTextureMapper  *m_texmapper;
#else
    FlatScanlineTextureMapper   *m_texmapper;
#endif

    VectorComposer      *m_veccomposer;
    GridMap             *m_gridmap;

    // Places on the map
    PlaceMarkManager    *m_placemarkmanager;
    PlaceMarkContainer  *m_placeMarkContainer;
    PlaceMarkModel      *m_placemarkmodel;
    PlaceMarkPainter    *m_placemarkpainter;

    //Gps Stuff
    GpsLayer            *m_gpsLayer;

    QTimer       *m_timer;
};



MarbleModel::MarbleModel( QWidget *parent )
    : d( new MarbleModelPrivate )
{
    d->m_timer = new QTimer( this );
    d->m_timer->start( 1000 );

    connect( d->m_timer, SIGNAL( timeout() ),
             this,       SIGNAL( timeout() ) );

    d->m_texmapper = 0;

    d->m_placemarkpainter   = 0;
    d->m_placeMarkContainer = 0;

    d->m_maptheme = new MapTheme();


    QStringList  mapthemedirs = MapTheme::findMapThemes( "maps/earth/" );
    QString      selectedmap;

    if ( mapthemedirs.count() == 0 ) {
        qDebug() << "Couldn't find any maps! Exiting ...";
        exit(-1);
    }

    if ( mapthemedirs.count() >= 1 ) {
        QStringList  tmp = mapthemedirs.filter( "srtm.dgml" );
        if ( tmp.count() >= 1 )
            selectedmap = tmp[0];
        else
            selectedmap = mapthemedirs[0];
    }
    setMapTheme( selectedmap, parent );

    d->m_veccomposer  = new VectorComposer();
    d->m_gridmap      = new GridMap();
    d->m_texcolorizer = new TextureColorizer( KAtlasDirs::path( "seacolors.leg" ),
                                              KAtlasDirs::path( "landcolors.leg" ) );

    d->m_placemarkmanager   = new PlaceMarkManager();
    d->m_placeMarkContainer = d->m_placemarkmanager->getPlaceMarkContainer();

    d->m_placeMarkContainer ->clearTextPixmaps();

    d->m_placemarkmodel   = new PlaceMarkModel( this );
    d->m_placemarkmodel->setContainer( d->m_placeMarkContainer );

    d->m_gpsLayer = new GpsLayer();
}

MarbleModel::~MarbleModel()
{
    delete d->m_placeMarkContainer;
    delete d->m_texmapper;
    delete d;
}


bool MarbleModel::showGps() const
{
    return d->m_gpsLayer->visible();
}

void MarbleModel::setShowGps( bool visible )
{
    d->m_gpsLayer->setVisible( visible );
}


// Set a particular theme for the map, and load the top 3 tile levels.
// If these tiles aren't already created, then create them here and now.

void MarbleModel::setMapTheme( const QString &selectedMap, QWidget *parent )
{
    d->m_maptheme->open( KAtlasDirs::path( QString("maps/earth/%1")
                                           .arg( selectedMap ) ) );

    // If the tiles aren't already there, put up a progress dialog
    // while creating them.
    if ( !TileLoader::baseTilesAvailable( "maps/earth/" + d->m_maptheme->tilePrefix() ) ) {
        qDebug("Base tiles not available. Creating Tiles ... ");

#if 1
        KAtlasTileCreatorDialog tilecreatordlg( parent );
        tilecreatordlg.setSummary( d->m_maptheme->name(),
                                   d->m_maptheme->description() );
#endif

        TileScissor tilecreator( d->m_maptheme->prefix(),
                                 d->m_maptheme->installMap(),
                                 d->m_maptheme->bitmaplayer().dem);

        // This timer is necessary, because if we remove it, the GUI
        // never gets shown before the work starts.
        QTimer::singleShot( 0, &tilecreator, SLOT( createTiles() ) );
#if 1
        connect( &tilecreator,    SIGNAL( progress( int ) ),
                 &tilecreatordlg, SLOT( setProgress( int ) ) );

        tilecreatordlg.exec();
#else

        connect( &tilecreator, SIGNAL( progress( int ) ),
                 this,         SIGNAL( creatingTilesProgress( int ) ) );
        qDebug("Before emitting creatingTilesStart() ... ");
        emit creatingTilesStart( d->m_maptheme->name(),
                                 d->m_maptheme->description() );
        qDebug("After emitting creatingTilesStart() ... ");
#endif
    }
#ifndef FLAT_PROJ
    if ( d->m_texmapper == 0 )
        d->m_texmapper = new GlobeScanlineTextureMapper( "maps/earth/"
                                            + d->m_maptheme->tilePrefix(), this );
#else
    if ( d->m_texmapper == 0 )
        d->m_texmapper = new FlatScanlineTextureMapper( "maps/earth/"
                                            + d->m_maptheme->tilePrefix() );
#endif
    else
        d->m_texmapper->setMapTheme( "maps/earth/" + d->m_maptheme->tilePrefix() );

    connect( d->m_texmapper, SIGNAL( mapChanged() ),
             this,           SLOT( notifyModelChanged() ) );

    if ( d->m_placeMarkContainer == 0)
        d->m_placeMarkContainer = new PlaceMarkContainer("placecontainer");

    placeMarkContainer()->clearTextPixmaps();

    if ( d->m_placemarkpainter == 0)
        d->m_placemarkpainter = new PlaceMarkPainter( this );

    d->m_placemarkpainter->setLabelColor( d->m_maptheme->labelColor() );

    notifyModelChanged();
}


void MarbleModel::resize( int width, int height )
{
    d->m_texmapper->resizeMap( width, height );
    d->m_veccomposer->resizeMap( width, height );
    d->m_gridmap->resizeMap( width, height );
}


void MarbleModel::paintGlobe( ClipPainter* painter,
                              int width, int height,
                              ViewParams *viewParams,
                              bool redrawBackground,
                              const QRect& dirtyRect )
{
    resize( width, height );

    if ( redrawBackground ) {

        d->m_texmapper->mapTexture( viewParams->m_canvasImage,
                                    viewParams->m_radius,
                                    viewParams->m_planetAxis );

        if ( !viewParams->m_showElevationModel
             && d->m_maptheme->bitmaplayer().dem == "true" )
        {
            viewParams->m_coastImage->fill( Qt::transparent );

            // Create VectorMap
            d->m_veccomposer->drawTextureMap( viewParams->m_coastImage,
                                              viewParams->m_radius,
                                              viewParams->m_planetAxis );

            // Recolorize the heightmap using the VectorMap
            d->m_texcolorizer->colorize( viewParams->m_canvasImage,
                                         viewParams->m_coastImage,
                                         viewParams->m_radius,
                                         viewParams->m_planetAxis );
        }
    }

    // Paint the map on the Widget
//    QTime t;
//    t.start();

    painter->drawImage( dirtyRect, *viewParams->m_canvasImage, dirtyRect );

//    qDebug( "Painted in %ims", t.elapsed() );

    // Paint the vector layer.
    if ( d->m_maptheme->vectorlayer().enabled == true ) {

        // Add further Vectors
        d->m_veccomposer->paintVectorMap( painter,
                                          viewParams->m_radius,
                                          viewParams->m_planetAxis );
    }

    // Paint the grid around the earth.
    if ( viewParams->m_showGrid ) {
        QPen  gridpen( QColor( 255, 255, 255, 128 ) );

        // Create and paint a grid
        d->m_gridmap->createGrid( viewParams->m_radius,
                                  viewParams->m_planetAxis );
        d->m_gridmap->setPen( gridpen );
        d->m_gridmap->paintGridMap( painter, true );

        // Create and paint the tropics and polar circles
        d->m_gridmap->createTropics( viewParams->m_radius,
                                     viewParams->m_planetAxis );
        gridpen.setStyle( Qt::DotLine );
        gridpen.setWidthF( 1.5f );
        d->m_gridmap->setPen( gridpen );
        d->m_gridmap->paintGridMap( painter, true );

        // Create Equator
        d->m_gridmap->createEquator( viewParams->m_radius,
                                     viewParams->m_planetAxis );
        gridpen.setWidthF( 2.0f );
        d->m_gridmap->setPen( gridpen );
        d->m_gridmap->paintGridMap( painter, true );
    }

    // Paint the PlaceMark layer
#ifndef KML_GSOC
    if ( viewParams->m_showPlaceMarks && d->m_placeMarkContainer->size() > 0 ) {
        d->m_placemarkpainter->paintPlaceFolder( painter,
                                                 viewParams->m_canvasImage->width(),
                                                 viewParams->m_canvasImage->height(),
                                                 viewParams->m_radius,
                                                 d->m_placeMarkContainer,
                                                 viewParams->m_planetAxis );
    }
#else
    if ( viewParams->m_showPlaceMarks ) {
        d->m_placemarkpainter->paintPlaceFolder( painter,
                                                 viewParams->m_canvasImage->width(),
                                                 viewParams->m_canvasImage->height(),
                                                 viewParams->m_radius,
                                                 d->m_placemarkmanager->getPlaceMarkContainer(),
                                                 viewParams->m_planetAxis );
    }
#endif

    // Paint the Gps Layer
    if ( d->m_gpsLayer->visible() ) {
        d->m_gpsLayer->paintLayer( painter, viewParams->m_canvasImage->size(),
                                   viewParams->m_radius,
                                   viewParams->m_planetAxis );
    }
}


QAbstractListModel *MarbleModel::getPlaceMarkModel() const
{
    return d->m_placemarkmodel;
}


PlaceMarkContainer *MarbleModel::placeMarkContainer()   const
{
    return d->m_placeMarkContainer;
}

VectorComposer    *MarbleModel::vectorComposer()   const
{
    return d->m_veccomposer;
}

TextureColorizer  *MarbleModel::textureColorizer() const
{
    return d->m_texcolorizer;
}
#ifndef FLAT_PROJ
GlobeScanlineTextureMapper
#else
FlatScanlineTextureMapper
#endif
    *MarbleModel::textureMapper()    const
{
    return d->m_texmapper;
}

PlaceMarkPainter *MarbleModel::placeMarkPainter() const
{
    return d->m_placemarkpainter;
}

GpsLayer *MarbleModel::gpsLayer()         const
{
    return d->m_gpsLayer;
}

void MarbleModel::addPlaceMarkFile( const QString& filename )
{
    d->m_placemarkmanager->loadKml( filename );

    d->m_placeMarkContainer = d->m_placemarkmanager->getPlaceMarkContainer();

    d->m_placemarkmodel->setContainer( d->m_placeMarkContainer );
}

QVector< PlaceMark* > MarbleModel::whichFeatureAt( const QPoint& curpos )
{
    return d->m_placemarkpainter->whichPlaceMarkAt( curpos );
}

void MarbleModel::notifyModelChanged()
{
    emit modelChanged();
}


#include "MarbleModel.moc"
