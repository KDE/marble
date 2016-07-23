//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin  <ps_ml@gmx.de>
// Copyright 2010      Thibaut Gridel  <tgridel@free.fr>
// Copyright 2011-2012 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#ifndef MARBLE_GEOMETRYLAYER_H
#define MARBLE_GEOMETRYLAYER_H

#include <QObject>
#include "LayerInterface.h"
#include "GeoDataCoordinates.h"

class QAbstractItemModel;
class QModelIndex;
class QPoint;

namespace Marble
{
class GeoPainter;
class GeoDataFeature;
class GeoDataPlacemark;
class StyleBuilder;
class ViewportParams;

class GeometryLayerPrivate;

class GeometryLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit GeometryLayer(const QAbstractItemModel *model, const StyleBuilder *styleBuilder);
    ~GeometryLayer();

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = QLatin1String("NONE"),
                         GeoSceneLayer * layer = 0 );

    RenderState renderState() const;

    virtual QString runtimeTrace() const;

    QVector<const GeoDataFeature*> whichFeatureAt( const QPoint& curpos, const ViewportParams * viewport );

    QVector<const GeoDataFeature*> whichBuildingAt(const QPoint& curpos, const ViewportParams * viewport);

public Q_SLOTS:
    void addPlacemarks( const QModelIndex& index, int first, int last );
    void removePlacemarks( const QModelIndex& index, int first, int last );
    void resetCacheData();

    /**
     * Finds all placemarks that contain the clicked point.
     *
     * The placemarks under the clicked position may
     * have their styleUrl set to a style map which
     * doesn't specify any highlight styleId. Such
     * placemarks will be fletered out in GeoGraphicsScene
     * and will not be highlighted.
     */
    void handleHighlight( qreal lon, qreal lat, GeoDataCoordinates::Unit unit );

Q_SIGNALS:
    void repaintNeeded();

    /**
     * @p selectedPlacemarks may contain placemarks which don't have
     * their styleUrl set to id of the style map which specifies
     * a highlight styleId. Such placemarks will be filtered out
     * in GeoGraphicsScene which will query for placemark->styleUrl()
     * to decide whether the placemark should be highlighted ot not.
     */
    void highlightedPlacemarksChanged( const QVector<GeoDataPlacemark*>& clickedPlacemarks );

private:
    GeometryLayerPrivate *d;
};

} // namespace Marble
#endif // MARBLE_GEOMETRYLAYER_H
