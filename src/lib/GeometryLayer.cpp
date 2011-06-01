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
#include "ViewportParams.h"
#include "GeoGraphicsScene.h"
#include "GeoGraphicsItem.h"
#include "GeoLineStringGraphicsItem.h"
#include <QAbstractItemModel>

namespace Marble
{


class GeometryLayerPrivate
{
public:
    void createGraphicsItems( GeoDataObject *object );
    void createGraphicsItemFromGeometry( GeoDataGeometry *object, GeoDataStyle *style );

    QBrush m_currentBrush;
    QPen m_currentPen;
    GeoGraphicsScene m_scene;
    QAbstractItemModel *m_model;
};

GeometryLayer::GeometryLayer( QAbstractItemModel *model )
        : d( new GeometryLayerPrivate() )
{
    d->m_model = model;
    GeoDataObject *object = static_cast<GeoDataObject*>( d->m_model->index( 0, 0, QModelIndex() ).internalPointer() );
    if ( object && object->parent() )
        d->createGraphicsItems( object->parent() );
}

GeometryLayer::~GeometryLayer()
{
    delete d;
}

QStringList GeometryLayer::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

bool GeometryLayer::render( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
//    QTime t;
//    t.start();
//    mDebug() << "rendering " << m_root;
    /*if ( d->m_root )
    {
        d->renderGeoDataObject( painter, d->m_root, viewport );
    }*/
//    mDebug() << "rendering geometry: " << t.elapsed();
    QList<GeoGraphicsItem*> items = d->m_scene.items( viewport->viewLatLonAltBox() );
    foreach( GeoGraphicsItem* item, items )
    {
        //if(item->flags() & GeoGraphicsItem::ItemIsVisible)
        item->paint( painter, viewport, renderPos, layer );
    }
    return true;
}

void GeometryLayerPrivate::createGraphicsItems( GeoDataObject *object )
{
    GeoDataFeature *feature = dynamic_cast<GeoDataFeature*>( object );

    if ( dynamic_cast<GeoDataPlacemark*>( object ) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
        createGraphicsItemFromGeometry( placemark->geometry(), placemark->style() );
    }

    // parse all child objects of the container
    GeoDataContainer *container = dynamic_cast<GeoDataContainer*>( object );
    if ( container )
    {
        int rowCount = container->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItems( container->child( row ) );
        }
    }
}

void GeometryLayerPrivate::createGraphicsItemFromGeometry( GeoDataGeometry* object, GeoDataStyle* style )
{
    if ( dynamic_cast<GeoDataLinearRing*>( object ) )
    {
    }
    else if ( GeoDataLineString* line = dynamic_cast<GeoDataLineString*>( object ) )
    {
        GeoLineStringGraphicsItem *item = new GeoLineStringGraphicsItem();
        item->setLineString( *line );
        item->setStyle( style );
        m_scene.addIdem( item );
    }
    else if ( dynamic_cast<GeoDataPolygon*>( object ) )
    {
    }
    else if ( dynamic_cast<GeoDataMultiGeometry*>( object ) )
    {
        GeoDataMultiGeometry *multigeo = dynamic_cast<GeoDataMultiGeometry*>( object );
        int rowCount = multigeo->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multigeo->child( row ), style );
        }
    }
}

void GeometryLayer::invalidateScene()
{
    QList<GeoGraphicsItem*> items = d->m_scene.items();
    foreach( GeoGraphicsItem* item, items )
    {
        delete item;
    }
    d->m_scene.clear();
    GeoDataObject *object = static_cast<GeoDataObject*>( d->m_model->index( 0, 0, QModelIndex() ).internalPointer() );
    if ( object && object->parent() )
        d->createGraphicsItems( object->parent() );
}

}

#include "GeometryLayer.moc"
