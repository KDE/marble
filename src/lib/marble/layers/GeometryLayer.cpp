// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2014 Gábor Péterffy <peterffy95@gmail.com>
//

#include "GeometryLayer.h"

// Marble
#include "AbstractGeoPolygonGraphicsItem.h"
#include "GeoDataBuilding.h"
#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineStyle.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataMultiTrack.h"
#include "GeoDataObject.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataPolygon.h"
#include "GeoDataScreenOverlay.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataTreeModel.h"
#include "GeoGraphicsItem.h"
#include "GeoGraphicsScene.h"
#include "GeoLineStringGraphicsItem.h"
#include "GeoPainter.h"
#include "GeoPhotoGraphicsItem.h"
#include "GeoPolygonGraphicsItem.h"
#include "GeoTrackGraphicsItem.h"
#include "MarbleDebug.h"
#include "MarbleGraphicsItem.h"
#include "MarblePlacemarkModel.h"
#include "RenderState.h"
#include "ScreenOverlayGraphicsItem.h"
#include "StyleBuilder.h"
#include "TileId.h"
#include "ViewportParams.h"
#include <OsmPlacemarkData.h>

// Qt
#include <QAbstractItemModel>
#include <QModelIndex>
#include <qmath.h>

namespace Marble
{
class GeometryLayerPrivate
{
public:
    using OsmLineStringItems = QList<GeoLineStringGraphicsItem *>;
    using Relations = QSet<const GeoDataRelation *>;
    using FeatureRelationHash = QHash<const GeoDataFeature *, Relations>;
    using GeoGraphicItems = QList<GeoGraphicsItem *>;

    struct PaintFragments {
        // Three lists for different z values
        // A z value of 0 is default and used by the majority of items, so sorting
        // can be avoided for it
        QList<GeoGraphicsItem *> negative; // subways
        QList<GeoGraphicsItem *> null; // areas and roads
        QList<GeoGraphicsItem *> positive; // buildings
    };

    explicit GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder);

    void createGraphicsItems(const GeoDataObject *object);
    void createGraphicsItems(const GeoDataObject *object, FeatureRelationHash &relations);
    void createGraphicsItemFromGeometry(const GeoDataGeometry *object, const GeoDataPlacemark *placemark, const Relations &relations);
    void createGraphicsItemFromOverlay(const GeoDataOverlay *overlay);
    void removeGraphicsItems(const GeoDataFeature *feature);
    void updateTiledLineStrings(const GeoDataPlacemark *placemark, GeoLineStringGraphicsItem *lineStringItem);
    static void updateTiledLineStrings(OsmLineStringItems &lineStringItems);
    void clearCache();
    bool showRelation(const GeoDataRelation *relation) const;
    void updateRelationVisibility();

    const QAbstractItemModel *const m_model;
    const StyleBuilder *const m_styleBuilder;
    GeoGraphicsScene m_scene;
    QString m_runtimeTrace;
    QList<ScreenOverlayGraphicsItem *> m_screenOverlays;

    QHash<qint64, OsmLineStringItems> m_osmLineStringItems;
    int m_tileLevel;
    GeoGraphicsItem *m_lastFeatureAt;

    bool m_dirty;
    int m_cachedItemCount;
    QHash<QString, GeoGraphicItems> m_cachedPaintFragments;
    using LayerItem = QPair<QString, GeoGraphicsItem *>;
    QList<LayerItem> m_cachedDefaultLayer;
    QDateTime m_cachedDateTime;
    GeoDataLatLonBox m_cachedLatLonBox;
    QSet<qint64> m_highlightedRouteRelations;
    GeoDataRelation::RelationTypes m_visibleRelationTypes;
    bool m_levelTagDebugModeEnabled;
    int m_debugLevelTag;
};

GeometryLayerPrivate::GeometryLayerPrivate(const QAbstractItemModel *model, const StyleBuilder *styleBuilder)
    : m_model(model)
    , m_styleBuilder(styleBuilder)
    , m_tileLevel(0)
    , m_lastFeatureAt(nullptr)
    , m_dirty(true)
    , m_cachedItemCount(0)
    , m_visibleRelationTypes(GeoDataRelation::RouteFerry)
    , m_levelTagDebugModeEnabled(false)
    , m_debugLevelTag(0)
{
}

void GeometryLayerPrivate::createGraphicsItems(const GeoDataObject *object)
{
    FeatureRelationHash noRelations;
    createGraphicsItems(object, noRelations);
}

