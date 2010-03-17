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
#include "RouteSkeleton.h"
#include "MarbleDebug.h"
#include "OrsRoutingProvider.h"
#include "MarbleWidget.h"

namespace Marble {

class RoutingManagerPrivate
{
public:
    RoutingModel* m_routingModel;

    AbstractRoutingProvider *m_routingProvider;

    MarbleWidget *m_marbleWidget;

    RoutingManagerPrivate(MarbleWidget *widget, QObject *parent);

    RouteSkeleton* m_route;
};

RoutingManagerPrivate::RoutingManagerPrivate(MarbleWidget *widget, QObject *parent) :
        m_routingModel(new RoutingModel(parent)),
        m_routingProvider(new OrsRoutingProvider(parent)),
        m_marbleWidget(widget), m_route(0)
{
    // nothing to do
}

RoutingManager::RoutingManager(MarbleWidget *widget, QObject *parent) : QObject(parent),
d(new RoutingManagerPrivate(widget, this))
{
    connect(d->m_routingProvider, SIGNAL(routeRetrieved(AbstractRoutingProvider::Format, QByteArray)),
            this, SLOT(setRouteData(AbstractRoutingProvider::Format, QByteArray)));
}

RoutingManager::~RoutingManager()
{
    delete d;
}

RoutingModel* RoutingManager::routingModel()
{
    return d->m_routingModel;
}

void RoutingManager::retrieveRoute(RouteSkeleton* route)
{
    d->m_route = route;
    updateRoute();
}

void RoutingManager::setRouteData(AbstractRoutingProvider::Format format, const QByteArray &data)
{
    /** @todo: switch to using GeoDataDocument* */
    Q_UNUSED(format);

    d->m_routingModel->importOpenGis(data);
    d->m_marbleWidget->repaint();

    emit stateChanged(Retrieved, d->m_route);
}

void RoutingManager::updateRoute()
{
    if (d->m_route) {
        emit stateChanged(Downloading, d->m_route);
        d->m_routingProvider->retrieveDirections(d->m_route);
    }
}

} // namespace Marble

#include "RoutingManager.moc"
