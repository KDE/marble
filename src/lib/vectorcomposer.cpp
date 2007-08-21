//
// This file is part of the Marble Project.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "vectorcomposer.h"

#include <cmath>

#include <QtGui/QColor>
#include <QtCore/QDebug>

#include "GeoPolygon.h"
#include "ClipPainter.h"
#include "VectorMap.h"
#include "ViewParams.h"
#include "MarbleDirs.h"


VectorComposer::VectorComposer()
{

    m_coastLines = new PntMap();
    m_coastLines->load( MarbleDirs::path( "mwdbii/PCOAST.PNT" ) );

    m_islands = new PntMap();
    m_islands->load( MarbleDirs::path( "mwdbii/PISLAND.PNT" ) );
    m_lakeislands = new PntMap();
    m_lakeislands->load( MarbleDirs::path( "mwdbii/PLAKEISLAND.PNT" ) );
    m_lakes = new PntMap();
    m_lakes->load( MarbleDirs::path( "mwdbii/PLAKE.PNT" ) );
    m_glaciers = new PntMap();
    m_glaciers->load( MarbleDirs::path( "mwdbii/PGLACIER.PNT" ) );
    m_rivers = new PntMap();
    m_rivers->load( MarbleDirs::path( "mwdbii/RIVER.PNT" ) );

    // The countries.
    m_countries = new PntMap();
    m_countries->load( MarbleDirs::path( "mwdbii/PDIFFBORDER.PNT" ) );

    // The States of the USA.
    m_usaStates = new PntMap();
    m_usaStates->load( MarbleDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) );

    m_dateLine = new PntMap();
    m_dateLine->load( MarbleDirs::path( "mwdbii/DATELINE.PNT" ) );

    m_vectorMap = new VectorMap();

    m_textureLandPen = QPen( Qt::NoPen );

    m_textureLandBrush = QBrush( QColor( 255, 0, 0 ) );
    m_textureLakeBrush = QBrush( QColor( 0, 255, 0 ) );
    m_textureGlacierBrush = QBrush( QColor( 0, 0, 0 ) );
    m_textureBorderPen.setStyle( Qt::SolidLine );
    m_textureBorderPen.setColor( QColor( 0, 255, 0 ) );

    m_oceanBrush = QBrush( QColor( 153, 179, 204 ) );
    m_oceanPen = QPen( Qt::NoPen );

    m_landBrush = QBrush( QColor( 242, 239, 233 ) );
    m_landPen = QPen( Qt::NoPen );

    m_dateLinePen.setStyle( Qt::DashLine );
    m_dateLinePen.setColor( QColor( 0, 0, 0 ) );
    m_dateLineBrush = QBrush( Qt::NoBrush );
}


void VectorComposer::drawTextureMap(ViewParams *viewParams)
{
    QPaintDevice  *origimg = viewParams->m_coastImage;
    const int      radius  = viewParams->m_radius;
    Quaternion     rotAxis = viewParams->m_planetAxis;

    //	m_vectorMap -> clearNodeCount();

    // Coastlines
    m_vectorMap -> setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap -> setzPointLimit( 0 ); // 0.6 results in green pacific

    m_vectorMap -> createFromPntMap( m_coastLines, radius, rotAxis, viewParams->m_projection );
    m_vectorMap -> setPen( m_textureLandPen );
    m_vectorMap -> setBrush( m_textureLandBrush );
    m_vectorMap -> drawMap( origimg, false, viewParams->m_projection );

    // Islands
    m_vectorMap -> setzBoundingBoxLimit( 0.8 );
    m_vectorMap -> setzPointLimit( 0.9 );

    m_vectorMap -> createFromPntMap( m_islands, radius, rotAxis, viewParams->m_projection );
    m_vectorMap -> setPen( m_textureLandPen );
    m_vectorMap -> setBrush( m_textureLandBrush );
    m_vectorMap -> drawMap( origimg, false, viewParams->m_projection );

    if ( viewParams->m_showLakes ) {
         // Lakes
         m_vectorMap -> setzBoundingBoxLimit( 0.95 );
         m_vectorMap -> setzPointLimit( 0.98 ); 

         m_vectorMap -> createFromPntMap( m_lakes, radius, rotAxis, viewParams->m_projection );
         m_vectorMap -> setBrush( m_textureLakeBrush );
         m_vectorMap -> drawMap( origimg, false, viewParams->m_projection );

         m_vectorMap -> createFromPntMap( m_lakeislands, radius, rotAxis, viewParams->m_projection );
         m_vectorMap -> setBrush( m_textureLandBrush );
         m_vectorMap -> drawMap( origimg, false, viewParams->m_projection );
    }

    if ( viewParams->m_showIceLayer ) {
        // Glaciers
         m_vectorMap -> setzBoundingBoxLimit( 0.8 );
         m_vectorMap -> setzPointLimit( 0.9 );
         m_vectorMap -> createFromPntMap( m_glaciers, radius, rotAxis, viewParams->m_projection );
         m_vectorMap -> setBrush( m_textureGlacierBrush );

         m_vectorMap -> drawMap( origimg, false, viewParams->m_projection );
    }

    // qDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
}

