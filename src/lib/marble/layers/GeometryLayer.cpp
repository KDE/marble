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
#include "GeoDataLatLonAltBox.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataLineStyle.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataObject.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPolygon.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataFeature.h"
#include "MarbleDebug.h"
#include "GeoPainter.h"
#include "ViewportParams.h"
#include "RenderState.h"
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
#include "AbstractGeoPolygonGraphicsItem.h"
#include "GeoLineStringGraphicsItem.h"
#include "GeoDataRelation.h"

// Qt
#include <qmath.h>
#include <QAbstractItemModel>
#include <QModelIndex>

namespace Marble
{
class GeometryLayerPrivate
{
public:
    typedef QVector<GeoLineStringGraphicsItem*> OsmLineStringItems;
    typedef QSet<const GeoDataRelation *> Relations;
    typedef QHash<const GeoDataFeature *, Relations> FeatureRelationHash;
    typedef QVector<GeoGraphicsItem*> GeoGraphicItems;

    struct PaintFragments {
        // Three lists for different z values
        // A z value of 0 is default and used by the majority of items, so sorting
        // can be avoided for it
        QVector<GeoGraphicsItem*> negative; // subways
        QVector<GeoGraphicsItem*> null;     // areas and roads
        QVector<GeoGraphicsItem*> positive; // buildings
    };

    explicit GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder);

    void createGraphicsItems(const GeoDataObject *object);
    void createGraphicsItems(const GeoDataObject *object, FeatureRelationHash &relations);
    void createGraphicsItemFromGeometry(const GeoDataGeometry *object, const GeoDataPlacemark *placemark, const Relations &relations);
    void createGraphicsItemFromOverlay(const GeoDataOverlay *overlay);
    void removeGraphicsItems(const GeoDataFeature *feature);
    void updateTiledLineStrings(const GeoDataPlacemark *placemark, GeoLineStringGraphicsItem* lineStringItem);
    void updateTiledLineStrings(OsmLineStringItems &lineStringItems);
    void clearCache();
    bool showRelation(const GeoDataRelation* relation) const;
    void updateRelationVisibility();

    const QAbstractItemModel *const m_model;
    const StyleBuilder *const m_styleBuilder;
    GeoGraphicsScene m_scene;
    QString m_runtimeTrace;
    QList<ScreenOverlayGraphicsItem*> m_screenOverlays;

    QHash<qint64, OsmLineStringItems> m_osmLineStringItems;
    int m_tileLevel;
    GeoGraphicsItem* m_lastFeatureAt;

    bool m_dirty;
    int m_cachedItemCount;
    QHash<QString, GeoGraphicItems> m_cachedPaintFragments;
    typedef QPair<QString, GeoGraphicsItem*> LayerItem;
    QList<LayerItem> m_cachedDefaultLayer;
    QDateTime m_cachedDateTime;
    GeoDataLatLonBox m_cachedLatLonBox;
    QSet<qint64> m_highlightedRouteRelations;
    bool m_showPublicTransport;
};

GeometryLayerPrivate::GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder) :
    m_model(model),
    m_styleBuilder(styleBuilder),
    m_tileLevel(0),
    m_lastFeatureAt(nullptr),
    m_dirty(true),
    m_cachedItemCount(0),
    m_showPublicTransport(false)
{
}

void GeometryLayerPrivate::createGraphicsItems(const GeoDataObject *object)
{
    FeatureRelationHash noRelations;
    createGraphicsItems(object, noRelations);
}

