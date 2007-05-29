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

    pcoast = new PntMap();
    pcoast->load( KAtlasDirs::path( "mwdbii/PCOAST.PNT" ) );

    pisland = new PntMap();
    pisland->load( KAtlasDirs::path( "mwdbii/PISLAND.PNT" ) );
    plake = new PntMap();
    plake->load( KAtlasDirs::path( "mwdbii/PLAKE.PNT" ) );
    pglacier = new PntMap();
    pglacier->load( KAtlasDirs::path( "mwdbii/PGLACIER.PNT" ) );
    priver = new PntMap();
    priver->load( KAtlasDirs::path( "mwdbii/RIVER.PNT" ) );

    // The countries.
    pborder = new PntMap();
    pborder->load( KAtlasDirs::path( "mwdbii/PDIFFBORDER.PNT" ) );

    // The States of the USA.
    pusa = new PntMap();
    pusa->load( KAtlasDirs::path( "mwdbii/PUSA48.DIFF.PNT" ) );

    vectormap = new VectorMap();

    m_areapen = QPen( Qt::NoPen );
    m_riverpen.setStyle( Qt::SolidLine );
    m_riverpen.setColor( QColor( 0, 0, 255 ) );
    m_areabrush = QBrush( QColor( 255, 0, 0 ) );
    m_lakebrush = QBrush( QColor( 0, 0, 0 ) );
    m_borderpen.setStyle( Qt::SolidLine );
    m_borderpen.setColor( QColor( 0, 255, 0 ) );
}


// void VectorComposer::drawMap(QImage* origimg, const int& radius, Quaternion& rotAxis){

void VectorComposer::drawTextureMap(QPaintDevice *origimg, const int& radius, 
                                    Quaternion& rotAxis)
{
    //	vectormap -> clearNodeCount();

    // Coastlines
    vectormap -> setzBoundingBoxLimit( 0.4 ); 
    vectormap -> setzPointLimit( 0 ); // 0.6 results in green pacific

    vectormap -> createFromPntMap( pcoast, radius, rotAxis );
    vectormap -> setPen( m_areapen );
    vectormap -> setBrush( m_areabrush );
    vectormap -> drawMap( origimg, false );

    // Islands
    vectormap -> setzBoundingBoxLimit( 0.8 );
    vectormap -> setzPointLimit( 0.9 );

    vectormap -> createFromPntMap( pisland, radius, rotAxis );
    vectormap -> setPen( m_areapen );
    vectormap -> setBrush( m_areabrush );
    vectormap -> drawMap( origimg, false );

    // Glaciers
    vectormap -> setzBoundingBoxLimit( 0.8 );
    vectormap -> setzPointLimit( 0.9 );
    vectormap -> createFromPntMap( pglacier, radius, rotAxis );
    vectormap -> setBrush( m_lakebrush );

    vectormap -> drawMap( origimg, false );
    //	qDebug() << "TextureMap calculated nodes: " << vectormap->nodeCount();
}


void VectorComposer::paintVectorMap(ClipPainter *painter, const int& radius, 
                                    Quaternion& rotAxis)
{

    // vectormap -> clearNodeCount();

    // Rivers
    vectormap -> setzBoundingBoxLimit( -1.0 );
    vectormap -> setzPointLimit( -1.0 );
    vectormap -> createFromPntMap( priver, radius, rotAxis );

    vectormap -> setPen( QColor( 99, 123, 255 ) );
    vectormap -> paintMap( painter, false );

    // Countries
    vectormap -> setzBoundingBoxLimit( -1.0 );
    vectormap -> setzPointLimit( -1.0 );

    vectormap -> createFromPntMap( pborder, radius, rotAxis );
    // QPen borderpen(QColor(242,187,136));

    QPen  borderpen( QColor( 242, 155, 104 ) );
    // borderpen.setStyle(Qt::DashLine);

    vectormap -> setPen( borderpen );
    vectormap -> setBrush( Qt::NoBrush );
    vectormap -> paintMap( painter, false );

    // US-States
    vectormap -> setzBoundingBoxLimit( -1.0 );
    vectormap -> setzPointLimit( -1.0 );

    vectormap -> createFromPntMap( pusa, radius, rotAxis );
    // QPen statepen(QColor(242,187,136));
    QPen  statepen( QColor( 242, 155, 104 ) );
    statepen.setStyle( Qt::DotLine );
    vectormap -> setPen( statepen );
    vectormap -> setBrush( Qt::NoBrush );
    vectormap -> paintMap( painter, false );

    // Lakes
    vectormap -> setzBoundingBoxLimit( 0.95 );
    vectormap -> setzPointLimit( 0.98 ); 

    vectormap -> createFromPntMap( plake, radius, rotAxis );
    vectormap -> setPen( m_areapen );
    vectormap -> setBrush( QBrush( QColor( 214, 226, 255 ) ) );
    vectormap -> paintMap( painter, false );

#if 0
    // Glacier

    vectormap -> setzBoundingBoxLimit(0.8); 
    vectormap -> setzPointLimit(0.9); 

    vectormap -> createFromPntMap(pglacier,radius,rotAxis);
    vectormap -> setBrush(QColor(Qt::white));
    vectormap -> drawMap(origimg);
#endif
    // qDebug() << "VectorMap calculated nodes: " << vectormap->nodeCount();
}


// void VectorComposer::resizeMap(const QImage* origimg){
void VectorComposer::resizeMap(const QPaintDevice* origimg)
{
    vectormap->resizeMap( origimg );
}

