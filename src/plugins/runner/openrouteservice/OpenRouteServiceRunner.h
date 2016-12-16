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

    // Overriding MarbleAbstractRunner
    void retrieveRoute( const RouteRequest *request ) override;

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

    void get();

private:
    static QString formatCoordinates(const GeoDataCoordinates &coordinates);

    GeoDataDocument* parse( const QByteArray &input ) const;

    static RoutingInstruction::TurnType parseTurnType( const QString &text, QString* road );

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
