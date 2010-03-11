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

#include "RoutingManager.h"

#include "RoutingModel.h"
#include "MarbleDebug.h"
#include "OrsRoutingProvider.h"
#include "MarbleWidget.h"

#include <QtCore/QTimer>

namespace Marble {

class RoutingManagerPrivate
{
public:
    RoutingModel* m_routingModel;

    AbstractRoutingProvider *m_routingProvider;

    MarbleWidget *m_marbleWidget;

    RoutingManagerPrivate(MarbleWidget *widget, QObject *parent);

    GeoDataLineString m_route;

    QTimer m_autoUpdate;
};

RoutingManagerPrivate::RoutingManagerPrivate(MarbleWidget *widget, QObject *parent) :
        m_routingModel(new RoutingModel(parent)),
        m_routingProvider(new OrsRoutingProvider(parent)),
        m_marbleWidget(widget)
{
    m_autoUpdate.setInterval(500);
    m_autoUpdate.setSingleShot(true);
}

RoutingManager::RoutingManager(MarbleWidget *widget, QObject *parent) : QObject(parent),
d(new RoutingManagerPrivate(widget, this))
{
    connect(d->m_routingProvider, SIGNAL(routeRetrieved(AbstractRoutingProvider::Format, QByteArray)),
            this, SLOT(setRouteData(AbstractRoutingProvider::Format, QByteArray)));
    connect(d->m_routingModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(scheduleRouteUpdate()));
    connect(d->m_routingModel, SIGNAL(rowsInserted(QModelIndex,int,int)),
            this, SLOT(updateRoute()));
    connect(&d->m_autoUpdate, SIGNAL(timeout()), this, SLOT(updateRoute()));
}

RoutingManager::~RoutingManager()
{
    delete d;
}

RoutingModel* RoutingManager::routingModel()
{
    return d->m_routingModel;
}

void RoutingManager::retrieveRoute(const GeoDataLineString &route)
{
    d->m_route = route;
    emit stateChanged(Downloading, d->m_route);
    d->m_routingProvider->retrieveDirections(route);
}

void RoutingManager::setRouteData(AbstractRoutingProvider::Format format, const QByteArray &data)
{
    /** @todo: switch to using GeoDataDocument* */
    Q_UNUSED(format);

    d->m_routingModel->importOpenGis(data, d->m_route);
    d->m_marbleWidget->repaint();

    emit stateChanged(Retrieved, d->m_route);
}

void RoutingManager::scheduleRouteUpdate()
{
    /** @todo: Assumes that only source and destination position can be changed.
      * Needs to be checked for when that changes
      */
    d->m_autoUpdate.start(); // Use a timer since it is called repeatedly while dragging
}

void RoutingManager::updateRoute()
{
    d->m_autoUpdate.stop();
    GeoDataLineString route;

    for (int i=0; i<routingModel()->rowCount(); ++i) {
        QModelIndex index = routingModel()->index(i,0);
        RoutingModel::RoutingItemType type = qVariantValue<RoutingModel::RoutingItemType>(index.data(RoutingModel::TypeRole));
        if (type == RoutingModel::Start) {
            GeoDataCoordinates source = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
            route.append(source);
        }

        if (type == RoutingModel::Via) {
            GeoDataCoordinates via = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
            route.append(via);
        }

        if (type == RoutingModel::Destination) {
            GeoDataCoordinates destination = qVariantValue<GeoDataCoordinates>(index.data(RoutingModel::CoordinateRole));
            route.append(destination);
        }
    }

    retrieveRoute(route);
}

} // namespace Marble

#include "RoutingManager.moc"
