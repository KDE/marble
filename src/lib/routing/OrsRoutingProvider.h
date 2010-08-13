//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_ORSROUTINGPROVIDER_H
#define MARBLE_ORSROUTINGPROVIDER_H

#include "AbstractRoutingProvider.h"
#include "RouteSkeleton.h"

#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace Marble
{

/**
  * @brief A AbstractRoutingProvider that retrieves routing
  * directions from openrouteservice.org via http.
  */
class OrsRoutingProvider : public AbstractRoutingProvider
{
    Q_OBJECT

    enum PointType {
        StartPoint,
        ViaPoint,
        EndPoint
    };

public:
    /** Constructor */
    explicit OrsRoutingProvider( QObject *parent = 0 );

    /** Overload of AbstractRoutingProvider */
    virtual void retrieveDirections( RouteSkeleton *route );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:
    void retrieveDirections();

    /** Builds the xml request header. */
    QString xmlHeader() const;

    /** Builds the route request header in the xml request. */
    QString requestHeader( const QString &unit, const QString &routePreference ) const;

    /** Builds a route point substring. */
    QString requestPoint( PointType pointType, const GeoDataCoordinates &coordinates ) const;

    /** Builds the route request footer in the xml request. */
    QString requestFooter( RouteSkeleton::AvoidFeatures avoidFeatures ) const;

    /** Builds the xml request footer. */
    QString xmlFooter() const;

    QNetworkAccessManager *m_networkAccessManager;
    
    RouteSkeleton *m_route;

    QString m_languageCode;
};

} // namespace Marble


#endif
