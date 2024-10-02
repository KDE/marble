// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoGraphicsScene.h"

#include "GeoDataDocument.h"
#include "GeoDataFeature.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoGraphicsItem.h"
#include "MarbleDebug.h"
#include "TileCoordsPyramid.h"
#include "TileId.h"

#include <QMap>
#include <QRect>

namespace Marble
{

class GeoGraphicsScenePrivate
{
public:
    GeoGraphicsScene *q;
    explicit GeoGraphicsScenePrivate(GeoGraphicsScene *parent)
        : q(parent)
    {
    }

    ~GeoGraphicsScenePrivate()
    {
        q->clear();
    }

    using FeatureItemMap = QHash<const GeoDataFeature *, GeoGraphicsItem *>;
    QMultiHash<TileId, FeatureItemMap> m_tiledItems;
    QMultiHash<const GeoDataFeature *, TileId> m_features; // multi hash because multi track and multi geometry insert multiple items

    // Stores the items which have been clicked;
    QList<GeoGraphicsItem *> m_selectedItems;

    static GeoDataStyle::Ptr highlightStyle(const GeoDataDocument *document, const GeoDataStyleMap &styleMap);

    void selectItem(GeoGraphicsItem *item);
    static void applyHighlightStyle(GeoGraphicsItem *item, const GeoDataStyle::Ptr &style);
};

GeoDataStyle::Ptr GeoGraphicsScenePrivate::highlightStyle(const GeoDataDocument *document, const GeoDataStyleMap &styleMap)
{
    // @todo Consider QUrl parsing when external styles are suppported
    QString highlightStyleId = styleMap.value(QStringLiteral("highlight"));
    highlightStyleId.remove(QLatin1Char('#'));
    if (!highlightStyleId.isEmpty()) {
        GeoDataStyle::Ptr highlightStyle(new GeoDataStyle(*document->style(highlightStyleId)));
        return highlightStyle;
    } else {
        return {};
    }
}

void GeoGraphicsScenePrivate::selectItem(GeoGraphicsItem *item)
{
    m_selectedItems.append(item);
}

void GeoGraphicsScenePrivate::applyHighlightStyle(GeoGraphicsItem *item, const GeoDataStyle::Ptr &highlightStyle)
{
    item->setHighlightStyle(highlightStyle);
    item->setHighlighted(true);
}

GeoGraphicsScene::GeoGraphicsScene(QObject *parent)
    : QObject(parent)
    , d(new GeoGraphicsScenePrivate(this))
{
}

GeoGraphicsScene::~GeoGraphicsScene()
{
    delete d;
}

QList<GeoGraphicsItem *> GeoGraphicsScene::items(const GeoDataLatLonBox &box, int zoomLevel) const
{
    if (box.west() > box.east()) {
        // Handle boxes crossing the IDL by splitting it into two separate boxes
        GeoDataLatLonBox left;
        left.setWest(-M_PI);
        left.setEast(box.east());
        left.setNorth(box.north());
        left.setSouth(box.south());

        GeoDataLatLonBox right;
        right.setWest(box.west());
        right.setEast(M_PI);
        right.setNorth(box.north());
        right.setSouth(box.south());

        return items(left, zoomLevel) + items(right, zoomLevel);
    }

    QList<GeoGraphicsItem *> result;
    QRect rect;
    qreal north, south, east, west;
    box.boundaries(north, south, east, west);
    TileId key;

    key = TileId::fromCoordinates(GeoDataCoordinates(west, north, 0), zoomLevel);
    rect.setLeft(key.x());
    rect.setTop(key.y());

    key = TileId::fromCoordinates(GeoDataCoordinates(east, south, 0), zoomLevel);
    rect.setRight(key.x());
    rect.setBottom(key.y());

    TileCoordsPyramid pyramid(0, zoomLevel);
    pyramid.setBottomLevelCoords(rect);

    for (int level = pyramid.topLevel(); level <= pyramid.bottomLevel(); ++level) {
        QRect const coords = pyramid.coords(level);
        int x1, y1, x2, y2;
        coords.getCoords(&x1, &y1, &x2, &y2);
        for (int x = x1; x <= x2; ++x) {
            bool const isBorderX = x == x1 || x == x2;
            for (int y = y1; y <= y2; ++y) {
                bool const isBorder = isBorderX || y == y1 || y == y2;
                const TileId tileId = TileId(0, level, x, y);
                const auto objects = d->m_tiledItems.value(tileId);
                for (GeoGraphicsItem *object : objects) {
                    if (object->minZoomLevel() <= zoomLevel && object->visible()) {
                        if (!isBorder || object->latLonAltBox().intersects(box)) {
                            result.push_back(object);
                        }
                    }
                }
            }
        }
    }

    return result;
}

QList<GeoGraphicsItem *> GeoGraphicsScene::selectedItems() const
{
    return d->m_selectedItems;
}

void GeoGraphicsScene::resetStyle()
{
    for (auto const &items : std::as_const(d->m_tiledItems)) {
        for (auto item : items) {
            item->resetStyle();
        }
    }
    Q_EMIT repaintNeeded();
}

void GeoGraphicsScene::applyHighlight(const QList<GeoDataPlacemark *> &selectedPlacemarks)
{
    /**
     * First set the items, which were selected previously, to
     * use normal style
     */
    for (GeoGraphicsItem *item : std::as_const(d->m_selectedItems)) {
        item->setHighlighted(false);
    }

    // Also clear the list to store the new selected items
    d->m_selectedItems.clear();

    /**
     * Process the placemark. which were under mouse
     * while clicking, and update corresponding graphics
     * items to use highlight style
     */
    for (const GeoDataPlacemark *placemark : selectedPlacemarks) {
        for (auto tileIter = d->m_features.find(placemark); tileIter != d->m_features.end() && tileIter.key() == placemark; ++tileIter) {
            auto const &clickedItemsList = d->m_tiledItems.values(*tileIter);
            for (auto const &clickedItems : clickedItemsList) { // iterate through FeatureItemMap clickedItems (QHash)
                for (auto iter = clickedItems.find(placemark); iter != clickedItems.end(); ++iter) {
                    if (iter.key() == placemark) {
                        const GeoDataObject *parent = placemark->parent();
                        if (parent) {
                            auto item = *iter;
                            if (const auto doc = geodata_cast<GeoDataDocument>(parent)) {
                                QString styleUrl = placemark->styleUrl();
                                styleUrl.remove(QLatin1Char('#'));
                                if (!styleUrl.isEmpty()) {
                                    GeoDataStyleMap const &styleMap = doc->styleMap(styleUrl);
                                    GeoDataStyle::Ptr style = d->highlightStyle(doc, styleMap);
                                    if (style) {
                                        d->selectItem(item);
                                        d->applyHighlightStyle(item, style);
                                    }
                                }

                                /**
                                 * If a placemark is using an inline style instead of a shared
                                 * style ( e.g in case when theme file specifies the colorMap
                                 * attribute ) then highlight it if any of the style maps have a
                                 * highlight styleId
                                 */
                                else {
                                    const auto styleMaps = doc->styleMaps();
                                    for (const GeoDataStyleMap &styleMap : styleMaps) {
                                        GeoDataStyle::Ptr style = d->highlightStyle(doc, styleMap);
                                        if (style) {
                                            d->selectItem(item);
                                            d->applyHighlightStyle(item, style);
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
    }
    Q_EMIT repaintNeeded();
}

void GeoGraphicsScene::removeItem(const GeoDataFeature *feature)
{
    for (auto tileIter = d->m_features.find(feature), end = d->m_features.end(); tileIter != end && tileIter.key() == feature;) {
        auto &tileList = d->m_tiledItems[*tileIter];
        auto iter = tileList.find(feature);
        if (iter != tileList.end()) {
            auto item = iter.value();
            tileIter = d->m_features.erase(tileIter);
            tileList.erase(iter);
            delete item;
        } else {
            ++tileIter;
        }
    }
}

void GeoGraphicsScene::clear()
{
    const auto lists = d->m_tiledItems.values();
    for (auto const &list : lists) {
        qDeleteAll(list.values());
    }
    d->m_tiledItems.clear();
    d->m_features.clear();
}

void GeoGraphicsScene::addItem(GeoGraphicsItem *item)
{
    // Select zoom level so that the object fit in single tile
    int zoomLevel;
    qreal north, south, east, west;
    item->latLonAltBox().boundaries(north, south, east, west);
    for (zoomLevel = item->minZoomLevel(); zoomLevel >= 0; zoomLevel--) {
        if (TileId::fromCoordinates(GeoDataCoordinates(west, north, 0), zoomLevel) == TileId::fromCoordinates(GeoDataCoordinates(east, south, 0), zoomLevel))
            break;
    }

    const TileId key = TileId::fromCoordinates(GeoDataCoordinates(west, north, 0), zoomLevel); // same as GeoDataCoordinates(east, south, 0), see above

    auto &tileList = d->m_tiledItems[key];
    auto feature = item->feature();
    tileList.insert(feature, item);
    d->m_features.insert(feature, key);
}

}

#include "moc_GeoGraphicsScene.cpp"
