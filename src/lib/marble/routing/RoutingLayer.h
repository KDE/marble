//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_ROUTINGLAYER_H
#define MARBLE_ROUTINGLAYER_H

#include "LayerInterface.h"

#include "MarbleGlobal.h"

#include <QObject>
#include <QRect>

class QItemSelectionModel;
class QModelIndex;

namespace Marble
{

class MarbleWidget;
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
    explicit RoutingLayer( MarbleWidget *widget, QWidget *parent = 0 );

    /** Destructor */
    ~RoutingLayer();

    /** Reimplemented from LayerInterface. We'll hover above the surface */
    QStringList renderPosition() const;

    /** Reimplemented from LayerInterface. */
    qreal zValue() const;

    /** Reimplemented from LayerInterface. Paints route items and placemarks */
    bool render( GeoPainter *painter, ViewportParams *viewport,
                 const QString &renderPos = "NONE", GeoSceneLayer *layer = 0 );

    RenderState renderState() const;

    /**
      * Set the proxy model another QAbstractItemView uses that should share
      * its selection model with us. Needed because this class uses an unfiltered
      * model which has different indices than a filtered one.
      */
    void synchronizeWith( QItemSelectionModel *selection );

    /**
      * Set the placemark model to use. Implicitly removes the routing model.
      */
    void setPlacemarkModel ( MarblePlacemarkModel *model );

    /**
      * Set the view context to determine whether the map is used interactively
      */
    void setViewContext( ViewContext viewContext );

    /**
     * Determine whether the route can be edited by the user (via points added,
     * route cleared)
     */
    void setInteractive( bool interactive );

    /**
     * Returns whether the route is interactive (true by default if not changed
     * by setInteractive)
     */
    bool isInteractive() const;

    QString runtimeTrace() const;

Q_SIGNALS:
    /**
      * A placemark was selected (clicked) by the user. The index belongs to
      * the model set via setModel
      */
    void placemarkSelected( const QModelIndex &index );

    void repaintNeeded( const QRect &rect = QRect() );

public:
    /** Overriding QWidget, used to make the layer interactive */
    bool eventFilter( QObject *obj, QEvent *event );

private Q_SLOTS:
    void removeViaPoint();

    void showAlternativeRoutes();

    /** Export route to a file */
    void exportRoute();

    /**
      * Paint a dashed route when downloading a new route, a solid one otherwise.
      */
    void updateRouteState();

    /**
      * The viewport has changed, recalculate positions accordingly
      */
    void setViewportChanged();

private:
    RoutingLayerPrivate *const d;
    friend class RoutingLayerPrivate;

};

} // namespace Marble

#endif