GeometryLayer::GeometryLayer(const QAbstractItemModel *model, const StyleBuilder *styleBuilder) :
    d(new GeometryLayerPrivate(model, styleBuilder))
{
    const GeoDataObject *object = static_cast<GeoDataObject*>(d->m_model->index(0, 0, QModelIndex()).internalPointer());
    if (object && object->parent()) {
        d->createGraphicsItems(object->parent());
    }

    connect(model, SIGNAL(dataChanged(QModelIndex, QModelIndex)),
            this, SLOT(resetCacheData()));
    connect(model, SIGNAL(rowsInserted(QModelIndex, int, int)),
            this, SLOT(addPlacemarks(QModelIndex, int, int)));
    connect(model, SIGNAL(rowsAboutToBeRemoved(QModelIndex, int, int)),
            this, SLOT(removePlacemarks(QModelIndex, int, int)));
    connect(model, SIGNAL(modelReset()),
            this, SLOT(resetCacheData()));
    connect(this, SIGNAL(highlightedPlacemarksChanged(QVector<GeoDataPlacemark*>)),
            &d->m_scene, SLOT(applyHighlight(QVector<GeoDataPlacemark*>)));
    connect(&d->m_scene, SIGNAL(repaintNeeded()),
            this, SIGNAL(repaintNeeded()));
}

GeometryLayer::~GeometryLayer()
{
    delete d;
}

QStringList GeometryLayer::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}


bool GeometryLayer::render(GeoPainter *painter, ViewportParams *viewport,
                           const QString& renderPos, GeoSceneLayer * layer)
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    painter->save();

    auto const & box = viewport->viewLatLonAltBox();
    bool isEqual = GeoDataLatLonBox::fuzzyCompare(d->m_cachedLatLonBox, box, 0.05);

    if (d->m_cachedLatLonBox.isEmpty() || !isEqual) {
        d->m_dirty = true;
    }

    // update the items cache at least every second since the last request
    auto const now = QDateTime::currentDateTime();
    if (!d->m_cachedDateTime.isValid() || d->m_cachedDateTime.msecsTo(now) > 1000) {
        d->m_dirty = true;
    }

    if (d->m_dirty) {
        d->m_dirty = false;

        const int maxZoomLevel = qMin(d->m_tileLevel, d->m_styleBuilder->maximumZoomLevel());
        auto const items = d->m_scene.items(box, maxZoomLevel);;
        d->m_cachedLatLonBox = box;
        d->m_cachedDateTime = now;

        d->m_cachedItemCount = items.size();
        d->m_cachedDefaultLayer.clear();
        d->m_cachedPaintFragments.clear();
        QHash<QString, GeometryLayerPrivate::PaintFragments> paintFragments;
        QSet<QString> const knownLayers = QSet<QString>::fromList(d->m_styleBuilder->renderOrder());
        for (GeoGraphicsItem* item: items) {
            QStringList paintLayers = item->paintLayers();
            if (paintLayers.isEmpty()) {
                mDebug() << item << " provides no paint layers, so I force one onto it.";
                paintLayers << QString();
            }
            for (const auto &layer: paintLayers) {
                if (knownLayers.contains(layer)) {
                    GeometryLayerPrivate::PaintFragments &fragments = paintFragments[layer];
                    double const zValue = item->zValue();
                    // assign subway stations
                    if (zValue == 0.0) {
                        fragments.null << item;
                        // assign areas and streets
                    } else if (zValue < 0.0) {
                        fragments.negative << item;
                        // assign buildings
                    } else {
                        fragments.positive << item;
                    }
                } else {
                    // assign symbols
                    d->m_cachedDefaultLayer << GeometryLayerPrivate::LayerItem(layer, item);
                    static QSet<QString> missingLayers;
                    if (!missingLayers.contains(layer)) {
                        mDebug() << "Missing layer " << layer << ", in render order, will render it on top";
                        missingLayers << layer;
                    }
                }
            }
        }
        // Sort each fragment by z-level
        for (const QString &layer: d->m_styleBuilder->renderOrder()) {
            GeometryLayerPrivate::PaintFragments & layerItems = paintFragments[layer];
            std::sort(layerItems.negative.begin(), layerItems.negative.end(), GeoGraphicsItem::zValueLessThan);
            // The idea here is that layerItems.null has most items and does not need to be sorted by z-value
            // since they are all equal (=0). We do sort them by style pointer though for batch rendering
            std::sort(layerItems.null.begin(), layerItems.null.end(), GeoGraphicsItem::styleLessThan);
            std::sort(layerItems.positive.begin(), layerItems.positive.end(), GeoGraphicsItem::zValueAndStyleLessThan);
            auto const count = layerItems.negative.size() + layerItems.null.size() + layerItems.positive.size();
            d->m_cachedPaintFragments[layer].reserve(count);
            d->m_cachedPaintFragments[layer] << layerItems.negative;
            d->m_cachedPaintFragments[layer] << layerItems.null;
            d->m_cachedPaintFragments[layer] << layerItems.positive;
        }
    }

    for (const QString &layer: d->m_styleBuilder->renderOrder()) {
        auto & layerItems = d->m_cachedPaintFragments[layer];
        AbstractGeoPolygonGraphicsItem::s_previousStyle = 0;
        GeoLineStringGraphicsItem::s_previousStyle = 0;
        for (auto item: layerItems) {
            item->paint(painter, viewport, layer, d->m_tileLevel);
        }
    }

    for (const auto & item: d->m_cachedDefaultLayer) {
        item.second->paint(painter, viewport, item.first, d->m_tileLevel);
    }

    for (ScreenOverlayGraphicsItem* item: d->m_screenOverlays) {
        item->paintEvent(painter, viewport);
    }

    painter->restore();
    d->m_runtimeTrace = QStringLiteral("Geometries: %1 Zoom: %2")
                        .arg(d->m_cachedItemCount)
                        .arg(d->m_tileLevel);
    return true;
}

