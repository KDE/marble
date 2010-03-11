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

#include "RoutingLayer.h"

#include "MarbleWidget.h"
#include "RoutingModel.h"
#include "MarblePlacemarkModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoPainter.h"

#include <QtCore/QMap>
#include <QtCore/QDebug>
#include <QtGui/QPixmap>
#include <QtGui/QIcon>
#include <QtGui/QMouseEvent>
#include <QtGui/QAbstractProxyModel>
#include <QtGui/QItemSelectionModel>

namespace Marble {

class RoutingLayerPrivate
{
    struct PaintRegion {
        QPersistentModelIndex index;
        QRegion region;

        PaintRegion(const QModelIndex &index_, const QRegion &region_) :
                index(index_), region(region_)
        {
            // nothing to do
        }
    };

public:
    RoutingLayer *q;

    QList<PaintRegion> m_regions;

    QList<PaintRegion> m_placemarks;

    QRegion m_routeRegion;

    QList<QRegion> m_infoRegions;

    QAbstractProxyModel *m_proxyModel;

    QPersistentModelIndex m_movingIndex;

    MarbleWidget *m_marbleWidget;

    QPixmap m_targetPixmap;

    QPixmap m_viaPixmap;

    QRect m_movingIndexDirtyRect;

    QPoint m_insertStopOver;

    bool m_dragStopOver;

    bool m_pointSelection;

    RoutingModel* m_routingModel;

    MarblePlacemarkModel* m_placemarkModel;

    QItemSelectionModel * m_selectionModel;

    bool m_routeDirty;

    QSize m_pixmapSize;

    /** Constructor */
    explicit RoutingLayerPrivate(RoutingLayer* parent, MarbleWidget* widget);

    // The following methods are mostly only called at one place in the code, but often
    // Inlined to avoid the function call overhead. Having functions here is just to
    // keep the code clean

    /** Paint icons for each placemark in the placemark model */
    inline void renderPlacemarks( GeoPainter *painter);

    /** Paint waypoint polygon, icons for trip points etc */
    inline void renderRoute(GeoPainter* painter);

    /** Insert via points or emit position signal, if appropriate */
    inline bool handleMouseButtonRelease(QMouseEvent* e);

    /** Select route instructions points, start dragging trip points */
    inline bool handleMouseButtonPress(QMouseEvent* e);

    /** Dragging trip points, route polygon hovering */
    inline bool handleMouseMove(QMouseEvent* e);

