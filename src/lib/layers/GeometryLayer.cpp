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
#include "GeoDataFeature.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoGraphicsScene.h"
#include "GeoGraphicsItem.h"
#include "GeoLineStringGraphicsItem.h"
#include "GeoPolygonGraphicsItem.h"
#include "TileId.h"

// Qt
#include <QtCore/QTime>
#include <QtCore/QAbstractItemModel>

namespace Marble
{
int GeometryLayer::s_defaultZValues[GeoDataFeature::LastIndex];
int GeometryLayer::s_defaultMinZoomLevels[GeoDataFeature::LastIndex];
bool GeometryLayer::s_defaultValuesInitialized = false;
int GeometryLayer::s_defaultZValue = 50;

QVector< int > GeometryLayer::s_weightfilter = QVector<int>()
    << 20 << 40 << 80
    << 160 << 320 << 640
    << 1280 << 2560 << 5120
    << 10240 << 20480 << 40960
    << 81920 << 163840 << 327680
    << 655360 << 1310720 << 2621440;

class GeometryLayerPrivate
{
public:
    void createGraphicsItems( GeoDataObject *object );
    void createGraphicsItemFromGeometry( GeoDataGeometry *object, GeoDataPlacemark *placemark );

    QBrush m_currentBrush;
    QPen m_currentPen;
    GeoGraphicsScene m_scene;
    QAbstractItemModel *m_model;
};

GeometryLayer::GeometryLayer( QAbstractItemModel *model )
        : d( new GeometryLayerPrivate() )
{
    if ( !s_defaultValuesInitialized )
        initializeDefaultValues();

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

void GeometryLayer::initializeDefaultValues()
{
    for ( int i = 0; i < GeoDataFeature::LastIndex; i++ )
        s_defaultZValues[i] = s_defaultZValue;
    
    for ( int i = 0; i < GeoDataFeature::LastIndex; i++ )
        s_defaultMinZoomLevels[i] = 0;

    s_defaultZValues[GeoDataFeature::None]                = 0;
    
    for ( int i = GeoDataFeature::LanduseAllotments; i <= GeoDataFeature::LanduseRetail; i++ )
        s_defaultZValues[(GeoDataFeature::GeoDataVisualCategory)i] = s_defaultZValue - 16;
    
    s_defaultZValues[GeoDataFeature::NaturalWater]        = s_defaultZValue - 16;
    s_defaultZValues[GeoDataFeature::NaturalWood]         = s_defaultZValue - 15;
    
    //Landuse
    
    s_defaultZValues[GeoDataFeature::LeisurePark]         = s_defaultZValue - 14; 
    
    s_defaultZValues[GeoDataFeature::TransportParking]    = s_defaultZValue - 13;
    
    s_defaultZValues[GeoDataFeature::HighwayTertiaryLink] = s_defaultZValue - 12;
    s_defaultZValues[GeoDataFeature::HighwaySecondaryLink]= s_defaultZValue - 12;
    s_defaultZValues[GeoDataFeature::HighwayPrimaryLink]  = s_defaultZValue - 12;
    s_defaultZValues[GeoDataFeature::HighwayTrunkLink]    = s_defaultZValue - 12;
    s_defaultZValues[GeoDataFeature::HighwayMotorwayLink] = s_defaultZValue - 12;

    s_defaultZValues[GeoDataFeature::HighwayUnknown]      = s_defaultZValue - 11;
    s_defaultZValues[GeoDataFeature::HighwayPath]         = s_defaultZValue - 10;
    s_defaultZValues[GeoDataFeature::HighwayTrack]        = s_defaultZValue - 9;
    s_defaultZValues[GeoDataFeature::HighwaySteps]        = s_defaultZValue - 8;
    s_defaultZValues[GeoDataFeature::HighwayPedestrian]   = s_defaultZValue - 8;
    s_defaultZValues[GeoDataFeature::HighwayService]      = s_defaultZValue - 7;
    s_defaultZValues[GeoDataFeature::HighwayRoad]         = s_defaultZValue - 6;
    s_defaultZValues[GeoDataFeature::HighwayTertiary]     = s_defaultZValue - 5;
    s_defaultZValues[GeoDataFeature::HighwaySecondary]    = s_defaultZValue - 4;
    s_defaultZValues[GeoDataFeature::HighwayPrimary]      = s_defaultZValue - 3;
    s_defaultZValues[GeoDataFeature::HighwayTrunk]        = s_defaultZValue - 2;
    s_defaultZValues[GeoDataFeature::HighwayMotorway]     = s_defaultZValue - 1;
    s_defaultZValues[GeoDataFeature::RailwayRail]         = s_defaultZValue - 1;
    
    
    s_defaultMinZoomLevels[GeoDataFeature::Building]            = 15;

        // OpenStreetMap highways
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySteps]        = 15;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayUnknown]      = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPath]         = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrack]        = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPedestrian]   = 14;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayService]      = 14;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayRoad]         = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTertiaryLink] = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTertiary]     = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondaryLink]= 9;
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondary]    = 9;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimaryLink]  = 8;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimary]      = 8; 
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunkLink]    = 7;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunk]        = 7;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayMotorwayLink] = 6;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayMotorway]     = 6;
        
    //FIXME: Bad, better to expand this
    for(int i = GeoDataFeature::AccomodationCamping; i <= GeoDataFeature::ReligionSikh; i++)
        s_defaultMinZoomLevels[i] = 15;

    s_defaultMinZoomLevels[GeoDataFeature::LeisurePark]         = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseAllotments]   = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseBasin]        = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseCemetery]     = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseCommercial]   = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseConstruction] = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseFarmland]     = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseFarmyard]     = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseGarages]      = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseGrass]        = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseIndustrial]   = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseLandfill]     = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseMeadow]       = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseMilitary]     = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseQuarry]       = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseRailway]      = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseReservoir]    = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseResidential]  = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseRetail]       = 11;

    s_defaultMinZoomLevels[GeoDataFeature::RailwayRail]         = 6;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayTram]         = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayLightRail]    = 12;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayAbandoned]    = 10;
    s_defaultMinZoomLevels[GeoDataFeature::RailwaySubway]       = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayPreserved]    = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayMiniature]    = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayConstruction] = 10;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayMonorail]     = 12;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayFunicular]    = 13;

    s_defaultValuesInitialized = true;
}


