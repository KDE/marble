// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
// SPDX-FileCopyrightText: 2010 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GEOMETRYLAYER_H
#define MARBLE_GEOMETRYLAYER_H

#include "GeoDataCoordinates.h"
#include "GeoDataRelation.h"
#include "LayerInterface.h"
#include <QObject>

class QAbstractItemModel;
class QModelIndex;
class QPoint;

namespace Marble
{
class GeoPainter;
class GeoDataFeature;
class GeoDataPlacemark;
class GeoDataRelation;
class StyleBuilder;
class ViewportParams;

class GeometryLayerPrivate;

class GeometryLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit GeometryLayer(const QAbstractItemModel *model, const StyleBuilder *styleBuilder);
    ~GeometryLayer() override;

    QStringList renderPosition() const override;

    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QLatin1String("NONE"), GeoSceneLayer *layer = nullptr) override;

    RenderState renderState() const override;

    QString runtimeTrace() const override;

    bool hasFeatureAt(const QPoint &curpos, const ViewportParams *viewport);

    QList<const GeoDataFeature *> whichFeatureAt(const QPoint &curpos, const ViewportParams *viewport);

    void highlightRouteRelation(qint64 osmId, bool enabled);

    void setVisibleRelationTypes(GeoDataRelation::RelationTypes relationTypes);

    void setLevelTagDebugModeEnabled(bool enabled);

    bool levelTagDebugModeEnabled() const;

    void setDebugLevelTag(int level);

    int debugLevelTag() const;

public Q_SLOTS:
    void addPlacemarks(const QModelIndex &index, int first, int last);
    void removePlacemarks(const QModelIndex &index, int first, int last);
    void resetCacheData();
    void setTileLevel(int tileLevel);

    /**
     * Finds all placemarks that contain the clicked point.
     *
     * The placemarks under the clicked position may
     * have their styleUrl set to a style map which
     * doesn't specify any highlight styleId. Such
     * placemarks will be fletered out in GeoGraphicsScene
     * and will not be highlighted.
     */
    void handleHighlight(qreal lon, qreal lat, GeoDataCoordinates::Unit unit);

Q_SIGNALS:
    void repaintNeeded();

    /**
     * @p selectedPlacemarks may contain placemarks which don't have
     * their styleUrl set to id of the style map which specifies
     * a highlight styleId. Such placemarks will be filtered out
     * in GeoGraphicsScene which will query for placemark->styleUrl()
     * to decide whether the placemark should be highlighted ot not.
     */
    void highlightedPlacemarksChanged(const QList<GeoDataPlacemark *> &clickedPlacemarks);

private:
    std::unique_ptr<GeometryLayerPrivate> d;
};

} // namespace Marble
#endif // MARBLE_GEOMETRYLAYER_H