RenderState GeometryLayer::renderState() const
{
    return RenderState(QStringLiteral("GeoGraphicsScene"));
}

QString GeometryLayer::runtimeTrace() const
{
    return d->m_runtimeTrace;
}

bool GeometryLayer::hasFeatureAt(const QPoint &curpos, const ViewportParams *viewport)
{
    if (d->m_lastFeatureAt && d->m_lastFeatureAt->contains(curpos, viewport)) {
        return true;
    }

    auto const renderOrder = d->m_styleBuilder->renderOrder();
    for (int i = renderOrder.size() - 1; i >= 0; --i) {
        auto & layerItems = d->m_cachedPaintFragments[renderOrder[i]];
        for (auto item : layerItems) {
            if (item->contains(curpos, viewport)) {
                d->m_lastFeatureAt = item;
                return true;
            }
        }
    }

    return false;
}

void GeometryLayerPrivate::createGraphicsItems(const GeoDataObject *object, FeatureRelationHash &relations)
{
    clearCache();
    if (auto document = geodata_cast<GeoDataDocument>(object)) {
        for (auto feature: document->featureList()) {
            if (auto relation = geodata_cast<GeoDataRelation>(feature)) {
                relation->setVisible(showRelation(relation));
                for (auto member: relation->members()) {
                    relations[member] << relation;
                }
            }
        }
    }
    if (auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
        createGraphicsItemFromGeometry(placemark->geometry(), placemark, relations.value(placemark));
    } else if (const GeoDataOverlay* overlay = dynamic_cast<const GeoDataOverlay*>(object)) {
        createGraphicsItemFromOverlay(overlay);
    }

    // parse all child objects of the container
    if (const GeoDataContainer *container = dynamic_cast<const GeoDataContainer*>(object)) {
        int rowCount = container->size();
        for (int row = 0; row < rowCount; ++row) {
            createGraphicsItems(container->child(row), relations);
        }
    }
}

void GeometryLayerPrivate::updateTiledLineStrings(const GeoDataPlacemark* placemark, GeoLineStringGraphicsItem* lineStringItem)
{
    if (!placemark->hasOsmData()) {
        return;
    }
    qint64 const osmId = placemark->osmData().oid();
    if (osmId <= 0) {
        return;
    }
    auto & lineStringItems = m_osmLineStringItems[osmId];
    lineStringItems << lineStringItem;
    updateTiledLineStrings(lineStringItems);
}

