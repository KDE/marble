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
#include "vectormap.h"
#include "katlasdirs.h"


VectorComposer::VectorComposer()
{

    m_coastLines = new PntMap();
    m_coastLines->load( KAtlasDirs::path( "mwdbii/PCOAST.PNT" ) );

    m_islands = new PntMap();
    m_islands->load( KAtlasDirs::path( "mwdbii/PISLAND.PNT" ) );
    m_lakes = new PntMap();
    m_lakes->load( KAtlasDirs::path( "mwdbii/PLAKE.PNT" ) );
    m_glaciers = new PntMap();
    m_glaciers->load( KAtlasDirs::path( "mwdbii/PGLACIER.PNT" ) );
    m_rivers = new PntMap();
    m_rivers->load( KAtlasDirs::path( "mwdbii/RIVER.PNT" ) );

    // The countries.
    m_countries = new PntMap();
    m_countries->load( KAtlasDirs::path( "mwdbii/PDIFFBORDER.PNT" ) );

    // The States of the USA.
    m_usaStates = new PntMap();
    m_usaStates->load( KAtlasDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) );

    m_vectorMap = new VectorMap();

    m_textureLandPen = QPen( Qt::NoPen );

    m_textureLandBrush = QBrush( QColor( 255, 0, 0 ) );
    m_textureLakeBrush = QBrush( QColor( 0, 0, 0 ) );
    m_textureBorderPen.setStyle( Qt::SolidLine );
    m_textureBorderPen.setColor( QColor( 0, 255, 0 ) );

    m_oceanBrush = QBrush( QColor( 153, 179, 204 ) );
    m_oceanPen = QPen( Qt::NoPen );

    m_landBrush = QBrush( QColor( 242, 239, 233 ) );
    m_landPen = QPen( Qt::NoPen );

    m_showIceLayer = true;
    m_showBorders = true;
    m_showRivers = true;
    m_showLakes = true;

}


void VectorComposer::drawTextureMap(QPaintDevice *origimg, const int& radius, 
                                    Quaternion& rotAxis)
{
    //	m_vectorMap -> clearNodeCount();

    // Coastlines
    m_vectorMap -> setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap -> setzPointLimit( 0 ); // 0.6 results in green pacific

    m_vectorMap -> createFromPntMap( m_coastLines, radius, rotAxis );
    m_vectorMap -> setPen( m_textureLandPen );
    m_vectorMap -> setBrush( m_textureLandBrush );
    m_vectorMap -> drawMap( origimg, false );

    // Islands
    m_vectorMap -> setzBoundingBoxLimit( 0.8 );
    m_vectorMap -> setzPointLimit( 0.9 );

    m_vectorMap -> createFromPntMap( m_islands, radius, rotAxis );
    m_vectorMap -> setPen( m_textureLandPen );
    m_vectorMap -> setBrush( m_textureLandBrush );
    m_vectorMap -> drawMap( origimg, false );

    if ( m_showIceLayer == true ) {
        // Glaciers
         m_vectorMap -> setzBoundingBoxLimit( 0.8 );
         m_vectorMap -> setzPointLimit( 0.9 );
         m_vectorMap -> createFromPntMap( m_glaciers, radius, rotAxis );
         m_vectorMap -> setBrush( m_textureLakeBrush );

         m_vectorMap -> drawMap( origimg, false );
    }

    // qDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
}

void VectorComposer::paintBaseVectorMap(ClipPainter *painter, const int& radius, 
                                    Quaternion& rotAxis)
{
    m_vectorMap -> setPen( m_oceanPen );
    m_vectorMap -> setBrush( m_oceanBrush );
    m_vectorMap -> paintBase( painter, radius, true );

    // Coastlines
    m_vectorMap -> setzBoundingBoxLimit( 0.4 ); 
    m_vectorMap -> setzPointLimit( 0 ); // 0.6 results in green pacific

    m_vectorMap -> createFromPntMap( m_coastLines, radius, rotAxis );
    m_vectorMap -> setPen( m_landPen );
    m_vectorMap -> setBrush( m_landBrush );
    m_vectorMap -> paintMap( painter, false );

    // Islands
    m_vectorMap -> setzBoundingBoxLimit( 0.8 );
    m_vectorMap -> setzPointLimit( 0.9 );

    m_vectorMap -> createFromPntMap( m_islands, radius, rotAxis );
    m_vectorMap -> setPen( m_landPen );
    m_vectorMap -> setBrush( m_landBrush );
    m_vectorMap -> paintMap( painter, false );
}

void VectorComposer::paintVectorMap(ClipPainter *painter, const int& radius, 
                                    Quaternion& rotAxis)
{
    // m_vectorMap -> clearNodeCount();

    if ( m_showRivers == true ) {
        // Rivers
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_rivers, radius, rotAxis );

         m_vectorMap -> setPen( m_riverPen );
         m_vectorMap -> setBrush( m_riverBrush );
         m_vectorMap -> paintMap( painter, false );
    }

    if ( m_showBorders == true ) {
        // Countries
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_countries, radius, rotAxis );

         m_vectorMap -> setPen( m_countryPen );
         m_vectorMap -> setBrush( m_countryBrush );
         m_vectorMap -> paintMap( painter, false );

        // US-States
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_usaStates, radius, rotAxis );

         m_vectorMap -> setPen( m_statePen );
         m_vectorMap -> setBrush( m_stateBrush );
         m_vectorMap -> paintMap( painter, false );
    }

    if ( m_showLakes == true ) {
         // Lakes
         m_vectorMap -> setzBoundingBoxLimit( 0.95 );
         m_vectorMap -> setzPointLimit( 0.98 ); 

         m_vectorMap -> createFromPntMap( m_lakes, radius, rotAxis );
         m_vectorMap -> setPen( m_lakePen );
         m_vectorMap -> setBrush( m_lakeBrush );
         m_vectorMap -> paintMap( painter, false );
    }

    // qDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}


void VectorComposer::resizeMap( int width, int height )
{
    m_vectorMap->resizeMap( width, height );
}

