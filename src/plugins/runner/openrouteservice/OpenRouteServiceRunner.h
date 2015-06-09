//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMOPENROUTESERVICERUNNER_H
#define MARBLE_OSMOPENROUTESERVICERUNNER_H

#include "RoutingRunner.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Marble
{

class GeoDataCoordinates;

class OpenRouteServiceRunner : public RoutingRunner
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
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

    void get();

private:
    /** Builds the xml request header. */
    static QString xmlHeader();

    /** Builds the route request header in the xml request. */
    static QString requestHeader( const QString &unit, const QString &routePreference );

    /** Builds a route point substring. */
    static QString requestPoint( PointType pointType, const GeoDataCoordinates &coordinates );

    /** Builds the route request footer in the xml request. */
    static QString requestFooter( const QHash<QString, QVariant> &settings );

    /** Builds the xml request footer. */
    static QString xmlFooter();

    GeoDataDocument* parse( const QByteArray &input ) const;

    static RoutingInstruction::TurnType parseTurnType( const QString &text, QString* road );

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;

    QByteArray m_requestData;
};

}

#endif
