//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2010           Thibaut Gridel <tgridel@free.fr>
// Copyright 2011-2012      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2014           Gábor Péterffy   <peterffy95@gmail.com>
//

#include "GeometryLayer.h"

// Marble
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataLineStyle.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataObject.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataTypes.h"
#include "GeoDataFeature.h"
#include "MarbleDebug.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "GeoGraphicsScene.h"
#include "GeoGraphicsItem.h"
#include "GeoLineStringGraphicsItem.h"
#include "GeoPolygonGraphicsItem.h"
#include "GeoTrackGraphicsItem.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoPhotoGraphicsItem.h"
#include "ScreenOverlayGraphicsItem.h"
#include "TileId.h"
#include "MarbleGraphicsItem.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataTreeModel.h"
#include <OsmPlacemarkData.h>
#include "StyleBuilder.h"

// Qt
#include <qmath.h>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QColor>

namespace Marble
{
class GeometryLayerPrivate
{
public:
    typedef QList<GeoDataPlacemark const *> OsmQueue;

    explicit GeometryLayerPrivate( const QAbstractItemModel *model );

    void createGraphicsItems( const GeoDataObject *object );
    void createGraphicsItemFromGeometry( const GeoDataGeometry *object, const GeoDataPlacemark *placemark, bool avoidOsmDuplicates );
    void createGraphicsItemFromOverlay( const GeoDataOverlay *overlay );
    void removeGraphicsItems( const GeoDataFeature *feature );

    static int maximumZoomLevel();

    const QAbstractItemModel *const m_model;
    GeoGraphicsScene m_scene;
    QString m_runtimeTrace;
    QList<ScreenOverlayGraphicsItem*> m_items;

    QMap<qint64,OsmQueue> m_osmWayItems;
    QMap<qint64,OsmQueue> m_osmRelationItems;

    StyleBuilder::Ptr m_styleBuilder;

private:
    static void initializeDefaultValues();
    static QString createPaintLayerOrder(const QString &itemType, GeoDataFeature::GeoDataVisualCategory visualCategory, const QString &subType = QString());

