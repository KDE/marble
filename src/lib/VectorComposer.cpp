//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2011      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "VectorComposer.h"

#include <cmath>

#include <QtGui/QColor>

#include "AbstractProjection.h"
#include "MarbleDebug.h"
#include "GeoPolygon.h"
#include "GeoPainter.h"
#include "global.h"
#include "VectorMap.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"

using namespace Marble;

QAtomicInt VectorComposer::refCounter( 0 );
PntMap *VectorComposer::s_coastLines( 0 );
PntMap *VectorComposer::s_islands( 0 );
PntMap *VectorComposer::s_lakeislands( 0 );
PntMap *VectorComposer::s_lakes( 0 );
PntMap *VectorComposer::s_glaciers( 0 );
PntMap *VectorComposer::s_rivers( 0 );
PntMap *VectorComposer::s_countries( 0 );
PntMap *VectorComposer::s_usaStates( 0 );
PntMap *VectorComposer::s_dateLine( 0 );
bool VectorComposer::s_coastLinesLoaded( false );
bool VectorComposer::s_overlaysLoaded( false );

VectorComposer::VectorComposer( QObject * parent )
    : QObject( parent ),
      m_vectorMap( new VectorMap() ),
      m_oceanPen( QPen( Qt::NoPen ) ),
      m_oceanBrush( QBrush( QColor( 153, 179, 204 ) ) ),
      m_landPen( QPen( Qt::NoPen ) ),
      m_landBrush( QBrush( QColor( 242, 239, 233 ) ) ),
      m_textureLandPen( QPen( Qt::NoPen ) ),
      m_textureLandBrush( QBrush( QColor( 255, 0, 0 ) ) ),
      m_textureGlacierBrush( QBrush( QColor( 0, 255, 0 ) ) ),
      m_textureLakeBrush( QBrush( QColor( 0, 0, 0 ) ) ),
      m_dateLineBrush( QBrush( Qt::NoBrush ) )
{
    if ( refCounter == 0 ) {
        s_coastLinesLoaded = false;
        s_overlaysLoaded = false;

        s_coastLines = new PntMap();
        s_islands = new PntMap();
        s_lakeislands = new PntMap();
        s_lakes = new PntMap();
        s_glaciers = new PntMap();
        s_rivers = new PntMap();
        s_countries = new PntMap();
        s_usaStates = new PntMap();
        s_dateLine = new PntMap();
    }
    refCounter.ref();

    m_textureBorderPen.setStyle( Qt::SolidLine );
    m_textureBorderPen.setColor( QColor( 0, 255, 0 ) );
    m_dateLinePen.setStyle( Qt::DashLine );
    m_dateLinePen.setColor( QColor( 0, 0, 0 ) );

    connect( s_coastLines, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_islands, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_lakeislands, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_lakes, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_glaciers, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_rivers, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_countries, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_usaStates, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
    connect( s_dateLine, SIGNAL( initialized() ), SIGNAL( datasetLoaded() ) );
}

VectorComposer::~VectorComposer()
{
    delete m_vectorMap;

    refCounter.deref();
    if (refCounter == 0) {
        delete s_dateLine;
        delete s_usaStates;    // The states of the USA
        delete s_countries;    // The country borders
        delete s_rivers;
        delete s_glaciers;
        delete s_lakes;
        delete s_lakeislands;
        delete s_islands;
        delete s_coastLines;

        s_coastLinesLoaded = false;
        s_overlaysLoaded = false;
    }
}

void VectorComposer::loadCoastlines()
{
    if ( s_coastLinesLoaded ) {
        return;
    }
    s_coastLinesLoaded = true;

    mDebug() << Q_FUNC_INFO;

    // Coastlines
    s_coastLines->load( MarbleDirs::path( "mwdbii/PCOAST.PNT" ) );
    s_islands->load( MarbleDirs::path( "mwdbii/PISLAND.PNT" ) );
    s_lakeislands->load( MarbleDirs::path( "mwdbii/PLAKEISLAND.PNT" ) );
    s_lakes->load( MarbleDirs::path( "mwdbii/PLAKE.PNT" ) );

    // Ice and snow ...
    s_glaciers->load( MarbleDirs::path( "mwdbii/PGLACIER.PNT" ) );
}

void VectorComposer::loadOverlays()
{
    if ( s_overlaysLoaded ) {
        return;
    }
    s_overlaysLoaded = true;

    mDebug() << Q_FUNC_INFO;

    // The rivers.
    s_rivers->load( MarbleDirs::path( "mwdbii/RIVER.PNT" ) );

    // The countries.
    s_countries->load( MarbleDirs::path( "mwdbii/PDIFFBORDER.PNT" ) );

    // The States of the USA.
    s_usaStates->load( MarbleDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) );

    // The date "line", which in reality is rather crooked.
    s_dateLine->load( MarbleDirs::path( "mwdbii/DATELINE.PNT" ) );
}