GeometryLayer::GeometryLayer(const QAbstractItemModel *model, const StyleBuilder *styleBuilder)
    : d(std::make_unique<GeometryLayerPrivate>(model, styleBuilder))
{
    const GeoDataObject *object = static_cast<GeoDataObject *>(d->m_model->index(0, 0, QModelIndex()).internalPointer());
    if (object && object->parent()) {
        d->createGraphicsItems(object->parent());
    }

    connect(model, &QAbstractItemModel::dataChanged, this, &GeometryLayer::resetCacheData);
    connect(model, &QAbstractItemModel::rowsInserted, this, &GeometryLayer::addPlacemarks);
    connect(model, &QAbstractItemModel::rowsAboutToBeRemoved, this, &GeometryLayer::removePlacemarks);
    connect(model, &QAbstractItemModel::modelReset, this, &GeometryLayer::resetCacheData);
    connect(this, &GeometryLayer::highlightedPlacemarksChanged, &d->m_scene, &GeoGraphicsScene::applyHighlight);
    connect(&d->m_scene, &GeoGraphicsScene::repaintNeeded, this, &GeometryLayer::repaintNeeded);
}

GeometryLayer::~GeometryLayer() = default;

QStringList GeometryLayer::renderPosition() const
{
    return QStringList(QStringLiteral("HOVERS_ABOVE_SURFACE"));
}