    static int s_defaultMinZoomLevels[GeoDataFeature::LastIndex];
    static bool s_defaultValuesInitialized;
    static int s_maximumZoomLevel;
};

int GeometryLayerPrivate::s_defaultMinZoomLevels[GeoDataFeature::LastIndex];
bool GeometryLayerPrivate::s_defaultValuesInitialized = false;
int GeometryLayerPrivate::s_maximumZoomLevel = 0;
QStringList s_paintLayerOrder;

GeometryLayerPrivate::GeometryLayerPrivate( const QAbstractItemModel *model )
    : m_model( model ),
      m_styleBuilder(new StyleBuilder)
{
    initializeDefaultValues();
}

int GeometryLayerPrivate::maximumZoomLevel()
{
    return s_maximumZoomLevel;
}

QString GeometryLayerPrivate::createPaintLayerOrder(const QString &itemType, GeoDataFeature::GeoDataVisualCategory visualCategory, const QString &subType)
{
    QString const category = GeoDataFeature::visualCategoryName(visualCategory);
    if (subType.isEmpty()) {
        return QString("%1/%2").arg(itemType).arg(category);
    } else {
        return QString("%1/%2/%3").arg(itemType).arg(category).arg(subType);
    }
}

GeometryLayer::GeometryLayer( const QAbstractItemModel *model )
        : d( new GeometryLayerPrivate( model ) )
{
    const GeoDataObject *object = static_cast<GeoDataObject*>( d->m_model->index( 0, 0, QModelIndex() ).internalPointer() );
    if ( object && object->parent() )
        d->createGraphicsItems( object->parent() );

    connect( model, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
             this, SLOT(resetCacheData()) );
    connect( model, SIGNAL(rowsInserted(QModelIndex,int,int)),
             this, SLOT(addPlacemarks(QModelIndex,int,int)) );
    connect( model, SIGNAL(rowsAboutToBeRemoved(QModelIndex,int,int)),
             this, SLOT(removePlacemarks(QModelIndex,int,int)) );
    connect( model, SIGNAL(modelReset()),
             this, SLOT(resetCacheData()) );
    connect( this, SIGNAL(highlightedPlacemarksChanged(QVector<GeoDataPlacemark*>)),
             &d->m_scene, SLOT(applyHighlight(QVector<GeoDataPlacemark*>)) );
    connect( &d->m_scene, SIGNAL(repaintNeeded()),
             this, SIGNAL(repaintNeeded()) );
}

GeometryLayer::~GeometryLayer()
{
    delete d;
}

QStringList GeometryLayer::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

void GeometryLayerPrivate::initializeDefaultValues()
{
    if ( s_defaultValuesInitialized )
        return;

    for ( int i = 0; i < GeoDataFeature::LastIndex; i++ )
        s_defaultMinZoomLevels[i] = 15;

    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::Landmass);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::UrbanArea);
    for ( int i = GeoDataFeature::LanduseAllotments; i <= GeoDataFeature::LanduseVineyard; i++ ) {
        if ((GeoDataFeature::GeoDataVisualCategory)i != GeoDataFeature::LanduseGrass) {
            s_paintLayerOrder << createPaintLayerOrder("Polygon", (GeoDataFeature::GeoDataVisualCategory)i);
        }
    }
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalBeach);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalWetland);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalGlacier);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalIceShelf);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalCliff);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalPeak);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::MilitaryDangerArea);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisurePark);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisurePitch);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisureSportsCentre);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisureStadium);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalWood);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LanduseGrass);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisurePlayground);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalScrub);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::LeisureTrack);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::TransportParking);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::TransportParkingSpace);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::ManmadeBridge);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::BarrierCityWall);

    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::AmenityGraveyard);

    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::EducationCollege);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::EducationSchool);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::EducationUniversity);
    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::HealthHospital);

    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::Landmass);

    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::NaturalWater);
    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalWater, "outline");
    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalWater, "inline");
    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalWater, "label");


    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalReef, "outline");
    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalReef, "inline");
    s_paintLayerOrder << createPaintLayerOrder("LineString", GeoDataFeature::NaturalReef, "label");


    for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
    }
    for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
    }
    for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
    }
    for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
    }
    for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
    }
    for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
    }

    s_paintLayerOrder << createPaintLayerOrder("Polygon", GeoDataFeature::TransportPlatform);

    for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
    }
    for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
    }
    for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
        s_paintLayerOrder << createPaintLayerOrder("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
    }

    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::AmenityGraveyard);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalWood);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalBeach);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalWetland);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalGlacier);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalIceShelf);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalScrub);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisurePark);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisurePlayground);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisurePitch);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisureSportsCentre);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisureStadium);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::LeisureTrack);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::TransportParking);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::ManmadeBridge);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::BarrierCityWall);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalWater);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalReef);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::Landmass);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalCliff);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::NaturalPeak);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::EducationCollege);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::EducationSchool);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::EducationUniversity);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::HealthHospital);
    s_paintLayerOrder << createPaintLayerOrder("Point", GeoDataFeature::MilitaryDangerArea);

    s_paintLayerOrder << "Polygon/Building/frame";
    s_paintLayerOrder << "Polygon/Building/roof";

    Q_ASSERT(QSet<QString>::fromList(s_paintLayerOrder).size() == s_paintLayerOrder.size());

    s_defaultMinZoomLevels[GeoDataFeature::Default]             = 1;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalReef]         = 3;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalWater]        = 3;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalWood]         = 8;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalBeach]        = 10;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalWetland]      = 10;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalGlacier]      = 3;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalIceShelf]     = 3;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalScrub]        = 10;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalCliff]        = 15;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalPeak]         = 11;
    s_defaultMinZoomLevels[GeoDataFeature::BarrierCityWall]     = 15;
    s_defaultMinZoomLevels[GeoDataFeature::Building]            = 15;

    s_defaultMinZoomLevels[GeoDataFeature::ManmadeBridge]       = 15;

        // OpenStreetMap highways
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySteps]        = 15;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayUnknown]      = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPath]         = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrack]        = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPedestrian]   = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayFootway]      = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayCycleway]     = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayService]      = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayRoad]         = 13;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTertiaryLink] = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTertiary]     = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondaryLink]= 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondary]    = 9;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimaryLink]  = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimary]      = 8;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunkLink]    = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunk]        = 7;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayMotorwayLink] = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayMotorway]     = 6;

#if 0 // not needed as long as default min zoom level is 15
    for(int i = GeoDataFeature::AccomodationCamping; i <= GeoDataFeature::ReligionSikh; i++)
        s_defaultMinZoomLevels[i] = 15;
