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
#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include "GeoPolygon.h"
#include "GeoPainter.h"
#include "global.h"
#include "VectorMap.h"
#include "ViewParams.h"
#include "ViewportParams.h"
#include "MarbleDirs.h"

using namespace Marble;

VectorComposer::VectorComposer()
    : m_vectorMap( new VectorMap() ),
      m_coastLines( new PntMap() ),
      m_islands( new PntMap() ),
      m_lakeislands( new PntMap() ),
      m_lakes( new PntMap() ),
      m_glaciers( new PntMap() ),
      m_rivers( new PntMap() ),
      m_countries( new PntMap() ),
      m_usaStates( new PntMap() ),
      m_dateLine( new PntMap() ),
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
    m_textureBorderPen.setStyle( Qt::SolidLine );
    m_textureBorderPen.setColor( QColor( 0, 255, 0 ) );
    m_dateLinePen.setStyle( Qt::DashLine );
    m_dateLinePen.setColor( QColor( 0, 0, 0 ) );

    loadCoastlines();
    loadOverlay();
}

VectorComposer::~VectorComposer()
{
    delete m_dateLine;
    delete m_usaStates;    // The states of the USA
    delete m_countries;    // The country borders
    delete m_rivers;
    delete m_glaciers;
    delete m_lakes;
    delete m_lakeislands;
    delete m_islands;
    delete m_coastLines;
    delete m_vectorMap;
}

void VectorComposer::loadCoastlines()
{
    m_coastLines->load( MarbleDirs::path( "mwdbii/PCOAST.PNT" ) );
    m_islands->load( MarbleDirs::path( "mwdbii/PISLAND.PNT" ) );
    m_lakeislands->load( MarbleDirs::path( "mwdbii/PLAKEISLAND.PNT" ) );
    m_lakes->load( MarbleDirs::path( "mwdbii/PLAKE.PNT" ) );
}

void VectorComposer::loadOverlay()
{
    // Ice and snow ...
    m_glaciers->load( MarbleDirs::path( "mwdbii/PGLACIER.PNT" ) );

    // The rivers.
    m_rivers->load( MarbleDirs::path( "mwdbii/RIVER.PNT" ) );

    // The countries.
    m_countries->load( MarbleDirs::path( "mwdbii/PDIFFBORDER.PNT" ) );

    // The States of the USA.
    m_usaStates->load( MarbleDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) );

    // The date "line", which in reality is rather crooked.
    m_dateLine->load( MarbleDirs::path( "mwdbii/DATELINE.PNT" ) );
}

void VectorComposer::drawTextureMap(ViewParams *viewParams)
{
    QPaintDevice  *origimg = viewParams->coastImage();
    Quaternion     rotAxis = viewParams->planetAxis();

    //	m_vectorMap->clearNodeCount();

    bool antialiased = false;

    if (   viewParams->mapQuality() == Marble::HighQuality
        || viewParams->mapQuality() == Marble::PrintQuality )
    {
        antialiased = true;
    }

    // Coastlines
    m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific

    // Draw the coast line vectors
    m_vectorMap->createFromPntMap( m_coastLines, viewParams->viewport() );
    m_vectorMap->setPen( m_textureLandPen );
    m_vectorMap->setBrush( m_textureLandBrush );
    m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                          viewParams->mapQuality() );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( m_islands, viewParams->viewport() );
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

         m_vectorMap->createFromPntMap( m_lakes, viewParams->viewport() );
         m_vectorMap->setPen( Qt::NoPen );
         m_vectorMap->setBrush( m_textureLakeBrush );
         m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                               viewParams->mapQuality() );

         m_vectorMap->createFromPntMap( m_lakeislands, viewParams->viewport() );
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
         m_vectorMap->createFromPntMap( m_glaciers, viewParams->viewport() );
         m_vectorMap->setPen( Qt::NoPen );
         m_vectorMap->setBrush( m_textureGlacierBrush );

         m_vectorMap->drawMap( origimg, antialiased, viewParams->viewport(),
                               viewParams->mapQuality() );
    }

    // qDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
}

void VectorComposer::paintBaseVectorMap( GeoPainter *painter, 
                                         ViewParams *viewParams )
{
    Quaternion  rotAxis = viewParams->planetAxis();

    bool antialiased = false;

    if (   viewParams->mapQuality() == Marble::HighQuality
        || viewParams->mapQuality() == Marble::PrintQuality )
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

    m_vectorMap->createFromPntMap( m_coastLines, viewParams->viewport() );
    m_vectorMap->paintMap( painter, antialiased );

    // Islands
    m_vectorMap->setzBoundingBoxLimit( 0.8 );
    m_vectorMap->setzPointLimit( 0.9 );

    m_vectorMap->createFromPntMap( m_islands, viewParams->viewport() );

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

         m_vectorMap->createFromPntMap( m_lakes, viewParams->viewport() );
         m_vectorMap->setPen( m_lakePen );
         m_vectorMap->setBrush( m_lakeBrush );
         m_vectorMap->paintMap( painter, antialiased );

         m_vectorMap->createFromPntMap( m_lakeislands, viewParams->viewport() );
         m_vectorMap->setBrush( m_landBrush );
         m_vectorMap->paintMap( painter, antialiased );
    }
}