void VectorComposer::paintBaseVectorMap( ClipPainter *painter, 
                                         ViewParams *viewParams )
{
    const int   radius  = viewParams->m_radius;
    Quaternion  rotAxis = viewParams->m_planetAxis;

    m_vectorMap -> setPen( m_oceanPen );
    m_vectorMap -> setBrush( m_oceanBrush );
    m_vectorMap -> paintBase( painter, radius, true, viewParams->m_projection );

    // Coastlines
    m_vectorMap -> setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap -> setzPointLimit( 0 ); // 0.6 results in green pacific

    m_vectorMap -> createFromPntMap( m_coastLines, radius, rotAxis, viewParams->m_projection );
    m_vectorMap -> setPen( m_landPen );
    m_vectorMap -> setBrush( m_landBrush );
    m_vectorMap -> paintMap( painter, false );

    // Islands
    m_vectorMap -> setzBoundingBoxLimit( 0.8 );
    m_vectorMap -> setzPointLimit( 0.9 );

    m_vectorMap -> createFromPntMap( m_islands, radius, rotAxis, viewParams->m_projection );
    m_vectorMap -> setPen( m_landPen );
    m_vectorMap -> setBrush( m_landBrush );
    m_vectorMap -> paintMap( painter, false );

    if ( viewParams->m_showLakes ) {
         // Lakes
         m_vectorMap -> setzBoundingBoxLimit( 0.95 );
         m_vectorMap -> setzPointLimit( 0.98 ); 

         m_vectorMap -> createFromPntMap( m_lakes, radius, rotAxis, viewParams->m_projection );
         m_vectorMap -> setPen( m_lakePen );
         m_vectorMap -> setBrush( m_lakeBrush );
         m_vectorMap -> paintMap( painter, false );

         m_vectorMap -> createFromPntMap( m_lakeislands, radius, rotAxis, viewParams->m_projection );
         m_vectorMap -> setBrush( m_landBrush );
         m_vectorMap -> paintMap( painter, false );
    }
}

void VectorComposer::paintVectorMap( ClipPainter *painter,
                                     ViewParams *viewParams )
{
    // m_vectorMap -> clearNodeCount();

    const int   radius  = viewParams->m_radius;
    Quaternion  rotAxis = viewParams->m_planetAxis;

    if ( viewParams->m_showRivers ) {
        // Rivers
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_rivers, radius, rotAxis, viewParams->m_projection );

         m_vectorMap -> setPen( m_riverPen );
         m_vectorMap -> setBrush( m_riverBrush );
         m_vectorMap -> paintMap( painter, false );
    }

    if ( viewParams->m_showBorders ) {
        // Countries
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_countries, radius, rotAxis, viewParams->m_projection );

         m_vectorMap -> setPen( m_countryPen );
         m_vectorMap -> setBrush( m_countryBrush );
         m_vectorMap -> paintMap( painter, false );

        // US-States
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_usaStates, radius, rotAxis, viewParams->m_projection );

         m_vectorMap -> setPen( m_statePen );
         m_vectorMap -> setBrush( m_stateBrush );
         m_vectorMap -> paintMap( painter, false );

        // International Dateline
         m_vectorMap -> setzBoundingBoxLimit( 0.0 );
         m_vectorMap -> setzPointLimit( 0.0 );
         m_vectorMap -> createFromPntMap( m_dateLine, radius, rotAxis, viewParams->m_projection );

         m_vectorMap -> setPen( m_dateLinePen );
         m_vectorMap -> setBrush( m_dateLineBrush );
         m_vectorMap -> paintMap( painter, false );

    }

    // qDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}


void VectorComposer::resizeMap( int width, int height )
{
    m_vectorMap->resizeMap( width, height );
}

