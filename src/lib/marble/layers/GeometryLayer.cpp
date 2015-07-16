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
    GeometryLayerPrivate( const QAbstractItemModel *model );

    void createGraphicsItems( const GeoDataObject *object );
    void createGraphicsItemFromGeometry( const GeoDataGeometry *object, const GeoDataPlacemark *placemark );
    void createGraphicsItemFromOverlay( const GeoDataOverlay *overlay );
    void removeGraphicsItems( const GeoDataFeature *feature );

    static int maximumZoomLevel();

    const QAbstractItemModel *const m_model;
    GeoGraphicsScene m_scene;
    QString m_runtimeTrace;
    QList<ScreenOverlayGraphicsItem*> m_items;

private:
    static void initializeDefaultValues();

    static int s_defaultZValues[GeoDataFeature::LastIndex];
    static int s_defaultMinZoomLevels[GeoDataFeature::LastIndex];
    static bool s_defaultValuesInitialized;
    static int s_maximumZoomLevel;
    static const int s_defaultZValue;
};

int GeometryLayerPrivate::s_defaultZValues[GeoDataFeature::LastIndex];
int GeometryLayerPrivate::s_defaultMinZoomLevels[GeoDataFeature::LastIndex];
bool GeometryLayerPrivate::s_defaultValuesInitialized = false;
int GeometryLayerPrivate::s_maximumZoomLevel = 0;
const int GeometryLayerPrivate::s_defaultZValue = 50;

GeometryLayerPrivate::GeometryLayerPrivate( const QAbstractItemModel *model )
    : m_model( model )
{
    initializeDefaultValues();
}

int GeometryLayerPrivate::maximumZoomLevel()
{
    return s_maximumZoomLevel;
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
        s_defaultZValues[i] = s_defaultZValue;

    for ( int i = 0; i < GeoDataFeature::LastIndex; i++ )
        s_defaultMinZoomLevels[i] = 15;

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

    s_defaultMinZoomLevels[GeoDataFeature::Default]             = 1;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalWater]        = 8;
    s_defaultMinZoomLevels[GeoDataFeature::NaturalWood]         = 8;
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
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondaryLink]= 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwaySecondary]    = 9;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimaryLink]  = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayPrimary]      = 8;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunkLink]    = 10;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayTrunk]        = 7;
    s_defaultMinZoomLevels[GeoDataFeature::HighwayMotorwayLink] = 10;
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

    s_defaultMinZoomLevels[GeoDataFeature::Satellite]           = 0;

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

    int painted = 0;
    foreach( GeoGraphicsItem* item, items )
    {
        if ( item->latLonAltBox().intersects( viewport->viewLatLonAltBox() ) ) {
            item->paint( painter, viewport );
            ++painted;
        }
    }

    foreach( ScreenOverlayGraphicsItem* item, d->m_items ) {
        item->paintEvent( painter, viewport );
    }

    painter->restore();
    d->m_runtimeTrace = QString( "Geometries: %1 Drawn: %2 Zoom: %3")
                .arg( items.size() )
                .arg( painted )
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
        createGraphicsItemFromGeometry( placemark->geometry(), placemark );
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

void GeometryLayerPrivate::createGraphicsItemFromGeometry( const GeoDataGeometry* object, const GeoDataPlacemark *placemark )
{
    GeoGraphicsItem *item = 0;
    if ( object->nodeType() == GeoDataTypes::GeoDataLineStringType )
    {
        const GeoDataLineString* line = static_cast<const GeoDataLineString*>( object );
        item = new GeoLineStringGraphicsItem( placemark, line );
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataLinearRingType )
    {
        const GeoDataLinearRing *ring = static_cast<const GeoDataLinearRing*>( object );
        item = new GeoPolygonGraphicsItem( placemark, ring );
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataPolygonType )
    {
        const GeoDataPolygon *poly = static_cast<const GeoDataPolygon*>( object );
        item = new GeoPolygonGraphicsItem( placemark, poly );
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataMultiGeometryType  )
    {
        const GeoDataMultiGeometry *multigeo = static_cast<const GeoDataMultiGeometry*>( object );
        int rowCount = multigeo->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multigeo->child( row ), placemark );
        }
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataMultiTrackType  )
    {
        const GeoDataMultiTrack *multitrack = static_cast<const GeoDataMultiTrack*>( object );
        int rowCount = multitrack->size();
        for ( int row = 0; row < rowCount; ++row )
        {
            createGraphicsItemFromGeometry( multitrack->child( row ), placemark );
        }
    }
    else if ( object->nodeType() == GeoDataTypes::GeoDataTrackType )
    {
        const GeoDataTrack *track = static_cast<const GeoDataTrack*>( object );
        item = new GeoTrackGraphicsItem( placemark, track );
    }
    if ( !item )
        return;
    item->setStyle( placemark->style() );
    item->setVisible( placemark->isGloballyVisible() );
    item->setZValue( s_defaultZValues[placemark->visualCategory()] );
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
        item->setStyle( overlay->style() );
        item->setVisible( overlay->isGloballyVisible() );
        m_scene.addItem( item );
    }
}

void GeometryLayerPrivate::removeGraphicsItems( const GeoDataFeature *feature )
{

    if( feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
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

void GeometryLayer::addPlacemarks( QModelIndex parent, int first, int last )
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

void GeometryLayer::removePlacemarks( QModelIndex parent, int first, int last )
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
