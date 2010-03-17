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

#ifndef MARBLE_ABSTRACT_ROUTING_PROVIDER_H
#define MARBLE_ABSTRACT_ROUTING_PROVIDER_H

#include "GeoDataLineString.h"

#include <QtCore/QObject>
#include <QtCore/QByteArray>

namespace Marble {

class RouteSkeleton;

class AbstractRoutingProvider : public QObject
{
    Q_OBJECT

public:
    enum Format {
      OpenGIS,
      GPX,
      KML
    };

    /** Constructor */
    AbstractRoutingProvider(QObject *parent);

    /** Destructor */
    virtual ~AbstractRoutingProvider();

    /**
      * Retrieve routing directions. Upon success, the routeRetrieved signal should
      * be emitted.
      * @param route Set of points to include in the route. Size will be at least two,
      * first one is to be interpreted as source, last as destination, others (possibly
      * empty) as stopover points.
      */
    virtual void retrieveDirections(RouteSkeleton* route) = 0;

Q_SIGNALS:
    void routeRetrieved(AbstractRoutingProvider::Format format, const QByteArray &data);
};

} // namespace Marble

Q_DECLARE_METATYPE( Marble::AbstractRoutingProvider::Format )

#endif // MARBLE_ABSTRACT_ROUTING_PROVIDER_H
