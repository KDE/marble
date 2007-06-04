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

#include "GeoPolygon.h"
#include "texcolorizer.h"
#include "TileLoader.h"
#include "tilescissor.h"
#include "katlasdirs.h"
#include "katlastilecreatordialog.h"
#include "placemarkmanager.h"
#include "xmlhandler.h"

const float RAD2INT = 21600.0 / M_PI;


MarbleModel::MarbleModel( QWidget* parent )
    : m_parent(parent)
{
    m_texmapper = 0;

    m_placemarkpainter = 0;
    m_placecontainer   = 0;
    m_radius           = 2000;

    m_justModified = false;

    m_showGrid = true;
    m_showPlaceMarks = true;

    m_planetAxis   = Quaternion( 1.0, 0.0, 0.0, 0.0 );

    m_coastimg = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
    m_maptheme = new MapTheme();


    QStringList  mapthemedirs = MapTheme::findMapThemes( "maps/earth/" );
    QString      selectedmap;

    if ( mapthemedirs.count() == 0 ) {
        qDebug() << "Couldn't find any maps! Exiting ...";
        exit(-1);
    }

    if ( mapthemedirs.count() >= 1 ) {
        QStringList  tmp = mapthemedirs.filter( "etopo2.dgml" );
        if ( tmp.count() >= 1 )
            selectedmap = tmp[0];
        else
            selectedmap = mapthemedirs[0];
    }
    setMapTheme( selectedmap );

    m_veccomposer  = new VectorComposer();
    m_gridmap      = new GridMap();
    m_texcolorizer = new TextureColorizer( KAtlasDirs::path( "seacolors.leg" ), 
                                           KAtlasDirs::path( "landcolors.leg" ) );

    m_placemarkmanager = new PlaceMarkManager();
    m_placecontainer   = m_placemarkmanager->getPlaceContainer();

    m_placemarkmodel   = new PlaceMarkModel( this );
    m_placemarkmodel->setContainer( m_placecontainer );
}

MarbleModel::~MarbleModel()
{
    delete m_texmapper;
}


// Set a particular theme for the map, and load the top 3 tile levels.
// If these tiles aren't already created, then create them here and now. 

void MarbleModel::setMapTheme( const QString& selectedmap )
{

    m_maptheme->open( KAtlasDirs::path( QString("maps/earth/%1")
                                        .arg( selectedmap ) ) );

    // If the tiles aren't already there, put up a progress dialog
    // while creating them.
    if ( !TileLoader::baseTilesAvailable( m_maptheme->tilePrefix() ) ) {
        qDebug("Base tiles not available. Creating Tiles ... ");

#if 1
        KAtlasTileCreatorDialog tilecreatordlg( m_parent );
        tilecreatordlg.setSummary( m_maptheme->name(), 
                                   m_maptheme->description() );
#endif

        TileScissor tilecreator( m_maptheme->prefix(),
                                 m_maptheme->installMap(), 
                                 m_maptheme->bitmaplayer().dem);

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
        emit creatingTilesStart( m_maptheme->name(), 
                                 m_maptheme->description() );
        qDebug("After emitting creatingTilesStart() ... ");
#endif
    }

    if ( m_texmapper == 0 )
        m_texmapper = new TextureMapper( "maps/earth/"
                                         + m_maptheme->tilePrefix() );
    else
        m_texmapper->setMap( "maps/earth/" + m_maptheme->tilePrefix() );

    m_texmapper->setMaxTileLevel( TileLoader::maxPartialTileLevel( m_maptheme->tilePrefix() ) + 1 );

    if ( m_placecontainer == 0)
        m_placecontainer = new PlaceContainer("placecontainer");

    m_placecontainer ->clearTextPixmaps();

    if ( m_placemarkpainter == 0)
        m_placemarkpainter = new PlaceMarkPainter( this );

    m_placemarkpainter->setLabelColor( m_maptheme->labelColor() );

    emit themeChanged();

    m_justModified = true;
}


void MarbleModel::resize()
{
    *m_coastimg = QImage( m_canvasimg->width(), m_canvasimg->height(),
                          QImage::Format_ARGB32_Premultiplied );
    m_canvasimg->fill( Qt::transparent );

    m_texmapper->resizeMap( m_canvasimg );
    m_veccomposer->resizeMap( m_coastimg );
    m_gridmap->resizeMap( m_coastimg );

    QRadialGradient  grad1( QPointF( m_canvasimg->width()  / 2,
                                     m_canvasimg->height() / 2 ),
                            1.05 * m_radius );
    grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
    grad1.setColorAt( 1.0,  QColor( 255, 255, 255, 0 ) );

    QBrush    brush1( grad1 );
    QPainter  painter( m_canvasimg );
    painter.setBrush( brush1 );
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.drawEllipse( m_canvasimg->width() / 2 - (int)( (float)(m_radius) * 1.05 ),
                         m_canvasimg->height() / 2 - (int)( (float)(m_radius) * 1.05 ),
                         (int)( 2.1 * (float)(m_radius) ), 
                         (int)( 2.1 * (float)(m_radius) ) );

    m_justModified = true;
}


