//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2010           Thibaut Gridel <tgridel@free.fr>
//

#include "GeometryLayer.h"

// Marble
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataLineStyle.h"
#include "GeoDataObject.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "MarbleDebug.h"
#include "GeoDataTypes.h"

#include "GeoPainter.h"

// Qt
#include <QtCore/QTime>

namespace Marble
{


class GeometryLayerPrivate
{
 public:
    GeoDataDocument *m_root;

    void setBrushStyle( GeoPainter *painter, GeoDataStyle *style );
    void setPenStyle( GeoPainter *painter, GeoDataStyle *style );
    bool renderGeoDataObject( GeoPainter *painter, GeoDataObject *object );
    bool renderGeoDataGeometry( GeoPainter *painter, GeoDataGeometry *geometry, GeoDataStyle *style );

    QBrush m_currentBrush;
    QPen m_currentPen;
};

GeometryLayer::GeometryLayer( GeoDataDocument *document )
    : d( new GeometryLayerPrivate() )
{
    d->m_root = document;
}

GeometryLayer::~GeometryLayer()
{
    delete d;
}

QStringList GeometryLayer::renderPosition() const
{
    return QStringList("HOVERS_ABOVE_SURFACE");
}

bool GeometryLayer::render( GeoPainter *painter, ViewportParams *viewport,
   const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED(viewport)
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)
//    QTime t;
//    t.start();
//    mDebug() << "rendering " << m_root;
    if ( d->m_root )
    {
        d->renderGeoDataObject( painter, d->m_root );
    }
//    mDebug() << "rendering geometry: " << t.elapsed();

    return true;
}

void GeometryLayerPrivate::setBrushStyle( GeoPainter *painter, GeoDataStyle *style )
{
    if( !style->polyStyle().fill() ) {
        if( painter->brush().color() != Qt::transparent )
        painter->setBrush( QColor( Qt::transparent ) );
        return;
    }

    if( painter->brush().color() != style->polyStyle().color() ) {
/*        mDebug() << "BrushColor:" 
                 << style->polyStyle().color()
                 << painter->brush().color();*/
        painter->setBrush( style->polyStyle().color() );
    }
}

void GeometryLayerPrivate::setPenStyle( GeoPainter *painter, GeoDataStyle *style )
{
    if( !style->polyStyle().outline() ) {
        m_currentPen.setColor( Qt::transparent );
        if( painter->pen() != m_currentPen ) painter->setPen( m_currentPen );
        return;
    }

    if( m_currentPen.color() != style->lineStyle().color() ||
        m_currentPen.widthF() != style->lineStyle().width() ) {
/*        mDebug() << "PenColor:"
                 << style->lineStyle().color()
                 << m_currentPen.color();
        mDebug() << "PenWidth:" 
                 << style->lineStyle().width()
                 << m_currentPen.widthF();*/
        m_currentPen.setColor( style->lineStyle().color() );
        m_currentPen.setWidthF( style->lineStyle().width() );
    }

    if (    painter->mapQuality() != Marble::HighQuality
         && painter->mapQuality() != Marble::PrintQuality )
    {
//            m_currentPen.setWidth( 0 );
        QColor penColor = m_currentPen.color();
        penColor.setAlpha( 255 );
        m_currentPen.setColor( penColor );
    }

    if( painter->pen() != m_currentPen ) painter->setPen( m_currentPen );
}

bool GeometryLayerPrivate::renderGeoDataObject( GeoPainter *painter, GeoDataObject *object )
{
//    mDebug() << "render object " << object << " " << object->nodeType();
    GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );
    if ( feature && !feature->isVisible() ) {
        return false;
    }

    else if( dynamic_cast<GeoDataPlacemark*>( object ) ) {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
        renderGeoDataGeometry( painter, placemark->geometry(), placemark->style() );
    }

    // parse all child objects of the container
    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( object );
    if ( container )
    {
        int rowCount = container->size();
        for ( int row = 0; row < rowCount; ++row )
        {            
            renderGeoDataObject( painter, container->child( row ) );
        }
    }

    return true;
}

bool GeometryLayerPrivate::renderGeoDataGeometry( GeoPainter *painter, GeoDataGeometry *object, GeoDataStyle *style )
{
//    mDebug() << "render geometry " << object << " " << object->nodeType();

    painter->save();
    painter->autoMapQuality();

    if( dynamic_cast<GeoDataLinearRing*>( object ) ) {
        painter->setBrush( QColor( 0, 0, 0, 0 ) );
        setPenStyle( painter, style );
        GeoDataLinearRing linearRing( *object );
        painter->drawPolygon( linearRing );
    }
    else if( dynamic_cast<GeoDataLineString*>( object ) ) {
        setPenStyle( painter, style );
        GeoDataLineString lineString( *object );
        painter->drawPolyline( lineString );
    }
    else if( dynamic_cast<GeoDataPolygon*>( object ) ) {
        setBrushStyle( painter, style );
        setPenStyle( painter, style );
        // geometries are implicitly shared, this shouldn't hurt
        GeoDataPolygon polygon( *object );
        painter->drawPolygon( polygon );
    }
    else if( dynamic_cast<GeoDataMultiGeometry*>( object ) ) {
        GeoDataMultiGeometry *multigeo = dynamic_cast<GeoDataMultiGeometry*>( object );
        int rowCount = multigeo->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            renderGeoDataGeometry( painter, multigeo->child( row ), style );
        }
    }

    painter->restore();
    return true;
}

}
