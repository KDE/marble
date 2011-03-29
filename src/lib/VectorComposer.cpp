//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "VectorComposer.h"

#include <cmath>

#include <QtGui/QColor>

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

    QImage        *origimg = viewParams->coastImage();

    origimg->fill( Qt::transparent );

    //	m_vectorMap->clearNodeCount();

    bool antialiased = false;

    if (   viewParams->mapQuality() == HighQuality
        || viewParams->mapQuality() == PrintQuality )
    {
        antialiased = true;
    }

    // Coastlines
    m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific

    // Draw the coast line vectors
    m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
    m_vectorMap->setPen( m_textureLandPen );
    m_vectorMap->setBrush( m_textureLandBrush );
    m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                          viewParams->mapQuality() );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );
    m_vectorMap->setPen( m_textureLandPen );
    m_vectorMap->setBrush( m_textureLandBrush );
    m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                          viewParams->mapQuality() );

    bool showWaterbodies, showLakes;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "lakes", showLakes );

    if ( showWaterbodies && showLakes ) {
         // Lakes
         m_vectorMap->setzBoundingBoxLimit( 0.95 );
         m_vectorMap->setzPointLimit( 0.98 ); 

         m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
         m_vectorMap->setPen( Qt::NoPen );
         m_vectorMap->setBrush( m_textureLakeBrush );
         m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                               viewParams->mapQuality() );

         m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
         m_vectorMap->setPen( Qt::NoPen );
         m_vectorMap->setBrush( m_textureLandBrush );
         m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                               viewParams->mapQuality() );
    }

    bool showIce;
    viewParams->propertyValue( "ice", showIce );

    if ( showIce ) {
        // Glaciers
         m_vectorMap->setzBoundingBoxLimit( 0.8 );
         m_vectorMap->setzPointLimit( 0.9 );
         m_vectorMap->createFromPntMap( s_glaciers, viewParams->viewport() );
         m_vectorMap->setPen( Qt::NoPen );
         m_vectorMap->setBrush( m_textureGlacierBrush );

         m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                               viewParams->mapQuality() );
    }

    // mDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
}

void VectorComposer::paintBaseVectorMap( GeoPainter *painter, 
                                         ViewParams *viewParams )
{
    loadCoastlines();

    bool antialiased = false;

    if (   viewParams->mapQuality() == HighQuality
        || viewParams->mapQuality() == PrintQuality )
    {
	antialiased = true;
    }

    // Paint the background of it all, i.e. the water.
    m_vectorMap->setPen( m_oceanPen );
    m_vectorMap->setBrush( m_oceanBrush );
    m_vectorMap->paintBase( painter, viewParams->viewport(), antialiased );

    // Coastlines
    m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific

    bool showCoastlines;
    viewParams->propertyValue( "coastlines", showCoastlines );

    if ( showCoastlines ) {
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
    }
    else
    {
        m_vectorMap->setPen( Qt::NoPen );
        m_vectorMap->setBrush( m_landBrush );
    }

    m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
    m_vectorMap->paintMap( painter, antialiased );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );

    if ( showCoastlines ) {
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
    }
    else
    {
        m_vectorMap->setPen( Qt::NoPen );
        m_vectorMap->setBrush( m_landBrush );
    }

    m_vectorMap->paintMap( painter, antialiased );

    bool showWaterbodies, showLakes;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "lakes", showLakes );

    if ( ( showWaterbodies && showLakes ) || showCoastlines ) {
         // Lakes
         m_vectorMap->setzBoundingBoxLimit( 0.95 );
         m_vectorMap->setzPointLimit( 0.98 ); 

         m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
         m_vectorMap->setPen( m_lakePen );
         m_vectorMap->setBrush( m_lakeBrush );
         m_vectorMap->paintMap( painter, antialiased );

         m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
         m_vectorMap->setBrush( m_landBrush );
         m_vectorMap->paintMap( painter, antialiased );
    }
}

void VectorComposer::paintVectorMap( GeoPainter *painter,
                                     ViewParams *viewParams )
{
    // m_vectorMap->clearNodeCount();

    bool antialiased = false;

    if (   viewParams->mapQuality() == HighQuality
        || viewParams->mapQuality() == PrintQuality )
    {
        antialiased = true;
    }

    // Coastlines
    bool showCoastlines;
    viewParams->propertyValue( "coastlines", showCoastlines );

    if ( showCoastlines ) {

        loadCoastlines();

        m_vectorMap->setzBoundingBoxLimit( 0.4 );
        m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific
    
        m_vectorMap->createFromPntMap( s_coastLines, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        m_vectorMap->setzBoundingBoxLimit( 0.8 );
        m_vectorMap->setzPointLimit( 0.9 );

        m_vectorMap->createFromPntMap( s_islands, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        // Lakes
        m_vectorMap->setzBoundingBoxLimit( 0.95 );
        m_vectorMap->setzPointLimit( 0.98 ); 

        m_vectorMap->createFromPntMap( s_lakes, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        m_vectorMap->createFromPntMap( s_lakeislands, viewParams->viewport() );
        m_vectorMap->paintMap( painter, antialiased );
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

         m_vectorMap->setPen( m_riverPen );
         m_vectorMap->setBrush( m_riverBrush );
         m_vectorMap->paintMap( painter, antialiased );
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
        m_vectorMap->setBrush( m_countryBrush );

        if ( viewParams->mapQuality() == HighQuality 
          || viewParams->mapQuality() == PrintQuality ) {

            countryPen.setColor( penColor );
            m_vectorMap->setPen( countryPen );
            m_vectorMap->paintMap( painter, antialiased );

            // Only paint fancy style if the coast line doesn't get painted as well
            // (as it looks a bit awkward otherwise)

            if ( !showCoastlines ) {
                borderDashPen.setDashPattern( QVector<qreal>() << 1 << 5 );
                borderDashPen.setWidthF( penWidth * 0.5 );
                m_vectorMap->setPen( borderDashPen );
                m_vectorMap->paintMap( painter, antialiased );
            }
        }
        if ( viewParams->mapQuality() == OutlineQuality
          || viewParams->mapQuality() == LowQuality
          || viewParams->mapQuality() == NormalQuality ) {

            if ( !showCoastlines ) {
                countryPen.setWidthF( 1.0 );
                countryPen.setColor( penColor.darker(115) );
            }
            m_vectorMap->setPen( countryPen );
            m_vectorMap->paintMap( painter, antialiased );
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
        m_vectorMap->setPen( statePen );
        m_vectorMap->setBrush( m_stateBrush );
        m_vectorMap->paintMap( painter, antialiased );

        // International Dateline
        m_vectorMap->setzBoundingBoxLimit( -1.0 );
        m_vectorMap->setzPointLimit( -1.0 );
        m_vectorMap->createFromPntMap( s_dateLine, viewParams->viewport() );

        QPen dateLinePen( m_dateLinePen);
        if ( viewParams->mapQuality() == OutlineQuality
          || viewParams->mapQuality() == LowQuality ) {
            dateLinePen.setStyle( Qt::SolidLine );
        }
        m_vectorMap->setPen( dateLinePen );
        m_vectorMap->setBrush( m_dateLineBrush );
        m_vectorMap->paintMap( painter, antialiased );
    }

    // mDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}

#include "VectorComposer.moc"
