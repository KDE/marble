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

namespace Marble
{
class GeometryLayerPrivate
{
public:
    typedef QList<GeoDataPlacemark const *> OsmQueue;

    explicit GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder);

    void createGraphicsItems( const GeoDataObject *object );
    void createGraphicsItemFromGeometry( const GeoDataGeometry *object, const GeoDataPlacemark *placemark, bool avoidOsmDuplicates );
    void createGraphicsItemFromOverlay( const GeoDataOverlay *overlay );
    void removeGraphicsItems( const GeoDataFeature *feature );

    const QAbstractItemModel *const m_model;
    const StyleBuilder *const m_styleBuilder;
    GeoGraphicsScene m_scene;
    QString m_runtimeTrace;
    QList<ScreenOverlayGraphicsItem*> m_items;

    QMap<qint64,OsmQueue> m_osmWayItems;
    QMap<qint64,OsmQueue> m_osmRelationItems;
};

GeometryLayerPrivate::GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder) :
    m_model(model),
    m_styleBuilder(styleBuilder)
{
}

GeometryLayer::GeometryLayer(const QAbstractItemModel *model, const StyleBuilder *styleBuilder) :
    d(new GeometryLayerPrivate(model, styleBuilder))
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


bool GeometryLayer::render( GeoPainter *painter, ViewportParams *viewport,
                            const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED( renderPos )
    Q_UNUSED( layer )

    painter->save();

    const int maxZoomLevel = qMin<int>(qMax<int>(qLn(viewport->radius()*4/256)/qLn(2.0), 1), d->m_styleBuilder->maximumZoomLevel());
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
                if (d->m_styleBuilder->renderOrder().contains(layer)) {
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

    foreach (const QString &layer, d->m_styleBuilder->renderOrder()) {
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
        if (item->zValue() == 0) {
             item->setZValue(poly->renderOrder());
        }
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
    item->setMinZoomLevel(m_styleBuilder->minimumZoomLevel(placemark->visualCategory()));
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

QVector<const GeoDataFeature*> GeometryLayer::whichFeatureAt(const QPoint &curpos, const ViewportParams *viewport)
{
    const int maxZoom = qMin<int>(qMax<int>(qLn(viewport->radius()*4/256)/qLn(2.0), 1), d->m_styleBuilder->maximumZoomLevel());
    QVector<const GeoDataFeature*> result;
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

QVector<const GeoDataFeature *> GeometryLayer::whichBuildingAt(const QPoint &curpos, const ViewportParams *viewport)
{
    QVector<const GeoDataFeature*> result;
    qreal lon, lat;
    if (!viewport->geoCoordinates(curpos.x(), curpos.y(), lon, lat, GeoDataCoordinates::Radian)) {
        return result;
    }
    GeoDataCoordinates const coordinates = GeoDataCoordinates(lon, lat);

    const int maxZoom = qMin<int>(qMax<int>(qLn(viewport->radius()*4/256)/qLn(2.0), 1), d->m_styleBuilder->maximumZoomLevel());
    foreach ( GeoGraphicsItem * item, d->m_scene.items( viewport->viewLatLonAltBox(), maxZoom ) ) {
        if (item->feature()->visualCategory() == GeoDataFeature::Building && item->feature()->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            const GeoDataPlacemark* placemark = static_cast<const GeoDataPlacemark*>(item->feature());
            if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>(placemark->geometry());
                if (polygon->contains(coordinates)) {
                    result << item->feature();
                }
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                const GeoDataLinearRing *ring = static_cast<const GeoDataLinearRing*>(placemark->geometry());
                if (ring->contains(coordinates)) {
                    result << item->feature();
                }
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