#endif

    s_defaultMinZoomLevels[GeoDataFeature::AmenityGraveyard]    = 14;
    s_defaultMinZoomLevels[GeoDataFeature::AmenityFountain]     = 17;

    s_defaultMinZoomLevels[GeoDataFeature::MilitaryDangerArea]  = 11;

    s_defaultMinZoomLevels[GeoDataFeature::LeisurePark]         = 11;
    s_defaultMinZoomLevels[GeoDataFeature::LeisurePlayground]   = 11;
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
    s_defaultMinZoomLevels[GeoDataFeature::LanduseOrchard]      = 14;
    s_defaultMinZoomLevels[GeoDataFeature::LanduseVineyard]     = 14;

    s_defaultMinZoomLevels[GeoDataFeature::RailwayRail]         = 6;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayNarrowGauge]  = 6;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayTram]         = 14;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayLightRail]    = 12;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayAbandoned]    = 10;
    s_defaultMinZoomLevels[GeoDataFeature::RailwaySubway]       = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayPreserved]    = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayMiniature]    = 13;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayConstruction] = 10;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayMonorail]     = 12;
    s_defaultMinZoomLevels[GeoDataFeature::RailwayFunicular]    = 13;
    s_defaultMinZoomLevels[GeoDataFeature::TransportPlatform]   = 16;

    s_defaultMinZoomLevels[GeoDataFeature::Satellite]           = 0;

    s_defaultMinZoomLevels[GeoDataFeature::Landmass]            = 0;
    s_defaultMinZoomLevels[GeoDataFeature::UrbanArea]           = 3;
    s_defaultMinZoomLevels[GeoDataFeature::InternationalDateLine]      = 1;

    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel1]         = 0;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel2]         = 1;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel3]         = 1;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel4]         = 2;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel5]         = 4;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel6]         = 5;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel7]         = 5;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel8]         = 7;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel9]         = 7;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel10]        = 8;
    s_defaultMinZoomLevels[GeoDataFeature::AdminLevel11]        = 8;

    s_defaultMinZoomLevels[GeoDataFeature::BoundaryMaritime]    = 1;
    for ( int i = 0; i < GeoDataFeature::LastIndex; ++i ) {
        s_maximumZoomLevel = qMax( s_maximumZoomLevel, s_defaultMinZoomLevels[i] );
    }

    s_defaultValuesInitialized = true;
}


bool GeometryLayer::render( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();

    int maxZoomLevel = qMin<int>( qMax<int>( qLn( viewport->radius() *4 / 256 ) / qLn( 2.0 ), 1), GeometryLayerPrivate::maximumZoomLevel() );
    QList<GeoGraphicsItem*> items = d->m_scene.items( viewport->viewLatLonAltBox(), maxZoomLevel );

    typedef QPair<QString, GeoGraphicsItem*> LayerItem;
    QList<LayerItem> defaultLayer;
    int paintedItems = 0;
    QHash<QString, QList<GeoGraphicsItem*> > paintedFragments;
    foreach( GeoGraphicsItem* item, items )
    {
        if ( item->latLonAltBox().intersects( viewport->viewLatLonAltBox() ) ) {
            QStringList paintLayers = item->paintLayers();
            if (paintLayers.isEmpty()) {
                mDebug() << item << " provides no paint layers, so I force one onto it.";
                paintLayers << QString();
            }
            foreach(const auto &layer, paintLayers) {
                if (s_paintLayerOrder.contains(layer)) {
                    paintedFragments[layer] << item;
                } else {
                    defaultLayer << LayerItem(layer, item);
                    static QSet<QString> missingLayers;
                    if (!missingLayers.contains(layer)) {
                        mDebug() << "Missing layer " << layer << ", in render order, will render it on top";
                        missingLayers << layer;
                    }
                }
            }
            ++paintedItems;
        }
    }

    QStringList paintedLayers = s_paintLayerOrder;
    foreach(const QString &layer, paintedLayers) {
        QList<GeoGraphicsItem*> & layerItems = paintedFragments[layer];
        qStableSort(layerItems.begin(), layerItems.end(), GeoGraphicsItem::zValueLessThan);
        foreach(auto item, layerItems) {
            item->paint(painter, viewport, layer);
        }
    }
    foreach(const auto & item, defaultLayer) {
        item.second->paint(painter, viewport, item.first);
    }

    foreach( ScreenOverlayGraphicsItem* item, d->m_items ) {
        item->paintEvent( painter, viewport );
    }

    painter->restore();
    d->m_runtimeTrace = QString( "Geometries: %1 Drawn: %2 Zoom: %3")
                .arg( items.size() )
                .arg( paintedItems )
                .arg( maxZoomLevel );
    return true;
}