void MarbleModel::paintGlobe(ClipPainter* painter, const QRect& dirty)
{
    if ( needsUpdate() || m_canvasimg->isNull() || m_justModified == true ) {

        m_texmapper->mapTexture( m_canvasimg, m_radius, m_planetAxis );

        if ( m_maptheme->bitmaplayer().dem == "true" ){
            m_coastimg->fill(Qt::transparent);

            // Create VectorMap
            m_veccomposer->drawTextureMap( m_coastimg, m_radius, 
                                           m_planetAxis );

            // Recolorize the heightmap using the VectorMap
            m_texcolorizer->colorize( m_canvasimg, m_coastimg, m_radius );
        }
    }

    // Paint Map on Widget
    painter->drawImage( dirty, *m_canvasimg, dirty ); 

    if ( m_maptheme->vectorlayer().enabled == true ) {

        // Add further Vectors
        m_veccomposer->paintVectorMap( painter, m_radius, m_planetAxis );
    }

    // if ( m_maptheme->vectorlayer().enabled == true ){
    QPen  gridpen( QColor( 255, 255, 255, 128 ) );

    if ( m_showGrid == true )
    {
        m_gridmap->createGrid( m_radius, m_planetAxis );

        m_gridmap->setPen( gridpen );
        m_gridmap->paintGridMap( painter, true );

        m_gridmap->createTropics( m_radius, m_planetAxis );

        gridpen.setStyle( Qt::DotLine );
        m_gridmap->setPen( gridpen );
        m_gridmap->paintGridMap( painter, true );
    }

    //	}
	
    if ( m_showPlaceMarks == true && m_placecontainer->size() > 0 ) {
        m_placemarkpainter->paintPlaceFolder( painter, 
                                              m_canvasimg->width() / 2,
                                              m_canvasimg->height()/ 2,
                                              m_radius, m_placecontainer,
                                              m_planetAxis );
    }

    m_planetAxisUpdated = m_planetAxis;
    m_radiusUpdated     = m_radius;
    m_justModified      = false;
}


void MarbleModel::setCanvasImage(QImage* canvasimg)
{
    m_canvasimg = canvasimg;
}

void MarbleModel::setRadius(const int& radius)
{
    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    int  imgrx = ( m_canvasimg->width() ) >> 1;
    int  imgry = ( m_canvasimg->height() ) >> 1;

    if ( radius * radius < imgrx * imgrx + imgry * imgry
         && radius != m_radius )
    {
        m_canvasimg->fill( Qt::transparent );

        QRadialGradient grad1( QPointF( m_canvasimg->width() / 2,
                                        m_canvasimg->height() / 2 ),
                               1.05 * radius );
        grad1.setColorAt( 0.91, QColor( 255, 255, 255, 255 ) );
        grad1.setColorAt( 1.0,  QColor( 255, 255, 255, 0 ) );
        QBrush    brush1( grad1 );
        QPainter  painter( m_canvasimg );
        painter.setBrush( brush1 );
        painter.setRenderHint( QPainter::Antialiasing, true );
        painter.drawEllipse( m_canvasimg->width() / 2 - (int)( (float)(radius) * 1.05 ),
                             m_canvasimg->height() / 2 - (int)( (float)(radius) * 1.05 ),
                             (int)( 2.1 * (float)(radius) ), 
                             (int)( 2.1 * (float)(radius) ) );
    }

    m_radius = radius;
}


void MarbleModel::rotateTo(const uint& phi, const uint& theta, const uint& psi)
{
    m_planetAxis.createFromEuler( (float)(phi)   / RAD2INT,
                                  (float)(theta) / RAD2INT,
                                  (float)(psi)   / RAD2INT );
}

void MarbleModel::rotateTo(const float& phi, const float& theta)
{
    m_planetAxis.createFromEuler( (phi + 180.0) * M_PI / 180.0,
                                  (theta + 180.0) * M_PI / 180.0, 0.0 );
}


void MarbleModel::rotateBy(const Quaternion& incRot)
{
    m_planetAxis = incRot * m_planetAxis;
}

void MarbleModel::rotateBy(const float& phi, const float& theta)
{
    Quaternion  rotPhi( 1.0, phi, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, theta, 0.0 );

    m_planetAxis = rotTheta * m_planetAxis;
    m_planetAxis *= rotPhi;
    m_planetAxis.normalize();
}

int MarbleModel::northPoleY()
{
    
    Quaternion  northPole   = GeoPoint( 0.0f, (float)( -M_PI*0.5 ) ).quaternion();
    Quaternion  invPlanetAxis = m_planetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( m_radius * northPole.v[Q_Y] );
}

int MarbleModel::northPoleZ()
{
    Quaternion  northPole   = GeoPoint( 0.0f, (float)( -M_PI*0.5 ) ).quaternion();
    Quaternion  invPlanetAxis = m_planetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( m_radius * northPole.v[Q_Z] );
}

bool MarbleModel::screenCoordinates( const float lng, const float lat, 
                                     int& x, int& y )
{
    Quaternion  qpos       = GeoPoint( lng, lat ).quaternion();
    Quaternion  invRotAxis = m_planetAxis.inverse();

    qpos.rotateAroundAxis(invRotAxis);

    x = (int)(  m_radius * qpos.v[Q_X] );
    y = (int)( -m_radius * qpos.v[Q_Y] );

    if ( qpos.v[Q_Z] >= 0.0 )
        return true;
    else 
        return false;
}

void MarbleModel::addPlaceMarkFile( const QString& filename )
{
    m_placemarkmanager->loadKml( filename );

    m_placecontainer = m_placemarkmanager->getPlaceContainer();

    m_placemarkmodel->setContainer( m_placecontainer );	
}

QVector< PlaceMark* > MarbleModel::whichFeatureAt( const QPoint& curpos )
{
    return m_placemarkpainter->whichPlaceMarkAt( curpos );
}

#ifndef Q_OS_MACX
#include "MarbleModel.moc"
#endif
