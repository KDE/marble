//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

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
    virtual void retrieveDirections(RouteSkeleton* route);

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