void VectorComposer::paintVectorMap( GeoPainter *painter,
                                     ViewParams *viewParams )
{
    // m_vectorMap->clearNodeCount();

    Quaternion  rotAxis = viewParams->planetAxis();

    bool antialiased = false;

    if (   viewParams->mapQuality() == Marble::HighQuality
        || viewParams->mapQuality() == Marble::PrintQuality )
    {
        antialiased = true;
    }

    // Coastlines
    bool showCoastlines;
    viewParams->propertyValue( "coastlines", showCoastlines );

    if ( showCoastlines ) {
        m_vectorMap->setzBoundingBoxLimit( 0.4 ); 
        m_vectorMap->setzPointLimit( 0 ); // 0.6 results in green pacific
    
        m_vectorMap->createFromPntMap( m_coastLines, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        m_vectorMap->setzBoundingBoxLimit( 0.8 );
        m_vectorMap->setzPointLimit( 0.9 );

        m_vectorMap->createFromPntMap( m_islands, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        // Lakes
        m_vectorMap->setzBoundingBoxLimit( 0.95 );
        m_vectorMap->setzPointLimit( 0.98 ); 

        m_vectorMap->createFromPntMap( m_lakes, viewParams->viewport() );
        m_vectorMap->setPen( m_landPen );
        m_vectorMap->setBrush( Qt::NoBrush );
        m_vectorMap->paintMap( painter, antialiased );

        m_vectorMap->createFromPntMap( m_lakeislands, viewParams->viewport() );
        m_vectorMap->paintMap( painter, antialiased );
    }

    bool showWaterbodies, showRivers;
    viewParams->propertyValue( "waterbodies", showWaterbodies );
    viewParams->propertyValue( "rivers", showRivers );

    if ( showWaterbodies && showRivers ) {
        // Rivers
         m_vectorMap->setzBoundingBoxLimit( -1.0 );
         m_vectorMap->setzPointLimit( -1.0 );
         m_vectorMap->createFromPntMap( m_rivers, viewParams->viewport() );

         m_vectorMap->setPen( m_riverPen );
         m_vectorMap->setBrush( m_riverBrush );
         m_vectorMap->paintMap( painter, antialiased );
    }

    bool showBorders;
    viewParams->propertyValue( "borders", showBorders );

    if ( showBorders ) {
        // Countries
         m_vectorMap->setzBoundingBoxLimit( -1.0 );
         m_vectorMap->setzPointLimit( -1.0 );
         m_vectorMap->createFromPntMap( m_countries, viewParams->viewport() );

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

        if ( viewParams->mapQuality() == Marble::HighQuality 
          || viewParams->mapQuality() == Marble::PrintQuality ) {

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
        if ( viewParams->mapQuality() == Marble::NormalQuality ) {

            // Only paint fancy style if the coast line doesn't get painted as well
            // (as it looks a bit awkward otherwise)

            if ( !showCoastlines ) {
                countryPen.setColor( penColor.darker(110) );
            }

            m_vectorMap->setPen( countryPen );
            m_vectorMap->paintMap( painter, antialiased );

            if ( !showCoastlines ) {
                borderDashPen.setStyle( Qt::DotLine );
                m_vectorMap->setPen( borderDashPen );
                m_vectorMap->paintMap( painter, antialiased );
            }
        }
        if ( viewParams->mapQuality() == Marble::OutlineQuality 
          || viewParams->mapQuality() == Marble::LowQuality ) {

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
         m_vectorMap->createFromPntMap( m_usaStates, viewParams->viewport() );

         m_vectorMap->setPen( m_statePen );
         m_vectorMap->setBrush( m_stateBrush );
         m_vectorMap->paintMap( painter, antialiased );

        // International Dateline
         m_vectorMap->setzBoundingBoxLimit( -1.0 );
         m_vectorMap->setzPointLimit( -1.0 );
         m_vectorMap->createFromPntMap( m_dateLine, viewParams->viewport() );

         m_vectorMap->setPen( m_dateLinePen );
         m_vectorMap->setBrush( m_dateLineBrush );
         m_vectorMap->paintMap( painter, antialiased );
    }

    // qDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}


void VectorComposer::resizeMap( int width, int height )
{
    m_vectorMap->resizeMap( width, height );
}