void GeometryLayerPrivate::updateTiledLineStrings(OsmLineStringItems &lineStringItems)
{
    GeoDataLineString merged;
    if (lineStringItems.size() > 1) {
        QVector<const GeoDataLineString*> lineStrings;
        for (auto item : lineStringItems) {
            lineStrings << item->lineString();
        }
        merged = GeoLineStringGraphicsItem::merge(lineStrings);
    }

    // If merging failed, reset all. Otherwise only the first one
    // gets the merge result and becomes visible.
    bool visible = true;
    for (auto item : lineStringItems) {
        item->setVisible(visible);
        if (visible) {
            item->setMergedLineString(merged);
            visible = merged.isEmpty();
        }
    }
}

void GeometryLayerPrivate::clearCache()
{
    m_lastFeatureAt = nullptr;
    m_dirty = true;
    m_cachedDateTime = QDateTime();
    m_cachedItemCount = 0;
    m_cachedPaintFragments.clear();
    m_cachedDefaultLayer.clear();
    m_cachedLatLonBox = GeoDataLatLonBox();
}

inline bool GeometryLayerPrivate::showRelation(const GeoDataRelation *relation) const
{
    return (m_showPublicTransport
            && relation->relationType() >= GeoDataRelation::RouteTrain
            && relation->relationType() <= GeoDataRelation::RouteTrolleyBus)
            || m_highlightedRouteRelations.contains(relation->osmData().oid());
}

void GeometryLayerPrivate::updateRelationVisibility()
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QVariant const data = m_model->data(m_model->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
        GeoDataObject *object = qvariant_cast<GeoDataObject*> (data);
        if (auto doc = geodata_cast<GeoDataDocument>(object)) {
            for (auto feature: doc->featureList()) {
                if (auto relation = geodata_cast<GeoDataRelation>(feature)) {
                    relation->setVisible(showRelation(relation));
                }
            }
        }
    }
    m_scene.resetStyle();
}

void GeometryLayerPrivate::createGraphicsItemFromGeometry(const GeoDataGeometry* object, const GeoDataPlacemark *placemark, const Relations &relations)
{
    if (!placemark->isGloballyVisible()) {
        return; // Reconsider this when visibility can be changed dynamically
    }

    GeoGraphicsItem *item = 0;
    if (const auto line = geodata_cast<GeoDataLineString>(object)) {
        auto lineStringItem = new GeoLineStringGraphicsItem(placemark, line);
        item = lineStringItem;
        updateTiledLineStrings(placemark, lineStringItem);
    } else if (const auto ring = geodata_cast<GeoDataLinearRing>(object)) {
        item = GeoPolygonGraphicsItem::createGraphicsItem(placemark, ring);
    } else if (const auto poly = geodata_cast<GeoDataPolygon>(object)) {
        item = GeoPolygonGraphicsItem::createGraphicsItem(placemark, poly);
        if (item->zValue() == 0) {
            item->setZValue(poly->renderOrder());
        }
    } else if (const auto multigeo = geodata_cast<GeoDataMultiGeometry>(object)) {
        int rowCount = multigeo->size();
        for (int row = 0; row < rowCount; ++row) {
            createGraphicsItemFromGeometry(multigeo->child(row), placemark, relations);
        }
    } else if (const auto multitrack = geodata_cast<GeoDataMultiTrack>(object)) {
        int rowCount = multitrack->size();
        for (int row = 0; row < rowCount; ++row) {
            createGraphicsItemFromGeometry(multitrack->child(row), placemark, relations);
        }
    } else if (const auto track = geodata_cast<GeoDataTrack>(object)) {
        item = new GeoTrackGraphicsItem(placemark, track);
    }
    if (!item) {
        return;
    }
    item->setRelations(relations);
    item->setStyleBuilder(m_styleBuilder);
    item->setVisible(item->visible() && placemark->isGloballyVisible());
    item->setMinZoomLevel(m_styleBuilder->minimumZoomLevel(*placemark));
    m_scene.addItem(item);
}

