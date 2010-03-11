/*
    Copyright 2010 Dennis Nienhüser <earthwings@gentoo.org>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) version 3, or any
    later version accepted by the membership of KDE e.V. (or its
    successor approved by the membership of KDE e.V.), which shall
    act as a proxy defined in Section 6 of version 3 of the license.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MARBLE_ROUTING_LAYER_H
#define MARBLE_ROUTING_LAYER_H

#include "LayerInterface.h"
#include "GeoDataCoordinates.h"

#include <QtCore/QModelIndex>

class QAbstractProxyModel;
class QItemSelectionModel;

namespace Marble {

class MarbleWidget;
class RoutingModel;
class MarblePlacemarkModel;
class RoutingLayerPrivate;

/**
  * @brief A paint layer that serves as a view on a route model
  */
class RoutingLayer: public QObject, public LayerInterface
{
    Q_OBJECT

public:
    /**
      * @brief Constructor
      * @param widget The marble widget used for geopos <-> screenpos transformations
      *   and repainting of (small) areas. Must not be null
      * @param parent Optional parent widget
      */
    explicit RoutingLayer(MarbleWidget* widget, QWidget * parent = 0 );

    /** Destructor */
    ~RoutingLayer();

    /** Reimplemented from LayerInterface. We'll hover above the surface */
    QStringList renderPosition() const;

    /** Reimplemented from LayerInterface. Paints route items and placemarks */
    bool render( GeoPainter *painter, ViewportParams *viewport,
                         const QString& renderPos = "NONE", GeoSceneLayer * layer = 0 );

    /**
      * Set the proxy model another QAbstractItemView uses that should share
      * its selection model with us. Needed because this class uses an unfiltered
      * model which has different indices than a filtered one.
      */
    void synchronizeWith( QAbstractProxyModel *model, QItemSelectionModel* selection );

    /**
      * Set the routing model to use. Implicitly removes the placemark model.
      */
    void setModel ( RoutingModel* model );

    /**
      * Set the placemark model to use. Implicitly removes the routing model.
      */
    void setModel ( MarblePlacemarkModel* model );

    /**
      * When enabled, a cross hair cursor is shown in the map and each click
      * in the map is reported via the pointSelected signal. Mouse move,
      * press and release events are fully eaten to avoid things like context
      * menus chiming in.
      */
    void setPointSelectionEnabled(bool enabled);

    /**
      * The route is dirty (needs an update) and should be painted to indicate that
      */
    void setRouteDirty(bool dirty);

Q_SIGNALS:
    /**
      * A placemark was selected (clicked) by the user. The index belongs to
      * the model set via setModel
      */
    void placemarkSelected(const QModelIndex &index);

    /**
      * A point was selected by a mouse click after setPointSelectionEnabled(true) was called
      */
    void pointSelected(const GeoDataCoordinates &coordinates);   

    /**
      * Selection of points was aborted by the user without selecting a point
      */
    void pointSelectionAborted();

protected:
    /** Overriding QWidget, used to make the layer interactive */
    bool eventFilter(QObject *obj, QEvent *event);

private:
    RoutingLayerPrivate* const d;
    friend class RoutingLayerPrivate;

};

} // namespace Marble

#endif // MARBLE_ROUTING_LAYER_H