RenderState GeometryLayer::renderState() const
{
    return RenderState( "GeoGraphicsScene" );
}

QString GeometryLayer::runtimeTrace() const
{
    return d->m_runtimeTrace;
}

void GeometryLayerPrivate::createGraphicsItems( const GeoDataObject *object )
{
    if ( const GeoDataPlacemark *placemark = dynamic_cast<const GeoDataPlacemark*>( object ) )
    {
        createGraphicsItemFromGeometry( placemark->geometry(), placemark, true );
    } else if ( const GeoDataOverlay* overlay = dynamic_cast<const GeoDataOverlay*>( object ) ) {
        createGraphicsItemFromOverlay( overlay );
    }

    // parse all child objects of the container
    if ( const GeoDataContainer *container = dynamic_cast<const GeoDataContainer*>( object ) )
    {
        int rowCount = container->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItems( container->child( row ) );
        }
    }
}

void GeometryLayerPrivate::createGraphicsItemFromGeometry(const GeoDataGeometry* object, const GeoDataPlacemark *placemark , bool avoidOsmDuplicates)
{
    GeoGraphicsItem *item = 0;
    if ( object->nodeType() == GeoDataTypes::GeoDataLineStringType )
    {
        const GeoDataLineString* line = static_cast<const GeoDataLineString*>( object );
        item = new GeoLineStringGraphicsItem( placemark, line );
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataLinearRingType )
    {
        if (avoidOsmDuplicates && placemark->hasOsmData()){
            qint64 const osmId = placemark->osmData().id();
            if (osmId > 0) {
                m_osmWayItems[osmId] << placemark;
                if (m_osmWayItems[osmId].size()>1) {
                    return;
                }
            }
        }

        const GeoDataLinearRing *ring = static_cast<const GeoDataLinearRing*>( object );
        item = new GeoPolygonGraphicsItem( placemark, ring );
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataPolygonType )
    {
        if (avoidOsmDuplicates && placemark->hasOsmData()){
            qint64 const osmId = placemark->osmData().id();
            if (osmId > 0) {
                m_osmRelationItems[osmId] << placemark;
                if (m_osmRelationItems[osmId].size()>1) {
                    return;
                }
            }
        }

        const GeoDataPolygon *poly = static_cast<const GeoDataPolygon*>( object );
        item = new GeoPolygonGraphicsItem( placemark, poly );
        item->setZValue(poly->renderOrder());
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataMultiGeometryType  )
    {
        const GeoDataMultiGeometry *multigeo = static_cast<const GeoDataMultiGeometry*>( object );
        int rowCount = multigeo->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multigeo->child( row ), placemark, true );
        }
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataMultiTrackType  )
    {
        const GeoDataMultiTrack *multitrack = static_cast<const GeoDataMultiTrack*>( object );
        int rowCount = multitrack->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multitrack->child( row ), placemark, true );
        }
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataTrackType )
    {
        const GeoDataTrack *track = static_cast<const GeoDataTrack*>( object );
        item = new GeoTrackGraphicsItem( placemark, track );
    }
    if ( !item )
        return;
    item->setStyleBuilder(m_styleBuilder);
    item->setVisible( placemark->isGloballyVisible() );
    item->setMinZoomLevel( s_defaultMinZoomLevels[placemark->visualCategory()] );
    m_scene.addItem( item );
}

void GeometryLayerPrivate::createGraphicsItemFromOverlay( const GeoDataOverlay *overlay )
{
    GeoGraphicsItem* item = 0;
    if ( overlay->nodeType() == GeoDataTypes::GeoDataPhotoOverlayType ) {
        GeoDataPhotoOverlay const * photoOverlay = static_cast<GeoDataPhotoOverlay const *>( overlay );
        GeoPhotoGraphicsItem *photoItem = new GeoPhotoGraphicsItem( overlay );
        photoItem->setPoint( photoOverlay->point() );
        item = photoItem;
    } else if ( overlay->nodeType() == GeoDataTypes::GeoDataScreenOverlayType ) {
        GeoDataScreenOverlay const * screenOverlay = static_cast<GeoDataScreenOverlay const *>( overlay );
        ScreenOverlayGraphicsItem *screenItem = new ScreenOverlayGraphicsItem ( screenOverlay );
        m_items.push_back( screenItem );
    }

    if ( item ) {
        item->setStyleBuilder(m_styleBuilder);
        item->setVisible( overlay->isGloballyVisible() );
        m_scene.addItem( item );
    }
}