void GeometryLayerPrivate::createGraphicsItemFromOverlay(const GeoDataOverlay *overlay)
{
    if (!overlay->isGloballyVisible()) {
        return; // Reconsider this when visibility can be changed dynamically
    }

    GeoGraphicsItem* item = 0;
    if (const auto photoOverlay = geodata_cast<GeoDataPhotoOverlay>(overlay)) {
        GeoPhotoGraphicsItem *photoItem = new GeoPhotoGraphicsItem(overlay);
        photoItem->setPoint(photoOverlay->point());
        item = photoItem;
    } else if (const auto screenOverlay = geodata_cast<GeoDataScreenOverlay>(overlay)) {
        ScreenOverlayGraphicsItem *screenItem = new ScreenOverlayGraphicsItem(screenOverlay);
        m_screenOverlays.push_back(screenItem);
    }

    if (item) {
        item->setStyleBuilder(m_styleBuilder);
        item->setVisible(overlay->isGloballyVisible());
        m_scene.addItem(item);
    }
}

void GeometryLayerPrivate::removeGraphicsItems(const GeoDataFeature *feature)
{
    clearCache();
    if (const auto placemark = geodata_cast<GeoDataPlacemark>(feature)) {
        if (placemark->isGloballyVisible() &&
            geodata_cast<GeoDataLineString>(placemark->geometry()) &&
            placemark->hasOsmData() &&
            placemark->osmData().oid() > 0) {
            auto & items = m_osmLineStringItems[placemark->osmData().oid()];
            bool removed = false;
            for (auto item : items) {
                if (item->feature() == feature) {
                    items.removeOne(item);
                    removed = true;
                    break;
                }
            }
            Q_ASSERT(removed);
            updateTiledLineStrings(items);
        }
        m_scene.removeItem(feature);
    } else if (const auto container = dynamic_cast<const GeoDataContainer*>(feature)) {
        for (const GeoDataFeature *child: container->featureList()) {
            removeGraphicsItems(child);
        }
    } else if (geodata_cast<GeoDataScreenOverlay>(feature)) {
        for (ScreenOverlayGraphicsItem  *item: m_screenOverlays) {
            if (item->screenOverlay() == feature) {
                m_screenOverlays.removeAll(item);
            }
        }
    }
}

void GeometryLayer::addPlacemarks(const QModelIndex& parent, int first, int last)
{
    Q_ASSERT(first < d->m_model->rowCount(parent));
    Q_ASSERT(last < d->m_model->rowCount(parent));
    for (int i = first; i <= last; ++i) {
        QModelIndex index = d->m_model->index(i, 0, parent);
        Q_ASSERT(index.isValid());
        const GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data(MarblePlacemarkModel::ObjectPointerRole));
        Q_ASSERT(object);
        d->createGraphicsItems(object);
    }
    emit repaintNeeded();

}

void GeometryLayer::removePlacemarks(const QModelIndex& parent, int first, int last)
{
    Q_ASSERT(last < d->m_model->rowCount(parent));
    bool isRepaintNeeded = false;
    for (int i = first; i <= last; ++i) {
        QModelIndex index = d->m_model->index(i, 0, parent);
        Q_ASSERT(index.isValid());
        const GeoDataObject *object = qvariant_cast<GeoDataObject*>(index.data(MarblePlacemarkModel::ObjectPointerRole));
        const GeoDataFeature *feature = dynamic_cast<const GeoDataFeature*>(object);
        if (feature != 0) {
            d->removeGraphicsItems(feature);
            isRepaintNeeded = true;
        }
    }
    if (isRepaintNeeded) {
        emit repaintNeeded();
    }

}

void GeometryLayer::resetCacheData()
{
    d->clearCache();
    d->m_scene.clear();
    qDeleteAll(d->m_screenOverlays);
    d->m_screenOverlays.clear();
    d->m_osmLineStringItems.clear();

    const GeoDataObject *object = static_cast<GeoDataObject*>(d->m_model->index(0, 0, QModelIndex()).internalPointer());
    if (object && object->parent()) {
        d->createGraphicsItems(object->parent());
    }
    emit repaintNeeded();
}

void GeometryLayer::setTileLevel(int tileLevel)
{
    d->m_tileLevel = tileLevel;
}

