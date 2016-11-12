//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//


#ifndef MARBLE_OSMOSRMRUNNER_H
#define MARBLE_OSMOSRMRUNNER_H

#include "RoutingRunner.h"
#include "GeoDataCoordinates.h"
#include "routing/instructions/RoutingInstruction.h"

#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace Marble
{

class GeoDataLineString;

class OSRMRunner : public RoutingRunner
{
    Q_OBJECT

public:
    explicit OSRMRunner(QObject *parent = 0);

    ~OSRMRunner();

    // Overriding MarbleAbstractRunner
    virtual void retrieveRoute( const RouteRequest *request );

private Q_SLOTS:
    /** Route data was retrieved via http */
    void retrieveData( QNetworkReply *reply );

    /** A network error occurred */
    void handleError( QNetworkReply::NetworkError );

    void get();

private:
    static void append( QString* input, const QString &key, const QString &value );

    static GeoDataLineString* decodePolyline( const QString &geometry );

    static RoutingInstruction::TurnType parseTurnType( const QString &instruction );

    GeoDataDocument* parse( const QByteArray &input ) const;

    QNetworkAccessManager m_networkAccessManager;

    QNetworkRequest m_request;
};

}

#endif
