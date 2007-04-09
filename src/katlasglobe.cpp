#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#include "GeoPolygon.h"
#include "katlasglobe.h"
#include "texcolorizer.h"
#include "TileLoader.h"
#include "tilescissor.h"
#include "katlasdirs.h"
#include "katlastilecreatordialog.h"
#include "placemarkmanager.h"
#include "xmlhandler.h"

const float RAD2INT = 21600.0 / M_PI;


KAtlasGlobe::KAtlasGlobe( QWidget* parent )
    : m_parent(parent)
{
    texmapper = 0;

    m_placemarkpainter = 0;
    m_placecontainer   = 0;
    m_radius           = 2000;

    m_justModified = false;

    m_pPlanetAxis  = Quaternion( 1.0, 0.0, 0.0, 0.0 );

    m_coastimg = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
    m_maptheme = new MapTheme();


    QStringList  m_mapthemedirs = MapTheme::findMapThemes( "maps/earth/" );
    QString      selectedmap;

    if ( m_mapthemedirs.count() == 0 ) {
        qDebug() << "Couldn't find any maps! Exiting ...";
        exit(-1);
    }

    if ( m_mapthemedirs.count() >= 1 ) {
        QStringList  tmp = m_mapthemedirs.filter( "etopo2.dgml" );
        if ( tmp.count() >= 1 )
            selectedmap = tmp[0];
        else
            selectedmap = m_mapthemedirs[0];
    }
    setMapTheme( selectedmap );

    veccomposer  = new VectorComposer();
    gridmap      = new GridMap();
    texcolorizer = new TextureColorizer(KAtlasDirs::path("seacolors.leg"), KAtlasDirs::path("landcolors.leg"));

    placemarkmanager = new PlaceMarkManager();
    m_placecontainer = placemarkmanager->getPlaceContainer();

    m_placemarkmodel = new PlaceMarkModel( this );
    m_placemarkmodel->setContainer( m_placecontainer );
}

KAtlasGlobe::~KAtlasGlobe(){
	delete texmapper;
}


void KAtlasGlobe::setMapTheme( const QString& selectedmap )
{

    m_maptheme->open( KAtlasDirs::path( QString("maps/earth/%1")
                                        .arg( selectedmap ) ) );

    if ( !TileLoader::baseTilesAvailable( m_maptheme->tilePrefix() ) ) {
        qDebug("Base tiles not available. Creating Tiles ... ");

        KAtlasTileCreatorDialog tilecreatordlg( m_parent );
        tilecreatordlg.setSummary( m_maptheme->name(), m_maptheme->description() );

        TileScissor tilecreator( m_maptheme->prefix(),
                                 m_maptheme->installMap(), 
                                 m_maptheme->bitmaplayer().dem);
        QObject::connect( &tilecreator,    SIGNAL( progress( int ) ),
                          &tilecreatordlg, SLOT( setProgress( int ) ) );

        QTimer::singleShot( 0, &tilecreator, SLOT( createTiles() ) );

        tilecreatordlg.exec();
    }

    if ( texmapper == 0 )
        texmapper = new TextureMapper( "maps/earth/" + m_maptheme->tilePrefix() );
    else
        texmapper->setMap( "maps/earth/" + m_maptheme->tilePrefix() );

    texmapper->setMaxTileLevel( TileLoader::maxPartialTileLevel( m_maptheme->tilePrefix() ) );

    if ( m_placecontainer == 0)
        m_placecontainer = new PlaceContainer("placecontainer");

    m_placecontainer ->clearTextPixmaps();

    if ( m_placemarkpainter == 0)
        m_placemarkpainter = new PlaceMarkPainter( this );

    m_placemarkpainter->setLabelColor( m_maptheme->labelColor() );

    m_parent->update();

    m_justModified = true;
}