QVector<const GeoDataFeature*> GeometryLayer::whichFeatureAt(const QPoint &curpos, const ViewportParams *viewport)
{
    QVector<const GeoDataFeature*> result;
    auto const renderOrder = d->m_styleBuilder->renderOrder();
    QString const label = QStringLiteral("/label");
    QSet<GeoGraphicsItem*> checked;
    for (int i = renderOrder.size()-1; i >= 0; --i) {
        if (renderOrder[i].endsWith(label)) {
            continue;
        }
        auto & layerItems = d->m_cachedPaintFragments[renderOrder[i]];
        for (auto j = layerItems.size()-1; j >= 0; --j) {
            auto const & layerItem = layerItems[j];
            if (!checked.contains(layerItem)) {
                if (layerItem->contains(curpos, viewport)) {
                    result << layerItem->feature();
                }
                checked << layerItem;
            }
        }
    }

    return result;
}

void GeometryLayer::highlightRouteRelation(qint64 osmId, bool enabled)
{
    if (enabled) {
        d->m_highlightedRouteRelations << osmId;
    } else {
        d->m_highlightedRouteRelations.remove(osmId);
    }
    d->updateRelationVisibility();
}

void GeometryLayer::setShowPublicTransport(bool showPublicTransport)
{
    if (showPublicTransport == d->m_showPublicTransport) {
        return;
    }

    d->m_showPublicTransport = showPublicTransport;
    d->updateRelationVisibility();
}

void GeometryLayer::handleHighlight(qreal lon, qreal lat, GeoDataCoordinates::Unit unit)
{
    GeoDataCoordinates clickedPoint(lon, lat, 0, unit);
    QVector<GeoDataPlacemark*> selectedPlacemarks;

    for (int i = 0; i < d->m_model->rowCount(); ++i) {
        QVariant const data = d->m_model->data(d->m_model->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
        GeoDataObject *object = qvariant_cast<GeoDataObject*> (data);
        Q_ASSERT(object);
        if (const auto doc = geodata_cast<GeoDataDocument>(object)) {
            bool isHighlight = false;

            for (const GeoDataStyleMap &styleMap: doc->styleMaps()) {
                if (styleMap.contains(QStringLiteral("highlight"))) {
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
            if (isHighlight) {
                QVector<GeoDataFeature*>::Iterator iter = doc->begin();
                QVector<GeoDataFeature*>::Iterator const end = doc->end();

                for (; iter != end; ++iter) {
                    if (auto placemark = geodata_cast<GeoDataPlacemark>(*iter)) {
                        GeoDataPolygon *polygon = dynamic_cast<GeoDataPolygon*>(placemark->geometry());
                        if (polygon &&
                            polygon->contains(clickedPoint)) {
                            selectedPlacemarks.push_back(placemark);
                        }

                        if (auto linearRing = geodata_cast<GeoDataLinearRing>(placemark->geometry())) {
                            if (linearRing->contains(clickedPoint)) {
                                selectedPlacemarks.push_back(placemark);
                            }
                        }

                        if (auto multiGeometry = geodata_cast<GeoDataMultiGeometry>(placemark->geometry())) {
                            QVector<GeoDataGeometry*>::Iterator multiIter = multiGeometry->begin();
                            QVector<GeoDataGeometry*>::Iterator const multiEnd = multiGeometry->end();

                            for (; multiIter != multiEnd; ++multiIter) {
                                GeoDataPolygon *poly = dynamic_cast<GeoDataPolygon*>(*multiIter);

                                if (poly &&
                                    poly->contains(clickedPoint)) {
                                    selectedPlacemarks.push_back(placemark);
                                    break;
                                }

                                if (auto linearRing = geodata_cast<GeoDataLinearRing>(*multiIter)) {
                                    if (linearRing->contains(clickedPoint)) {
                                        selectedPlacemarks.push_back(placemark);
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

    emit highlightedPlacemarksChanged(selectedPlacemarks);
}

}

#include "moc_GeometryLayer.cpp"
