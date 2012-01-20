//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//


#ifndef MARBLE_OSMOPENROUTESERVICERUNNER_H
#define MARBLE_OSMOPENROUTESERVICERUNNER_H

#include "MarbleAbstractRunner.h"
#include "routing/RouteRequest.h"

#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>

class QNetworkAccessManager;

namespace Marble
{

class OpenRouteServiceRunner : public MarbleAbstractRunner
{
    Q_OBJECT

    enum PointType {
        StartPoint,
        ViaPoint,
        EndPoint
    };

public:
    explicit OpenRouteServiceRunner(QObject *parent = 0);

    ~OpenRouteServiceRunner();

    // Overriding MarbleAbstractRunner
    GeoDataFeature::GeoDataVisualCategory category() const;

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

private:
    /** Builds the xml request header. */
    QString xmlHeader() const;

    /** Builds the route request header in the xml request. */
    QString requestHeader( const QString &unit, const QString &routePreference ) const;

    /** Builds a route point substring. */
    QString requestPoint( PointType pointType, const GeoDataCoordinates &coordinates ) const;

    /** Builds the route request footer in the xml request. */
    QString requestFooter( const QHash<QString, QVariant> &settings ) const;

    /** Builds the xml request footer. */
    QString xmlFooter() const;

    GeoDataDocument* parse( const QByteArray &input ) const;

    QNetworkAccessManager *m_networkAccessManager;
};

}

#endif