bool GeometryLayer::render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos, GeoSceneLayer *layer)
{
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    painter->save();

    auto const &box = viewport->viewLatLonAltBox();
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
        auto const items = d->m_scene.items(box, maxZoomLevel);
        d->m_cachedLatLonBox = box;
        d->m_cachedDateTime = now;

        d->m_cachedItemCount = items.size();
        d->m_cachedDefaultLayer.clear();
        d->m_cachedPaintFragments.clear();
        QHash<QString, GeometryLayerPrivate::PaintFragments> paintFragments;
        const QStringList &renderOrder = d->m_styleBuilder->renderOrder();
        QSet<QString> const knownLayers(renderOrder.constBegin(), renderOrder.constEnd());
        for (GeoGraphicsItem *item : items) {
            QStringList paintLayers = item->paintLayers();
            if (paintLayers.isEmpty()) {
                mDebug() << item << " provides no paint layers, so I force one onto it.";
                paintLayers << QString();
            }
            for (const auto &layer : std::as_const(paintLayers)) {
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
        const auto layers = d->m_styleBuilder->renderOrder();
        for (const QString &layer : layers) {
            GeometryLayerPrivate::PaintFragments &layerItems = paintFragments[layer];
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

    const auto layers = d->m_styleBuilder->renderOrder();
    for (const QString &layer : layers) {
        auto &layerItems = d->m_cachedPaintFragments[layer];
        AbstractGeoPolygonGraphicsItem::s_previousStyle = nullptr;
        GeoLineStringGraphicsItem::s_previousStyle = nullptr;
        for (auto item : std::as_const(layerItems)) {
            if (d->m_levelTagDebugModeEnabled) {
                if (const auto placemark = geodata_cast<GeoDataPlacemark>(item->feature())) {
                    if (placemark->hasOsmData()) {
                        QHash<QString, QString>::const_iterator tagIter = placemark->osmData().findTag(QStringLiteral("level"));
                        if (tagIter != placemark->osmData().tagsEnd()) {
                            const int val = tagIter.value().toInt();
                            if (val != d->m_debugLevelTag) {
                                continue;
                            }
                        }
                    }
                }
            }
            item->paint(painter, viewport, layer, d->m_tileLevel);
        }
    }

    for (const auto &item : std::as_const(d->m_cachedDefaultLayer)) {
        item.second->paint(painter, viewport, item.first, d->m_tileLevel);
    }

    for (ScreenOverlayGraphicsItem *item : std::as_const(d->m_screenOverlays)) {
        item->paintEvent(painter, viewport);
    }

    painter->restore();
    d->m_runtimeTrace = QStringLiteral("Geometries: %1 Zoom: %2").arg(d->m_cachedItemCount).arg(d->m_tileLevel);
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
        auto &layerItems = d->m_cachedPaintFragments[renderOrder[i]];
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
        const auto features = document->featureList();
        for (auto feature : features) {
            if (auto relation = geodata_cast<GeoDataRelation>(feature)) {
                relation->setVisible(showRelation(relation));
                const auto members = relation->members();
                for (const auto &member : members) {
                    relations[member] << relation;
                }
            }
        }
    }
    if (auto placemark = geodata_cast<GeoDataPlacemark>(object)) {
        createGraphicsItemFromGeometry(placemark->geometry(), placemark, relations.value(placemark));
    } else if (const auto overlay = dynamic_cast<const GeoDataOverlay *>(object)) {
        createGraphicsItemFromOverlay(overlay);
    }

    // parse all child objects of the container
    if (const auto container = dynamic_cast<const GeoDataContainer *>(object)) {
        int rowCount = container->size();
        for (int row = 0; row < rowCount; ++row) {
            createGraphicsItems(container->child(row), relations);
        }
    }
}

void GeometryLayerPrivate::updateTiledLineStrings(const GeoDataPlacemark *placemark, GeoLineStringGraphicsItem *lineStringItem)
{
    if (!placemark->hasOsmData()) {
        return;
    }
    qint64 const osmId = placemark->osmData().oid();
    if (osmId <= 0) {
        return;
    }
    auto &lineStringItems = m_osmLineStringItems[osmId];
    lineStringItems << lineStringItem;
    updateTiledLineStrings(lineStringItems);
}

void GeometryLayerPrivate::updateTiledLineStrings(OsmLineStringItems &lineStringItems)
{
    GeoDataLineString merged;
    if (lineStringItems.size() > 1) {
        QList<const GeoDataLineString *> lineStrings;
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
    return (m_visibleRelationTypes.testFlag(relation->relationType()) || m_highlightedRouteRelations.contains(relation->osmData().oid()));
}

void GeometryLayerPrivate::updateRelationVisibility()
{
    for (int i = 0; i < m_model->rowCount(); ++i) {
        QVariant const data = m_model->data(m_model->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
        auto object = qvariant_cast<GeoDataObject *>(data);
        if (auto doc = geodata_cast<GeoDataDocument>(object)) {
            const auto features = doc->featureList();
            for (auto feature : features) {
                if (auto relation = geodata_cast<GeoDataRelation>(feature)) {
                    relation->setVisible(showRelation(relation));
                }
            }
        }
    }
    m_scene.resetStyle();
}

void GeometryLayerPrivate::createGraphicsItemFromGeometry(const GeoDataGeometry *object, const GeoDataPlacemark *placemark, const Relations &relations)
{
    if (!placemark->isGloballyVisible()) {
        return; // Reconsider this when visibility can be changed dynamically
    }

    GeoGraphicsItem *item = nullptr;
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
    } else if (const auto building = geodata_cast<GeoDataBuilding>(object)) {
        item = GeoPolygonGraphicsItem::createGraphicsItem(placemark, building);
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

    GeoGraphicsItem *item = nullptr;
    if (const auto photoOverlay = geodata_cast<GeoDataPhotoOverlay>(overlay)) {
        auto photoItem = new GeoPhotoGraphicsItem(overlay);
        photoItem->setPoint(photoOverlay->point());
        item = photoItem;
    } else if (const auto screenOverlay = geodata_cast<GeoDataScreenOverlay>(overlay)) {
        auto screenItem = new ScreenOverlayGraphicsItem(screenOverlay);
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
        if (placemark->isGloballyVisible() && geodata_cast<GeoDataLineString>(placemark->geometry()) && placemark->hasOsmData()
            && placemark->osmData().oid() > 0) {
            auto &items = m_osmLineStringItems[placemark->osmData().oid()];
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
    } else if (const auto container = dynamic_cast<const GeoDataContainer *>(feature)) {
        const auto features = container->featureList();
        for (const GeoDataFeature *child : features) {
            removeGraphicsItems(child);
        }
    } else if (geodata_cast<GeoDataScreenOverlay>(feature)) {
        for (ScreenOverlayGraphicsItem *item : std::as_const(m_screenOverlays)) {
            if (item->screenOverlay() == feature) {
                m_screenOverlays.removeAll(item);
            }
        }
    }
}

void GeometryLayer::addPlacemarks(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(first < d->m_model->rowCount(parent));
    Q_ASSERT(last < d->m_model->rowCount(parent));
    for (int i = first; i <= last; ++i) {
        QModelIndex index = d->m_model->index(i, 0, parent);
        Q_ASSERT(index.isValid());
        const GeoDataObject *object = qvariant_cast<GeoDataObject *>(index.data(MarblePlacemarkModel::ObjectPointerRole));
        Q_ASSERT(object);
        d->createGraphicsItems(object);
    }
    Q_EMIT repaintNeeded();
}

void GeometryLayer::removePlacemarks(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(last < d->m_model->rowCount(parent));
    bool isRepaintNeeded = false;
    for (int i = first; i <= last; ++i) {
        QModelIndex index = d->m_model->index(i, 0, parent);
        Q_ASSERT(index.isValid());
        const GeoDataObject *object = qvariant_cast<GeoDataObject *>(index.data(MarblePlacemarkModel::ObjectPointerRole));
        const auto feature = dynamic_cast<const GeoDataFeature *>(object);
        if (feature != nullptr) {
            d->removeGraphicsItems(feature);
            isRepaintNeeded = true;
        }
    }
    if (isRepaintNeeded) {
        Q_EMIT repaintNeeded();
    }
}

void GeometryLayer::resetCacheData()
{
    d->clearCache();
    d->m_scene.clear();
    qDeleteAll(d->m_screenOverlays);
    d->m_screenOverlays.clear();
    d->m_osmLineStringItems.clear();

    const GeoDataObject *object = static_cast<GeoDataObject *>(d->m_model->index(0, 0, QModelIndex()).internalPointer());
    if (object && object->parent()) {
        d->createGraphicsItems(object->parent());
    }
    Q_EMIT repaintNeeded();
}

void GeometryLayer::setTileLevel(int tileLevel)
{
    d->m_tileLevel = tileLevel;
}

QList<const GeoDataFeature *> GeometryLayer::whichFeatureAt(const QPoint &curpos, const ViewportParams *viewport)
{
    QList<const GeoDataFeature *> result;
    auto const renderOrder = d->m_styleBuilder->renderOrder();
    QString const label = QStringLiteral("/label");
    QSet<GeoGraphicsItem *> checked;
    for (int i = renderOrder.size() - 1; i >= 0; --i) {
        if (renderOrder[i].endsWith(label)) {
            continue;
        }
        auto &layerItems = d->m_cachedPaintFragments[renderOrder[i]];
        for (auto j = layerItems.size() - 1; j >= 0; --j) {
            auto const &layerItem = layerItems[j];
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

void GeometryLayer::setVisibleRelationTypes(GeoDataRelation::RelationTypes relationTypes)
{
    if (relationTypes != d->m_visibleRelationTypes) {
        d->m_visibleRelationTypes = relationTypes;
        d->updateRelationVisibility();
    }
}

void GeometryLayer::handleHighlight(qreal lon, qreal lat, GeoDataCoordinates::Unit unit)
{
    GeoDataCoordinates clickedPoint(lon, lat, 0, unit);
    QList<GeoDataPlacemark *> selectedPlacemarks;

    for (int i = 0; i < d->m_model->rowCount(); ++i) {
        QVariant const data = d->m_model->data(d->m_model->index(i, 0), MarblePlacemarkModel::ObjectPointerRole);
        auto object = qvariant_cast<GeoDataObject *>(data);
        Q_ASSERT(object);
        if (const auto doc = geodata_cast<GeoDataDocument>(object)) {
            bool isHighlight = false;

            const auto styles = doc->styleMaps();
            for (const GeoDataStyleMap &styleMap : styles) {
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
                QList<GeoDataFeature *>::Iterator iter = doc->begin();
                QList<GeoDataFeature *>::Iterator const end = doc->end();

                for (; iter != end; ++iter) {
                    if (auto placemark = geodata_cast<GeoDataPlacemark>(*iter)) {
                        auto polygon = dynamic_cast<GeoDataPolygon *>(placemark->geometry());
                        if (polygon && polygon->contains(clickedPoint)) {
                            selectedPlacemarks.push_back(placemark);
                        }

                        if (auto linearRing = geodata_cast<GeoDataLinearRing>(placemark->geometry())) {
                            if (linearRing->contains(clickedPoint)) {
                                selectedPlacemarks.push_back(placemark);
                            }
                        }

                        if (auto multiGeometry = geodata_cast<GeoDataMultiGeometry>(placemark->geometry())) {
                            QList<GeoDataGeometry *>::Iterator multiIter = multiGeometry->begin();
                            QList<GeoDataGeometry *>::Iterator const multiEnd = multiGeometry->end();

                            for (; multiIter != multiEnd; ++multiIter) {
                                auto poly = dynamic_cast<GeoDataPolygon *>(*multiIter);

                                if (poly && poly->contains(clickedPoint)) {
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

    Q_EMIT highlightedPlacemarksChanged(selectedPlacemarks);
}

void GeometryLayer::setLevelTagDebugModeEnabled(bool enabled)
{
    if (d->m_levelTagDebugModeEnabled != enabled) {
        d->m_levelTagDebugModeEnabled = enabled;
        Q_EMIT repaintNeeded();
    }
}

bool GeometryLayer::levelTagDebugModeEnabled() const
{
    return d->m_levelTagDebugModeEnabled;
}

void GeometryLayer::setDebugLevelTag(int level)
{
    if (d->m_debugLevelTag != level) {
        d->m_debugLevelTag = level;
        Q_EMIT repaintNeeded();
    }
}

int GeometryLayer::debugLevelTag() const
{
    return d->m_debugLevelTag;
}

}

#include "moc_GeometryLayer.cpp"
