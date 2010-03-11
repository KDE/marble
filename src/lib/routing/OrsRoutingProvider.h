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

#ifndef ORS_ROUTING_PROVIDER_H
#define ORS_ROUTING_PROVIDER_H

#include "AbstractRoutingProvider.h"

#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace Marble {

/**
  * @brief A AbstractRoutingProvider that retrieves routing
  * directions from openrouteservice.org via http.
  */
class OrsRoutingProvider : public AbstractRoutingProvider
{
    Q_OBJECT

    enum Preference {
        Fastest,
        Shortest,
        Pedestrian
    };

    enum PointType {
        StartPoint,
        ViaPoint,
        EndPoint
    };

public:
    /** Constructor */
    explicit OrsRoutingProvider(QObject *parent = 0);

    /** Overload of AbstractRoutingProvider */
    virtual void retrieveDirections(const GeoDataLineString &route);

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData(QNetworkReply* reply);

    /** A network error occured */
    void handleError(QNetworkReply::NetworkError);

private:
    /** Builds the xml request header. */
    QString xmlHeader() const;

    /** Builds the route request header in the xml request. */
    QString requestHeader(DistanceUnit unit = Meter, Preference preference = Fastest ) const;

    /** Builds a route point substring. */
    QString requestPoint(PointType pointType, const GeoDataCoordinates &coordinates) const;

    /** Builds the route request footer in the xml request. */
    QString requestFooter() const;

    /** Builds the xml request footer. */
    QString xmlFooter() const;

    QNetworkAccessManager *m_networkAccessManager;
};

} // namespace Marble


#endif // ORS_ROUTING_PROVIDER_H