void VectorComposer::drawTextureMap(ViewParams *viewParams)
{
    loadCoastlines();

    QSharedPointer<QImage> origimg = viewParams->coastImagePtr();

    origimg->fill( Qt::transparent );

    bool doClip = false; //assume false
    switch( viewParams->projection() ) {
        case Spherical:
            doClip = ( viewParams->radius() > ( viewParams->width()  / 2 )
                       || viewParams->radius() > ( viewParams->height() / 2 ) );
            break;
        case Equirectangular:
            doClip = true; // clipping should always be enabled
            break;
        case Mercator:
            doClip = true; // clipping should always be enabled
            break;
    }

    const bool antialiased =    viewParams->mapQuality() == HighQuality
                             || viewParams->mapQuality() == PrintQuality;

    GeoPainter painter( origimg.data(), viewParams->viewport(), viewParams->mapQuality(), doClip );
    painter.setRenderHint( QPainter::Antialiasing, antialiased );

    // Coastlines
    m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific

    // Draw the coast line vectors
    m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
    painter.setPen( m_textureLandPen );
    painter.setBrush( m_textureLandBrush );
    m_vectorMap->drawMap( &painter );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );
    painter.setPen( m_textureLandPen );
    painter.setBrush( m_textureLandBrush );
    m_vectorMap->drawMap( &painter );

    bool showWaterbodies, showLakes;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "lakes", showLakes );

    if ( showWaterbodies && showLakes ) {
         // Lakes
         m_vectorMap->setzBoundingBoxLimit( 0.95 );
         m_vectorMap->setzPointLimit( 0.98 ); 

         m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
         painter.setPen( Qt::NoPen );
         painter.setBrush( m_textureLakeBrush );
         m_vectorMap->drawMap( &painter );

         m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
         painter.setPen( Qt::NoPen );
         painter.setBrush( m_textureLandBrush );
         m_vectorMap->drawMap( &painter );
    }

    bool showIce;
    viewParams->propertyValue( "ice", showIce );

    if ( showIce ) {
        // Glaciers
         m_vectorMap->setzBoundingBoxLimit( 0.8 );
         m_vectorMap->setzPointLimit( 0.9 );
         m_vectorMap->createFromPntMap( s_glaciers, viewParams->viewport() );
         painter.setPen( Qt::NoPen );
         painter.setBrush( m_textureGlacierBrush );

         m_vectorMap->drawMap( &painter );
    }

    // mDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
}

void VectorComposer::paintBaseVectorMap( GeoPainter *painter, 
                                         ViewParams *viewParams )
{
    loadCoastlines();

    const bool antialiased =    viewParams->mapQuality() == HighQuality
                             || viewParams->mapQuality() == PrintQuality;

    painter->setRenderHint( QPainter::Antialiasing, antialiased );

    // Paint the background of it all, i.e. the water.
    painter->setPen( m_oceanPen );
    painter->setBrush( m_oceanBrush );
    painter->drawPath( viewParams->currentProjection()->mapShape( viewParams->viewport() ) );

    // Coastlines
    m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific

    bool showCoastlines;
    viewParams->propertyValue( "coastlines", showCoastlines );

    if ( showCoastlines ) {
        painter->setPen( m_landPen );
        painter->setBrush( Qt::NoBrush );
    }
    else
    {
        painter->setPen( Qt::NoPen );
        painter->setBrush( m_landBrush );
    }

    m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
    m_vectorMap->paintMap( painter );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );

    if ( showCoastlines ) {
        painter->setPen( m_landPen );
        painter->setBrush( Qt::NoBrush );
    }
    else
    {
        painter->setPen( Qt::NoPen );
        painter->setBrush( m_landBrush );
    }

    m_vectorMap->paintMap( painter );

    bool showWaterbodies, showLakes;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "lakes", showLakes );

    if ( ( showWaterbodies && showLakes ) || showCoastlines ) {
         // Lakes
         m_vectorMap->setzBoundingBoxLimit( 0.95 );
         m_vectorMap->setzPointLimit( 0.98 ); 

         m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
         painter->setPen( m_lakePen );
         painter->setBrush( m_lakeBrush );
         m_vectorMap->paintMap( painter );

         m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
         painter->setBrush( m_landBrush );
         m_vectorMap->paintMap( painter );
    }
}

