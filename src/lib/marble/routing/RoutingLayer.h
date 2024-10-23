// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Dennis Nienhüser <nienhueser@kde.org>
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
class RoutingLayer : public QObject, public LayerInterface
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param widget The marble widget used for geopos <-> screenpos transformations
     *   and repainting of (small) areas. Must not be null
     * @param parent Optional parent widget
     */
    explicit RoutingLayer(MarbleWidget *widget, QWidget *parent = nullptr);

    /** Destructor */
    ~RoutingLayer() override;

    /** Reimplemented from LayerInterface. We'll hover above the surface */
    QStringList renderPosition() const override;

    /** Reimplemented from LayerInterface. */
    qreal zValue() const override;

    /** Reimplemented from LayerInterface. Paints route items and placemarks */
    bool render(GeoPainter *painter, ViewportParams *viewport, const QString &renderPos = QStringLiteral("NONE"), GeoSceneLayer *layer = nullptr) override;

    RenderState renderState() const override;

    /**
     * Set the proxy model another QAbstractItemView uses that should share
     * its selection model with us. Needed because this class uses an unfiltered
     * model which has different indices than a filtered one.
     */
    void synchronizeWith(QItemSelectionModel *selection);

    /**
     * Set the placemark model to use. Implicitly removes the routing model.
     */
    void setPlacemarkModel(MarblePlacemarkModel *model);

    /**
     * Set the view context to determine whether the map is used interactively
     */
    void setViewContext(ViewContext viewContext);

    /**
     * Determine whether the route can be edited by the user (via points added,
     * route cleared)
     */
    void setInteractive(bool interactive);

    /**
     * Returns whether the route is interactive (true by default if not changed
     * by setInteractive)
     */
    bool isInteractive() const;

    QString runtimeTrace() const override;

Q_SIGNALS:
    /**
     * A placemark was selected (clicked) by the user. The index belongs to
     * the model set via setModel
     */
    void placemarkSelected(const QModelIndex &index);

    void repaintNeeded(const QRect &rect = QRect());

public:
    /** Overriding QWidget, used to make the layer interactive */
    bool eventFilter(QObject *obj, QEvent *event) override;

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