void KAtlasGlobe::resize()
{
    *m_coastimg = QImage( m_canvasimg->width(), m_canvasimg->height(),
                          QImage::Format_ARGB32_Premultiplied );
    m_canvasimg->fill( Qt::transparent );

    texmapper->resizeMap(m_canvasimg);
    veccomposer->resizeMap(m_coastimg);
    gridmap->resizeMap(m_coastimg);

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


void KAtlasGlobe::paintGlobe(ClipPainter* painter, QRect dirty)
{
    if ( needsUpdate() || m_canvasimg->isNull() || m_justModified == true ) {

        texmapper->mapTexture(m_canvasimg, m_radius, m_pPlanetAxis);

        if ( m_maptheme->bitmaplayer().dem == "true" ){
            m_coastimg->fill(Qt::transparent);

            // Create VectorMap
            veccomposer->drawTextureMap( m_coastimg, m_radius, m_pPlanetAxis );

            // Recolorize the heightmap using the VectorMap
            texcolorizer->colorize( m_canvasimg, m_coastimg, m_radius );
        }
    }

    // Paint Map on Widget
    painter->drawImage(dirty, *m_canvasimg, dirty); 

    if ( m_maptheme->vectorlayer().enabled == true ) {

        // Add further Vectors
        veccomposer->paintVectorMap(painter, m_radius, m_pPlanetAxis);
    }

    // if ( m_maptheme->vectorlayer().enabled == true ){
    QPen  gridpen( QColor( 255, 255, 255, 128 ) );

    gridmap->createGrid(m_radius,m_pPlanetAxis);

    gridmap->setPen( gridpen );
    gridmap->paintGridMap( painter, true );

    gridmap->createTropics( m_radius, m_pPlanetAxis );

    gridpen.setStyle( Qt::DotLine );
    gridmap->setPen( gridpen );
    gridmap->paintGridMap( painter, true );

    //	}
	
    if ( m_placecontainer->size() > 0 ) {
        m_placemarkpainter->paintPlaceFolder( painter, 
                                              m_canvasimg->width() / 2,
                                              m_canvasimg->height()/ 2,
                                              m_radius, m_placecontainer,
                                              m_pPlanetAxis );
    }

    m_pPlanetAxisUpdated = m_pPlanetAxis;
    m_radiusUpdated      = m_radius;
    m_justModified       = false;
}


void KAtlasGlobe::setCanvasImage(QImage* canvasimg)
{
    m_canvasimg = canvasimg;
}

void KAtlasGlobe::setRadius(const int& radius)
{
    // Clear canvas if the globe is visible as a whole or if the globe
    // does shrink.
    int  imgrx = ( m_canvasimg->width() ) >> 1;
    int  imgry = ( m_canvasimg->height() ) >> 1;

    if ( radius*radius < imgrx * imgrx + imgry * imgry
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


void KAtlasGlobe::rotateTo(const uint& phi, const uint& theta, const uint& psi)
{
    m_pPlanetAxis.createFromEuler( (float)(phi)   / RAD2INT,
                                   (float)(theta) / RAD2INT,
                                   (float)(psi)   / RAD2INT );
}

void KAtlasGlobe::rotateTo(const float& phi, const float& theta)
{
    m_pPlanetAxis.createFromEuler( (phi + 180.0) * M_PI / 180.0,
                                   (theta + 180.0) * M_PI / 180.0, 0.0 );
}


void KAtlasGlobe::rotateBy(const Quaternion& incRot)
{
    m_pPlanetAxis = incRot * m_pPlanetAxis;
}

void KAtlasGlobe::rotateBy(const float& phi, const float& theta)
{
    Quaternion  rotPhi( 1.0, phi, 0.0, 0.0 );
    Quaternion  rotTheta( 1.0, 0.0, theta, 0.0 );

    m_pPlanetAxis = rotTheta * m_pPlanetAxis;
    m_pPlanetAxis *= rotPhi;
    m_pPlanetAxis.normalize();
}

int KAtlasGlobe::northPoleY()
{
    
    Quaternion  northPole   = GeoPoint( 0.0f, (float)( -M_PI*0.5 ) ).quaternion();
    Quaternion  invPlanetAxis = m_pPlanetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( m_radius * northPole.v[Q_Y] );
}

int KAtlasGlobe::northPoleZ()
{
    Quaternion  northPole   = GeoPoint( 0.0f, (float)( -M_PI*0.5 ) ).quaternion();
    Quaternion  invPlanetAxis = m_pPlanetAxis.inverse();

    northPole.rotateAroundAxis(invPlanetAxis);

    return (int)( m_radius * northPole.v[Q_Z] );
}

bool KAtlasGlobe::screenCoordinates( const float lng, const float lat, 
                                     int& x, int& y )
{
    Quaternion  qpos       = GeoPoint( lng, lat ).quaternion();
    Quaternion  invRotAxis = m_pPlanetAxis.inverse();

    qpos.rotateAroundAxis(invRotAxis);

    x = (int)(  m_radius * qpos.v[Q_X] );
    y = (int)( -m_radius * qpos.v[Q_Y] );

    if ( qpos.v[Q_Z] >= 0.0 )
        return true;
    else 
        return false;
}

void KAtlasGlobe::addPlaceMarkFile( QString filename )
{
    placemarkmanager->loadKml( filename );

    m_placecontainer = placemarkmanager->getPlaceContainer();

    m_placemarkmodel->setContainer( m_placecontainer );	
}

QVector< PlaceMark* > KAtlasGlobe::whichFeatureAt( const QPoint& curpos )
{
    return m_placemarkpainter->whichPlaceMarkAt( curpos );
}