    /** True if the given point (screen coordinates) is among the route instruction points */
    inline bool isInfoPoint(const QPoint &point);
};

RoutingLayerPrivate::RoutingLayerPrivate(RoutingLayer* parent, MarbleWidget* widget) :
  q(parent), m_proxyModel(0), m_marbleWidget(widget), m_targetPixmap(":/data/bitmaps/routing_pick.png"),
  m_viaPixmap(":/data/bitmaps/routing_via.png"), m_dragStopOver(false), m_pointSelection(false),
  m_routingModel(0), m_placemarkModel(0), m_selectionModel(0), m_routeDirty(false), m_pixmapSize(22,22)
{
    // nothing to do
}

void RoutingLayerPrivate::renderPlacemarks( GeoPainter *painter)
{
    m_placemarks.clear();
    painter->setPen(QColor(Qt::black));
    for (int i=0; i<m_placemarkModel->rowCount(); ++i) {
        QModelIndex index = m_placemarkModel->index(i,0);
        QVariant data = index.data(MarblePlacemarkModel::CoordinateRole);
        if (index.isValid() && !data.isNull()) {
            GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>(data);

            QPixmap pixmap = qVariantValue<QPixmap>(index.data(Qt::DecorationRole));
            if (!pixmap.isNull() && m_selectionModel->isSelected(index)) {
                QIcon selected = QIcon(pixmap);
                QPixmap result = selected.pixmap(m_pixmapSize, QIcon::Selected, QIcon::On);
                painter->drawPixmap(pos, result);
            } else {
                painter->drawPixmap(pos, pixmap);
            }

            QRegion region = painter->regionFromRect(pos, m_targetPixmap.width(), m_targetPixmap.height());
            m_placemarks.push_back(PaintRegion(index,region));
        }
    }
}

void RoutingLayerPrivate::renderRoute(GeoPainter* painter)
{
    m_regions.clear();
    m_infoRegions.clear();
    GeoDataLineString waypoints;

    for (int i=0; i<m_routingModel->rowCount(); ++i)
    {
        QModelIndex index = m_routingModel->index(i,0);
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>(index.data(RoutingModel::TypeRole));

        if (type == RoutingModel::WayPoint)
        {
            waypoints << pos;
        }
    }

    QPen bluePen(QColor::fromRgb(0,87,174,200)); // blue, oxygen palette
    bluePen.setWidth(5);
    if (m_routeDirty) {
        bluePen.setStyle(Qt::DotLine);
    }
    painter->setPen(bluePen);

    painter->drawPolyline(waypoints);
    m_routeRegion = painter->regionFromPolyline(waypoints, 8);

    bluePen.setWidth(2);
    painter->setPen(bluePen);
    painter->setBrush(QBrush(QColor::fromRgb(136,138,133,200))); // gray, oxygen palette

    if (!m_insertStopOver.isNull()) {
        int dx = 1 + m_pixmapSize.width() / 2;
        int dy = 1 + m_pixmapSize.height() / 2;
        QPoint center = m_insertStopOver - QPoint(dx,dy);
        painter->drawPixmap(center, m_targetPixmap);
    }

    QList<QModelIndex> points;

    for (int i=0; i<m_routingModel->rowCount(); ++i)
    {
        QModelIndex index = m_routingModel->index(i,0);
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>(index.data(RoutingModel::TypeRole));

        if (type == RoutingModel::Start || type == RoutingModel::Destination || type == RoutingModel::Via ) {
            points.push_back(index);
        }

        if (type == RoutingModel::Instruction) {

            painter->setBrush(QBrush(QColor::fromRgb(136,138,133,200))); // gray, oxygen palette
            QModelIndex proxyIndex = m_proxyModel->mapFromSource(index);
            if (m_selectionModel->selection().contains(proxyIndex))
            {
                painter->setPen(QColor(Qt::black));
                painter->setBrush(QBrush(QColor::fromRgb(227,173,0,100))); // yellow, oxygen palette
                painter->drawAnnotation(pos, index.data().toString(), QSize(120,60), 10, 30, 15, 15 );

                painter->setPen(bluePen);
                painter->setBrush(QBrush(QColor::fromRgb(236,115,49,200))); // orange, oxygen palette
            }

            QRegion region = painter->regionFromEllipse(pos, 12, 12);
            m_infoRegions.push_back(region);
            m_regions.push_front(PaintRegion(index, region));
            painter->drawEllipse(pos, 8, 8);
        }
    }

    foreach(const QModelIndex &index, points ) {
        GeoDataCoordinates pos = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
        QPixmap pixmap = qVariantValue<QPixmap>(index.data(Qt::DecorationRole));
        if (pixmap.isNull())
            pixmap = m_viaPixmap;
        painter->drawPixmap(pos, pixmap);
        QRegion region = painter->regionFromRect(pos, pixmap.width(), pixmap.height());
        m_regions.push_front(PaintRegion(index,region));
    }
}

bool RoutingLayerPrivate::handleMouseButtonPress(QMouseEvent* e)
{
    if (m_pointSelection) {
        return true;
    }

    foreach(const PaintRegion &region, m_regions) {
        if (region.region.contains(e->pos())) {
            QModelIndex index = m_proxyModel->mapFromSource(region.index);
            RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>(index.data(RoutingModel::TypeRole));
            if (type == RoutingModel::Instruction) {
                m_selectionModel->select(index, QItemSelectionModel::ClearAndSelect);
            } else {
                m_movingIndex = region.index;
            }
            m_insertStopOver = QPoint();
            m_dragStopOver = false;
            return true;
        }
    }

    if (m_routeRegion.contains(e->pos())) {
        /** @todo: Determine the neighbored via points and insert in order */
        m_insertStopOver = e->pos();
        m_dragStopOver = true;
        return true;
    }

    foreach(const PaintRegion &region, m_placemarks) {
        if (region.region.contains(e->pos())) {
            emit q->placemarkSelected(region.index);
            return true;
        }
    }

    return false;
}

bool RoutingLayerPrivate::handleMouseButtonRelease(QMouseEvent* e)
{
    if (m_pointSelection) {
        qreal lon(0.0), lat(0.0);
        if (m_marbleWidget->geoCoordinates(e->pos().x(), e->pos().y(),
                                           lon, lat, GeoDataCoordinates::Radian)) {
            emit q->pointSelected(GeoDataCoordinates(lon,lat));
            return true;
        }
    }

    m_movingIndex = QModelIndex();

    if (!m_insertStopOver.isNull()) {
        qreal lon(0.0), lat(0.0);
        if (m_marbleWidget->geoCoordinates(m_insertStopOver.x(), m_insertStopOver.y(), lon, lat, GeoDataCoordinates::Radian)) {
            GeoDataCoordinates position(lon, lat);
            m_routingModel->addVia(position);
        }
    }

    m_insertStopOver = QPoint();
    m_dragStopOver = false;
    return false;
}

bool RoutingLayerPrivate::handleMouseMove(QMouseEvent* e)
{
    if (m_pointSelection) {
        m_marbleWidget->setCursor(Qt::CrossCursor);
        return true;
    }

    if (!m_routingModel) {
        return false;
    }

    qreal lon(0.0), lat(0.0);
    if (m_marbleWidget->geoCoordinates(e->pos().x(), e->pos().y(),
                                       lon, lat, GeoDataCoordinates::Radian)) {

        if (m_movingIndex.isValid()) {
            QVariant pos;
            GeoDataCoordinates moved(lon,lat);
            qVariantSetValue<GeoDataCoordinates>(pos, moved);
            m_routingModel->setData(m_movingIndex, pos, RoutingModel::CoordinateRole);
            m_marbleWidget->setCursor(Qt::ArrowCursor);
        }
        else if (m_dragStopOver) {
            m_insertStopOver = e->pos();
            m_marbleWidget->setCursor(Qt::ArrowCursor);
        } else if (isInfoPoint(e->pos())) {
            m_marbleWidget->setCursor(Qt::ArrowCursor);
        } else if (m_routeRegion.contains(e->pos())) {
            m_insertStopOver = e->pos();
            m_marbleWidget->setCursor(Qt::ArrowCursor);
        } else if (!m_insertStopOver.isNull()) {
            m_insertStopOver = QPoint();
        } else {
            return false;
        }

        // Update pixmap in the map (old and new position needs repaint)
        m_marbleWidget->repaint(m_movingIndexDirtyRect);
        QRect dirty(e->pos(), m_pixmapSize);
        int dx = 1 + m_pixmapSize.width() / 2;
        int dy = 1 + m_pixmapSize.height() / 2;
        dirty.adjust( -dx, -dy, -dx, -dy );
        m_marbleWidget->repaint(dirty);
        m_movingIndexDirtyRect = dirty;
        return true;
    }

    return false;
}

bool RoutingLayerPrivate::isInfoPoint(const QPoint &point)
{
    foreach(const QRegion &region, m_infoRegions) {
        if (region.contains(point))
            return true;
    }

    return false;
}

RoutingLayer::RoutingLayer(MarbleWidget* widget, QWidget * parent) :
        QObject(parent), d(new RoutingLayerPrivate(this, widget))
{
    widget->installEventFilter(this);
}

RoutingLayer::~RoutingLayer()
{
    delete d;
}

QStringList RoutingLayer::renderPosition() const
{
    return QStringList() << "HOVERS_ABOVE_SURFACE";
}

bool RoutingLayer::render( GeoPainter *painter, ViewportParams *viewport,
                                 const QString& renderPos, GeoSceneLayer * layer )
{
    Q_UNUSED(viewport)
    Q_UNUSED(renderPos)
    Q_UNUSED(layer)

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (d->m_placemarkModel)
        d->renderPlacemarks(painter);

    if (d->m_routingModel)
        d->renderRoute(painter);

    painter->restore();
    return true;
}

bool RoutingLayer::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        return d->handleMouseButtonPress(e);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        return d->handleMouseButtonRelease(e);
    }

    if (event->type() == QEvent::MouseMove ) {
        QMouseEvent* e = static_cast<QMouseEvent*>(event);
        return d->handleMouseMove(e);
    }

    return false;
}

void RoutingLayer::setModel ( RoutingModel* model )
{
    d->m_placemarkModel = 0;
    d->m_routingModel = model;
}

void RoutingLayer::setModel ( MarblePlacemarkModel* model )
{
    d->m_routingModel = 0;
    d->m_placemarkModel = model;
}

void RoutingLayer::synchronizeWith( QAbstractProxyModel *model, QItemSelectionModel* selection )
{
    d->m_selectionModel = selection;
    d->m_proxyModel = model;
}

void RoutingLayer::setPointSelectionEnabled(bool enabled)
{
    d->m_pointSelection = enabled;
}

void RoutingLayer::setRouteDirty(bool dirty)
{
    d->m_routeDirty = dirty;

    /** @todo: The full repaint can be avoided. The route however has changed
      * and the exact bounding box needs to be recalculated before doing
      * a partly repaint, otherwise we might end up repainting only parts of the route
      */
    // d->m_marbleWidget->repaint(d->m_routeRegion);
    d->m_marbleWidget->repaint();
}

} // namespace Marble

#include "RoutingLayer.moc"
