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
class QColor;

namespace Marble
{
class GeoPainter;
class GeoDataFeature;
class ViewportParams;
class GeometryLayerPrivate;
class GeoDataPlacemark;

class GeometryLayer : public QObject, public LayerInterface
{
    Q_OBJECT
public:
    explicit GeometryLayer( const QAbstractItemModel *model );
    ~GeometryLayer();

    virtual QStringList renderPosition() const;

    virtual bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    RenderState renderState() const;

    virtual QString runtimeTrace() const;

    QVector<const GeoDataFeature*> whichFeatureAt( const QPoint& curpos, const ViewportParams * viewport );

public Q_SLOTS:
    void addPlacemarks( QModelIndex index, int first, int last );
    void removePlacemarks( QModelIndex index, int first, int last );
    void resetCacheData();
    void handleHighlight( qreal lon, qreal lat, GeoDataCoordinates::Unit unit );

Q_SIGNALS:
    void repaintNeeded();
    void highlightedPlacemarksChanged( const QVector<GeoDataPlacemark*>& clickedPlacemarks );

private:
    GeometryLayerPrivate *d;
};

} // namespace Marble
#endif // MARBLE_GEOMETRYLAYER_H