bool GeometryLayer::render( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    painter->save();
    painter->autoMapQuality();
    
    int maxZoomLevel = 0;
    for(QVector<int>::const_iterator i = s_weightfilter.constBegin(); 
        ( i != s_weightfilter.constEnd() ) && ( viewport->radius() > *i ); i++)
        maxZoomLevel++;
    
    QList<GeoGraphicsItem*> items = d->m_scene.items( viewport->viewLatLonAltBox(), maxZoomLevel );
    foreach( GeoGraphicsItem* item, items )
    {
        if ( item->visible() )
            item->paint( painter, viewport, renderPos, layer );
    }
    painter->restore();
    return true;
}

void GeometryLayerPrivate::createGraphicsItems( GeoDataObject *object )
{
    if ( dynamic_cast<GeoDataPlacemark*>( object ) )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( object );
        createGraphicsItemFromGeometry( placemark->geometry(), placemark );
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

void GeometryLayerPrivate::createGraphicsItemFromGeometry( GeoDataGeometry* object, GeoDataPlacemark *placemark )
{
    GeoGraphicsItem *item = 0;
    if ( dynamic_cast<GeoDataLinearRing*>( object ) )
    {
    }
    else if ( GeoDataLineString* line = dynamic_cast<GeoDataLineString*>( object ) )
    {
        item = new GeoLineStringGraphicsItem( line );
    }
    else if ( GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( object ) )
    {
        item = new GeoPolygonGraphicsItem( poly );
    }
    else if ( dynamic_cast<GeoDataMultiGeometry*>( object ) )
    {
        GeoDataMultiGeometry *multigeo = dynamic_cast<GeoDataMultiGeometry*>( object );
        int rowCount = multigeo->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multigeo->child( row ), placemark );
        }
    }
    if ( !item )
        return;
    item->setStyle( placemark->style() );
    item->setVisible( placemark->isVisible() );
    item->setZValue( GeometryLayer::s_defaultZValues[placemark->visualCategory()] );
    item->setMinZoomLevel( GeometryLayer::s_defaultMinZoomLevels[placemark->visualCategory()] );
    m_scene.addIdem( item );
}

void GeometryLayer::invalidateScene()
{
    QList<GeoGraphicsItem*> items = d->m_scene.items();
    QList<GeoGraphicsItem*> deletedItems;
    foreach( GeoGraphicsItem* item, items )
    {
        if( qBinaryFind( deletedItems, item ) != deletedItems.end() )
        {
            delete item;
            deletedItems.insert( qLowerBound( deletedItems.begin(), deletedItems.end(), item ), item );
        }
    }
    d->m_scene.clear();
    GeoDataObject *object = static_cast<GeoDataObject*>( d->m_model->index( 0, 0, QModelIndex() ).internalPointer() );
    if ( object && object->parent() )
        d->createGraphicsItems( object->parent() );
}

}

#include "GeometryLayer.moc"