void VectorComposer::paintVectorMap( GeoPainter *painter,
                                     ViewParams *viewParams )
{
    // m_vectorMap->clearNodeCount();

    const bool antialiased =    viewParams->mapQuality() == HighQuality
                             || viewParams->mapQuality() == PrintQuality;

    painter->setRenderHint( QPainter::Antialiasing, antialiased );

    // Coastlines
    bool showCoastlines;
    viewParams->propertyValue( "coastlines", showCoastlines );

    if ( showCoastlines ) {

        loadCoastlines();

        m_vectorMap->setzBoundingBoxLimit( 0.4 );
        m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific
    
        m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
        painter->setPen( m_landPen );
        painter->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter );

        m_vectorMap->setzBoundingBoxLimit( 0.8 );
        m_vectorMap->setzPointLimit( 0.9 );

        m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );
        painter->setPen( m_landPen );
        painter->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter );

        // Lakes
        m_vectorMap->setzBoundingBoxLimit( 0.95 );
        m_vectorMap->setzPointLimit( 0.98 ); 

        m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
        painter->setPen( m_landPen );
        painter->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter );

        m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
        m_vectorMap->paintMap( painter );
    }

    bool showWaterbodies, showRivers;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "rivers", showRivers );

    if ( showWaterbodies && showRivers ) {
        loadOverlays();
        // Rivers
         m_vectorMap->setzBoundingBoxLimit( -1.0 );
         m_vectorMap->setzPointLimit( -1.0 );
         m_vectorMap->createFromPntMap( s_rivers, viewParams->viewport() );

         painter->setPen( m_riverPen );
         painter->setBrush( m_riverBrush );
         m_vectorMap->paintMap( painter );
    }

    bool showBorders;
    viewParams->propertyValue( "borders", showBorders );

    if ( showBorders ) {
        loadOverlays();
        // Countries
         m_vectorMap->setzBoundingBoxLimit( -1.0 );
         m_vectorMap->setzPointLimit( -1.0 );
         m_vectorMap->createFromPntMap( s_countries, viewParams->viewport() );

        // Fancy Boundaries Hack:
        // FIXME: Find a clean solution that allows for all the 
        // tuning necessary for the different quality levels.

        int radius = viewParams->radius();
        qreal penWidth = (double)(radius) / 400.0;
        if ( radius < 400.0 ) penWidth = 1.0;
        if ( radius > 800.0 ) penWidth = 1.75;
        if ( showCoastlines ) penWidth = 1.0;

        QPen countryPen( m_countryPen);
        countryPen.setWidthF( penWidth );
        QColor penColor = m_countryPen.color();

        QPen borderDashPen( Qt::black );
        painter->setBrush( m_countryBrush );

        if ( viewParams->mapQuality() == HighQuality 
          || viewParams->mapQuality() == PrintQuality ) {

            countryPen.setColor( penColor );
            painter->setPen( countryPen );
            m_vectorMap->paintMap( painter );

            // Only paint fancy style if the coast line doesn't get painted as well
            // (as it looks a bit awkward otherwise)

            if ( !showCoastlines ) {
                borderDashPen.setDashPattern( QVector<qreal>() << 1 << 5 );
                borderDashPen.setWidthF( penWidth * 0.5 );
                painter->setPen( borderDashPen );
                m_vectorMap->paintMap( painter );
            }
        }
        if ( viewParams->mapQuality() == OutlineQuality
          || viewParams->mapQuality() == LowQuality
          || viewParams->mapQuality() == NormalQuality ) {

            if ( !showCoastlines ) {
                countryPen.setWidthF( 1.0 );
                countryPen.setColor( penColor.darker(115) );
            }
            painter->setPen( countryPen );
            m_vectorMap->paintMap( painter );
        }

        // US-States
        m_vectorMap->setzBoundingBoxLimit( -1.0 );
        m_vectorMap->setzPointLimit( -1.0 );
        m_vectorMap->createFromPntMap( s_usaStates, viewParams->viewport() );

        QPen statePen( m_statePen);
        if ( viewParams->mapQuality() == OutlineQuality
          || viewParams->mapQuality() == LowQuality ) {
            statePen.setStyle( Qt::SolidLine );
        }
        painter->setPen( statePen );
        painter->setBrush( m_stateBrush );
        m_vectorMap->paintMap( painter );

        // International Dateline
        m_vectorMap->setzBoundingBoxLimit( -1.0 );
        m_vectorMap->setzPointLimit( -1.0 );
        m_vectorMap->createFromPntMap( s_dateLine, viewParams->viewport() );

        QPen dateLinePen( m_dateLinePen);
        if ( viewParams->mapQuality() == OutlineQuality
          || viewParams->mapQuality() == LowQuality ) {
            dateLinePen.setStyle( Qt::SolidLine );
        }
        painter->setPen( dateLinePen );
        painter->setBrush( m_dateLineBrush );
        m_vectorMap->paintMap( painter );
    }

    // mDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}

#include "VectorComposer.moc"