void GeometryLayerPrivate::removeGraphicsItems( const GeoDataFeature *feature )
{

    if( feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
        GeoDataPlacemark const * placemark = static_cast<GeoDataPlacemark const *>(feature);
        if (placemark->hasOsmData() && placemark->osmData().id() > 0) {
            QMap<qint64,OsmQueue>* osmItems = 0;
            if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                osmItems = &m_osmWayItems;
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                osmItems = &m_osmRelationItems;
            }
            if (osmItems) {
                OsmQueue & items = (*osmItems)[placemark->osmData().id()];
                Q_ASSERT(items.contains(placemark));
                if (items.first() == placemark) {
                    items.removeAt(0);
                    m_scene.removeItem( feature ); // the item was in use
                    if (!items.empty()) {
                        // we need to fill in a replacement now
                        createGraphicsItemFromGeometry(items.first()->geometry(), items.first(), false);
                    }
                } else {
                    // the item was not used
                    items.removeOne(placemark);
                }
                return;
            }
        }
        m_scene.removeItem( feature );
    }
    else if( feature->nodeType() == GeoDataTypes::GeoDataFolderType
             || feature->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
        const GeoDataContainer *container = static_cast<const GeoDataContainer*>( feature );
        foreach( const GeoDataFeature *child, container->featureList() ) {
            removeGraphicsItems( child );
        }
    }
    else if( feature->nodeType() == GeoDataTypes::GeoDataScreenOverlayType ) {
        foreach( ScreenOverlayGraphicsItem  *item, m_items ) {
            if( item->screenOverlay() == feature ) {
                m_items.removeAll( item );
            }
        }
    }
}

void GeometryLayer::addPlacemarks( const QModelIndex& parent, int first, int last )
{
    Q_ASSERT( first < d->m_model->rowCount( parent ) );
    Q_ASSERT( last < d->m_model->rowCount( parent ) );
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = d->m_model->index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) );
        Q_ASSERT( object );
        d->createGraphicsItems( object );
    }
    emit repaintNeeded();

}

void GeometryLayer::removePlacemarks( const QModelIndex& parent, int first, int last )
{
    Q_ASSERT( last < d->m_model->rowCount( parent ) );
    bool isRepaintNeeded = false;
    for( int i=first; i<=last; ++i ) {
        QModelIndex index = d->m_model->index( i, 0, parent );
        Q_ASSERT( index.isValid() );
        const GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data( MarblePlacemarkModel::ObjectPointerRole ) );
        const GeoDataFeature *feature = dynamic_cast<const GeoDataFeature*>( object );
        if( feature != 0 ) {
            d->removeGraphicsItems( feature );
            isRepaintNeeded = true;
        }
    }
    if( isRepaintNeeded ) {
        emit repaintNeeded();
    }

}

void GeometryLayer::resetCacheData()
{
    d->m_scene.clear();
    qDeleteAll( d->m_items );
    d->m_items.clear();
    d->m_osmWayItems.clear();
    d->m_osmRelationItems.clear();

    const GeoDataObject *object = static_cast<GeoDataObject*>( d->m_model->index( 0, 0, QModelIndex() ).internalPointer() );
    if ( object && object->parent() )
        d->createGraphicsItems( object->parent() );
    emit repaintNeeded();
}

