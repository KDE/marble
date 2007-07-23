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
#include "clippainter.h"
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

    m_areapen = QPen( Qt::NoPen );
    m_riverpen.setStyle( Qt::SolidLine );
    m_riverpen.setColor( QColor( 0, 0, 255 ) );
    m_areabrush = QBrush( QColor( 255, 0, 0 ) );
    m_lakebrush = QBrush( QColor( 0, 0, 0 ) );
    m_borderpen.setStyle( Qt::SolidLine );
    m_borderpen.setColor( QColor( 0, 255, 0 ) );

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
    m_vectorMap -> setPen( m_areapen );
    m_vectorMap -> setBrush( m_areabrush );
    m_vectorMap -> drawMap( origimg, false );

    // Islands
    m_vectorMap -> setzBoundingBoxLimit( 0.8 );
    m_vectorMap -> setzPointLimit( 0.9 );

    m_vectorMap -> createFromPntMap( m_islands, radius, rotAxis );
    m_vectorMap -> setPen( m_areapen );
    m_vectorMap -> setBrush( m_areabrush );
    m_vectorMap -> drawMap( origimg, false );

    if ( m_showIceLayer == true ) {
        // Glaciers
         m_vectorMap -> setzBoundingBoxLimit( 0.8 );
         m_vectorMap -> setzPointLimit( 0.9 );
         m_vectorMap -> createFromPntMap( m_glaciers, radius, rotAxis );
         m_vectorMap -> setBrush( m_lakebrush );

         m_vectorMap -> drawMap( origimg, false );
    }
    // qDebug() << "TextureMap calculated nodes: " << vectormap->nodeCount();

    m_vectorMap -> drawMap( origimg, false );
    // qDebug() << "TextureMap calculated nodes: " << m_vectorMap->nodeCount();
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

         m_vectorMap -> setPen( QColor( 99, 123, 255 ) );
         m_vectorMap -> paintMap( painter, false );
    }

    if ( m_showBorders == true ) {
        // Countries
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_countries, radius, rotAxis );
        // QPen borderpen(QColor(242,187,136));

        QPen  borderPen( QColor( 242, 155, 104 ) );
        // borderpen.setStyle(Qt::DashLine);
         m_vectorMap -> setPen( borderPen );
         m_vectorMap -> setBrush( Qt::NoBrush );
         m_vectorMap -> paintMap( painter, false );

        // US-States
         m_vectorMap -> setzBoundingBoxLimit( -1.0 );
         m_vectorMap -> setzPointLimit( -1.0 );
         m_vectorMap -> createFromPntMap( m_usaStates, radius, rotAxis );

        // QPen statepen(QColor(242,187,136));
        QPen  statepen( QColor( 242, 155, 104 ) );
        statepen.setStyle( Qt::DotLine );
         m_vectorMap -> setPen( statepen );
         m_vectorMap -> setBrush( Qt::NoBrush );
         m_vectorMap -> paintMap( painter, false );
    }

    if ( m_showLakes == true ) {
        // Lakes
         m_vectorMap -> setzBoundingBoxLimit( 0.95 );
         m_vectorMap -> setzPointLimit( 0.98 ); 

         m_vectorMap -> createFromPntMap( m_lakes, radius, rotAxis );
         m_vectorMap -> setPen( m_areapen );
         m_vectorMap -> setBrush( QBrush( QColor( 208, 223, 241 ) ) );
         m_vectorMap -> paintMap( painter, false );
    }

    // qDebug() << "M_VectorMap calculated nodes: " << m_vectorMap->nodeCount();
}


void VectorComposer::resizeMap( int width, int height )
{
    m_vectorMap->resizeMap( width, height );
}