QVector<const GeoDataFeature*> GeometryLayer::whichFeatureAt(const QPoint& curpos , const ViewportParams *viewport)
{
    QVector<const GeoDataFeature*> result;
    int maxZoom = qMin<int>( qMax<int>( qLn( viewport->radius() *4 / 256 ) / qLn( 2.0 ), 1), GeometryLayerPrivate::maximumZoomLevel() );
    foreach ( GeoGraphicsItem * item, d->m_scene.items( viewport->viewLatLonAltBox(), maxZoom ) ) {
        if ( item->feature()->nodeType() == GeoDataTypes::GeoDataPhotoOverlayType ) {
            GeoPhotoGraphicsItem* photoItem = dynamic_cast<GeoPhotoGraphicsItem*>( item );
            qreal x(0.0), y( 0.0 );
            viewport->screenCoordinates( photoItem->point().coordinates(), x, y );

            if ( photoItem->style() != 0 &&
                 !photoItem->style()->iconStyle().icon().isNull() ) {

                int halfIconWidth = photoItem->style()->iconStyle().icon().size().width() / 2;
                int halfIconHeight = photoItem->style()->iconStyle().icon().size().height() / 2;

                if ( x - halfIconWidth < curpos.x() &&
                     curpos.x() < x + halfIconWidth &&
                     y - halfIconHeight / 2 < curpos.y() &&
                     curpos.y() < y + halfIconHeight / 2 ) {
                    result.push_back( item->feature() );
                }
            } else if ( curpos.x() == x && curpos.y() == y ) {
                result.push_back( item->feature() );
            }
        }
    }

    return result;
}

void GeometryLayer::handleHighlight( qreal lon, qreal lat, GeoDataCoordinates::Unit unit )
{
    GeoDataCoordinates clickedPoint( lon, lat, 0, unit );
    QVector<GeoDataPlacemark*> selectedPlacemarks;

    for ( int i = 0; i < d->m_model->rowCount(); ++i ) {
        QVariant const data = d->m_model->data ( d->m_model->index ( i, 0 ), MarblePlacemarkModel::ObjectPointerRole );
        GeoDataObject *object = qvariant_cast<GeoDataObject*> ( data );
        Q_ASSERT ( object );
        if ( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            Q_ASSERT( dynamic_cast<const GeoDataDocument *>( object ) != 0 );
            GeoDataDocument* doc = static_cast<GeoDataDocument*> ( object );
                bool isHighlight = false;

                foreach ( const GeoDataStyleMap &styleMap, doc->styleMaps() ) {
                    if ( styleMap.contains( QString("highlight") ) ) {
                        isHighlight = true;
                        break;
                    }
                }

                /*
                 * If a document doesn't specify any highlight
                 * styleId in its style maps then there is no need
                 * to further check that document for placemarks
                 * which have been clicked because we won't
                 * highlight them.
                 */
                if ( isHighlight ) {
                    QVector<GeoDataFeature*>::Iterator iter = doc->begin();
                    QVector<GeoDataFeature*>::Iterator const end = doc->end();

                    for ( ; iter != end; ++iter ) {
                        if ( (*iter)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
                            GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>( *iter );
                            GeoDataPolygon *polygon = dynamic_cast<GeoDataPolygon*>( placemark->geometry() );
                            GeoDataLineString *lineString = dynamic_cast<GeoDataLineString*>( placemark->geometry() );
                            GeoDataMultiGeometry *multiGeometry = dynamic_cast<GeoDataMultiGeometry*>(placemark->geometry() );
                            if ( polygon &&
                                polygon->contains( clickedPoint ) )
                            {
                                selectedPlacemarks.push_back( placemark );
                            }

                            if ( lineString &&
                                lineString->nodeType() == GeoDataTypes::GeoDataLinearRingType )
                            {
                                GeoDataLinearRing *linearRing = static_cast<GeoDataLinearRing*>( lineString );
                                if ( linearRing->contains( clickedPoint ) ) {
                                    selectedPlacemarks.push_back( placemark );
                                }
                            }

                            if ( multiGeometry ) {
                                QVector<GeoDataGeometry*>::Iterator multiIter = multiGeometry->begin();
                                QVector<GeoDataGeometry*>::Iterator const multiEnd = multiGeometry->end();

                                for ( ; multiIter != multiEnd; ++multiIter ) {
                                    GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>( *multiIter );
                                    GeoDataLineString *linestring = dynamic_cast<GeoDataLineString*>( *multiIter );

                                    if ( poly &&
                                        poly->contains( clickedPoint ) )
                                    {
                                        selectedPlacemarks.push_back( placemark );
                                        break;
                                    }

                                    if ( linestring &&
                                        linestring->nodeType() == GeoDataTypes::GeoDataLinearRingType )
                                    {
                                        GeoDataLinearRing *linearRing = static_cast<GeoDataLinearRing*>( linestring );
                                        if ( linearRing->contains( clickedPoint ) ) {
                                            selectedPlacemarks.push_back( placemark );
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
        }
    }

    emit highlightedPlacemarksChanged( selectedPlacemarks );
}

}

#include "moc_GeometryLayer.cpp"
